/*
 * DropletDetector.cpp
 *
 *  Created on: Jul 8, 2016
 *      Author: JP BATRA Work
 */

#include "DropletDetector.h"
#include <android/log.h>
#include <thread>
#include <string>
#include <sstream>

#define LOGI(TAG,...) __android_log_print(ANDROID_LOG_INFO   , TAG,__VA_ARGS__)

Mat RED_THRESH;
Mat CAMERA_FRAME;
Mat DARK_FRAME;

DropletDetector::DropletDetector() {
	// TODO Auto-generated constructor stub
}

DropletDetector::~DropletDetector() {
	// TODO Auto-generated destructor stub
}

void DropletDetector::setFrame(Mat& cameraFrame, Mat& darkFrame) {
	//frame to be return with AR overlay
	CAMERA_FRAME = cameraFrame;
	//frame to be processed in order to detect and track Droplets
	DARK_FRAME = darkFrame;
}

void DropletDetector::onTouch(Point p) {
	//if user touches close to a Droplet, toggle that Droplet's showinfo status
	int size = ALL_DROPLETS.size();
	for(int i = 0; i < size; i++) {
		if(distance(p, ALL_DROPLETS[i].location) <= 50) {
			ALL_DROPLETS[i].showInfo = !ALL_DROPLETS[i].showInfo;
		}
	}
}

void DropletDetector::morphOps(Mat& binFrame) {
	//remove background noise and bad light sources. Smooth the image
	Mat erodeElement = getStructuringElement(MORPH_RECT, Size(3, 3));
	Mat dilateElement = getStructuringElement(MORPH_RECT, Size(5, 5));

	erode(binFrame, binFrame, erodeElement);
	dilate(binFrame, binFrame, dilateElement);
	dilate(binFrame, binFrame, dilateElement);
}

void DropletDetector::processBinary(Mat& binaryMat) {
	//find Droplets' contours in binary image matrix
	Mat clone;
	binaryMat.copyTo(clone);
	vector<vector<Point>> contours;

	findContours(clone, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

	if (!contours.empty()) {
		drawContours(binaryMat, contours, -1, Scalar(255), CV_FILLED);
	}
}

void DropletDetector::drawDroplet(Droplet droplet) {
	ellipse(CAMERA_FRAME, droplet.getRotatedRect(), Scalar(0, 255, 0), 4);
	putText(CAMERA_FRAME, droplet.getName(), Point(droplet.getXPos() - 10, droplet.getYPos() + 10), 1, 2, droplet.getColor(), 3);
	if(droplet.showInfo) {
		ostringstream eNeg;
		ostringstream aNumber;
		eNeg << "ENeg: " << droplet.getENeg();
		aNumber << "Atomic #" << droplet.getAtomicNumber();
		putText(CAMERA_FRAME, eNeg.str(), Point(droplet.getXPos() - 30, droplet.getYPos() + 50), 1, 1, Scalar(0, 255, 0), 1);
		putText(CAMERA_FRAME, aNumber.str(), Point(droplet.getXPos() - 30, droplet.getYPos() + 70), 1, 1, Scalar(0, 255, 0), 1);
	}
}

int DropletDetector::minElectronegativity(vector<Droplet> syncedDroplets) {
	//find Droplet in group of bonded Droplets with smallest electronegativity. This will be the center Droplet
	double minE = (double)INT_MAX;
	int indexOfMin = 0;
	int size = syncedDroplets.size();
	for (int i = 0; i < size; i++) {

		if (syncedDroplets[i].getName() != "H" && syncedDroplets[i].getENeg() < minE) {
			indexOfMin = i;
			minE = syncedDroplets[i].getENeg();
		}
	}

	return indexOfMin;
}

void DropletDetector::drawMolecule(vector<Droplet> syncedDroplets, Mat& cameraFeed) {
	//draw lines between synced Droplets indicating they are in a molecule
	int indexOfCenter = minElectronegativity(syncedDroplets);
	Droplet center = syncedDroplets[indexOfCenter];

	if (syncedDroplets.size() > 2) {
		int size = syncedDroplets.size();
		for (int i = 0; i < size; i++) {

			if (i != indexOfCenter) {
				Point p1(center.getXPos(), center.getYPos());
				Point p2(syncedDroplets[i].getXPos(), syncedDroplets[i].getYPos());
				line(cameraFeed, p1, p2, Scalar(0, 255, 0), 3);
			}
		}
	}
	else {
		Point p1(syncedDroplets[0].getXPos(), syncedDroplets[0].getYPos());
		Point p2(syncedDroplets[1].getXPos(), syncedDroplets[1].getYPos());
		line(cameraFeed, p1, p2, Scalar(0, 255, 0), 3);
	}
}

bool DropletDetector::isFlashing(Droplet &droplet) {

	int value = RED_THRESH.at<uchar>(droplet.getYPos(), droplet.getXPos());

	if (value == 255) {
		droplet.t.reset();
		return true;
	}
	return false;
}

void DropletDetector::rotatePoint(Point& p, const Point center, double angle) {
	int shiftedX = p.x - center.x;
	int shiftedY = p.y - center.y;

	double rotatedX = (cos(angle)*shiftedX + sin(angle)*shiftedY);
	double rotatedY = (-sin(angle)*shiftedX + cos(angle)*shiftedY);

	p.x = rotatedX + center.x;
	p.y = rotatedY + center.y;
}

int DropletDetector::distance(Droplet d1, Droplet d2) {
	int dx = d1.getXPos() - d2.getXPos();
	int dy = d1.getYPos() - d2.getYPos();
	int distance = sqrt(pow(dx, 2) + pow(dy, 2));
	return distance;
}

int DropletDetector::distance(Point p1, Point p2) {
	int dx = p1.x - p2.x;
	int dy = p1.y - p2.y;
	int distance = sqrt(pow(dx, 2) + pow(dy, 2));
	return distance;
}

void DropletDetector::cleanup(Mat grayBinary) {
	//If Droplets move, their new positions are sometimes not updated. This function does checks all Droplets to make sure there aren't any "ghost" Droplets
	int size = ALL_DROPLETS.size();
	for (int i = 0; i < size; i++) {
		int val = grayBinary.at<uchar>(ALL_DROPLETS[i].getYPos(),
				ALL_DROPLETS[i].getXPos());
		if (val != 255) {
			ALL_DROPLETS.erase(ALL_DROPLETS.begin() + i);
		}
	}
}

void DropletDetector::processDroplet(Droplet droplet, bool& exists) {
	//checks to see if a Droplet in each new frame was the same Droplet from the previous frame, since Droplets are re-detected on every frame
	int size = ALL_DROPLETS.size();
	for (int i = 0; i < size; i++) {

		if(distance(ALL_DROPLETS[i], droplet) < DISTANCE && droplet.getName() == "H") {
			droplet.setName(ALL_DROPLETS[i].getName());
		}
		if (ALL_DROPLETS[i].getName() == droplet.getName() && distance(ALL_DROPLETS[i], droplet) < DISTANCE) {
			exists = true;
			droplet.t.start = ALL_DROPLETS[i].t.start;
			droplet.showInfo = ALL_DROPLETS[i].showInfo;
			ALL_DROPLETS[i] = droplet;

		}
	}
	if (!exists) {
		ALL_DROPLETS.push_back(droplet);
	}
}

void DropletDetector::sortBondedDroplets(vector<Droplet> bondedDroplets) {
	//Droplets that flash red are bonded to other Droplets. This function checks to see which Droplets are bonded together, and sorts them into molecules
	vector < vector<Droplet> > molecules;
	int bSize = bondedDroplets.size();
	for (int i = 0; i < bSize; i++) {

		if (!bondedDroplets[i].sorted) {
			bondedDroplets[i].sorted = true;
			vector<Droplet> molecule;
			molecule.push_back(bondedDroplets[i]);

			for (int j = i + 1; j < bSize; j++) {
				//if two Droplets flash red at roughly the same time, they are bonded together.
				if (abs((int)bondedDroplets[i].t.ms_ellapsed() - (int)bondedDroplets[j].t.ms_ellapsed()) < OFFSET) {
					molecule.push_back(bondedDroplets[j]);
					bondedDroplets[j].sorted = true;
				}
			}
			molecules.push_back(molecule);
		}
	}
	int mSize = molecules.size();
	for (int x = 0; x < mSize; x++) {

		if (molecules[x].size() >= 2) {
			drawMolecule(molecules[x], CAMERA_FRAME);
		}
	}
}

RotatedRect DropletDetector::calcRotatedRect(Droplet& droplet, Mat hsvBin, Point binCenter) {
	//creates RotatedRect objects that DrawDroplets() uses to create specific ellipses for each Droplet
	Mat hsvClone;
	hsvBin.copyTo(hsvClone);
	vector<vector<Point>> contours;
	findContours(hsvClone, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
	int size = contours.size();
	if (contours.size() > 0) {
		int cSize = contours.size();
		for (int i = 0; i < cSize; i++) {
			//finds whether point binCenter is inside or outside the contour
			if (pointPolygonTest(contours[i], binCenter, false) == 1.0) {
				RotatedRect rect = fitEllipse((Mat)contours[i]);
				rect.center = droplet.location;
				droplet.setRotatedRect(rect);

				return rect;
			}
		}
	}
}

int DropletDetector::average(vector<int> nums) {
	int sum = 0;
	int size = nums.size();
	for (int i = 0; i < size; i++) {
		sum += nums[i];
	}

	double average = (double)sum / nums.size();
	return (int)average;
}

bool DropletDetector::isBiggerScalar(Scalar s1, Scalar s2) {
	if (s1[0] >= s2[0] && s1[1] >= s2[1] && s1[2] >= s2[2]) {
		return true;
	}
	return false;
}

void DropletDetector::filterCandidates2(Mat contourOutlines, Mat grayBinary, vector<Droplet> candidates) {
	Mat hsv;
	Scalar h_min(0, 150, 50), h_max(75, 255, 110), o_min(80, 149, 50), o_max(140, 255, 110), he_min(105, 79, 200), he_max(179, 132, 255), li_min(47, 15, 140), li_max(83, 40, 160),
			n_min(140, 70, 90), n_max(160, 200, 110);


	cvtColor(DARK_FRAME, hsv, CV_RGB2HSV);

	int cSize = candidates.size();
	for (int i = 0; i < cSize; i++) {


		Mat subImage;
		Mat final;

		Point candidateLoc = candidates[i].location;
		//creates a submatrix containing only the Droplet in HSV color space. This makes it easier to calculate the HSV Average of the light the Droplet is emitting
		Rect r(candidateLoc.x - candidates[i].boundingRect.width, candidateLoc.y - candidates[i].boundingRect.height, candidates[i].boundingRect.width * 2, candidates[i].boundingRect.height * 2);
		Rect matRect(0, 0, hsv.cols, hsv.rows);
		bool is_inside = (r & matRect) == r;


		if (is_inside) {
			//Center of Droplet appears white to camera. Thus we use a mask to remove it so it does not affect our average HSV
			subImage = Mat(hsv, r);
			Mat grayMask = Mat(contourOutlines, r);
			bitwise_and(subImage, subImage, final, grayMask);
		}
		else {
			subImage = Mat(hsv, candidates[i].boundingRect);
		}

		//average HSV calculation
		vector<int> h_roi, s_roi, v_roi;
		int fCols = final.cols;
		int fRows = final.rows;
		for (int i = 0; i < fCols; i++) {
			for (int j = 0; j < fRows; j++) {

				if ((int)final.at<Vec3b>(j, i)[2] != 0 && (int)final.at<Vec3b>(j, i)[2] < 100) {
					h_roi.push_back((int)final.at<Vec3b>(j, i)[0]);
					s_roi.push_back((int)final.at<Vec3b>(j, i)[1]);
					v_roi.push_back((int)final.at<Vec3b>(j, i)[2]);
				}
			}
		}

		int hAverage = average(h_roi);
		int sAverage = average(s_roi);
		int vAverage = average(v_roi);

		Scalar hsvAverage(hAverage, sAverage, vAverage);


		//Determine which color the Droplet is by comparing calculated average HSV to known HSV thresholds for different Droplet colors
		if (isBiggerScalar(hsvAverage, h_min) && isBiggerScalar(h_max, hsvAverage)) {
			bool exists = false;
			Droplet hydrogen = candidates[i];
			hydrogen.setName("H");
			hydrogen.setColor(Scalar(0, 200, 255));
			hydrogen.setENeg(2.1);
			hydrogen.setAtomicNumber(1);
			processDroplet(hydrogen, exists);
		}
		else if (isBiggerScalar(hsvAverage, he_min) && isBiggerScalar(h_max, hsvAverage)) {
			bool exists = false;
			Droplet helium = candidates[i];
			helium.setName("He");
			helium.setColor(Scalar(0, 50, 255));
			helium.setENeg(0.0);
			//processDroplet(helium, exists);
		}
		else if (isBiggerScalar(hsvAverage, o_min) && isBiggerScalar(o_max, hsvAverage)) {
			bool exists = false;
			Droplet oxygen = candidates[i];
			oxygen.setName("O");
			oxygen.setColor(Scalar(255, 0, 0));
			oxygen.setENeg(3.5);
			oxygen.setAtomicNumber(8);
			processDroplet(oxygen, exists);

		}
		else if (isBiggerScalar(hsvAverage, li_min) && isBiggerScalar(li_max, hsvAverage)) {
			bool exists = false;
			Droplet lithium = candidates[i];
			lithium.setName("Li");
			lithium.setColor(Scalar(255, 100, 100));
			lithium.setENeg(1.0);
			processDroplet(lithium, exists);
		}
		else if(isBiggerScalar(hsvAverage, n_min) && isBiggerScalar(n_max, hsvAverage)) {
			bool exists = false;
			Droplet nitrogen = candidates[i];
			nitrogen.setName("N");
			nitrogen.setColor(Scalar(10, 10, 200));
			nitrogen.setENeg(3.0);
			processDroplet(nitrogen, exists);
		}

	}
	cleanup(grayBinary);

	vector<Droplet> bonded;
	//code for isFlashing()
	Droplet red("RED");
	inRange(hsv, red.getHSVMin(), red.getHSVMax(), RED_THRESH);
	processBinary(RED_THRESH);

	int dSize = ALL_DROPLETS.size();
	for (int j = 0; j < dSize; j++) {
		//see whether each Droplet is flashing red
		if (isFlashing(ALL_DROPLETS[j])) {
			ALL_DROPLETS[j].isBonded = true;
		}

		if (ALL_DROPLETS[j].isBonded) {
			bonded.push_back(ALL_DROPLETS[j]);
		}

		drawDroplet(ALL_DROPLETS[j]);
	}

	sortBondedDroplets(bonded);

}


double DropletDetector::getOrientation(const vector<Point> &pts, Point& center){
	//Construct a buffer used by the pca analysis
	int sz = static_cast<int>(pts.size());
	Mat data_pts = Mat(sz, 2, CV_64FC1);
	for (int i = 0; i < data_pts.rows; ++i)	{
		data_pts.at<double>(i, 0) = pts[i].x;
		data_pts.at<double>(i, 1) = pts[i].y;
	}
	//Perform PCA analysis
	PCA pca_analysis(data_pts, Mat(), CV_PCA_DATA_AS_ROW);
	//Store the center of the object
	center = Point(static_cast<int>(pca_analysis.mean.at<double>(0, 0)),
		static_cast<int>(pca_analysis.mean.at<double>(0, 1)));
	//Store the eigenvalues and eigenvectors
	vector<Point2d> eigen_vecs(2);
	vector<double> eigen_val(2);
	for (int i = 0; i < 2; ++i)	{
		eigen_vecs[i] = Point2d(pca_analysis.eigenvectors.at<double>(i, 0),
			pca_analysis.eigenvectors.at<double>(i, 1));
		eigen_val[i] = pca_analysis.eigenvalues.at<double>(0, i);
	}
	double angle = atan2(eigen_vecs[0].y, eigen_vecs[0].x); // orientation in radians
	return angle;
}

void DropletDetector::rotateContour(double angle, vector<Point> &pts, const Point center) {
	for (int i = 0; i < pts.size(); i++) {
		rotatePoint(pts[i], center, angle);
	}
}

void DropletDetector::trackDropletsGrayScale(Mat grayBinary) {
	clock_t t1, t2;
	double time;
	t1 = clock();

	morphOps(grayBinary);
	//Bonded Droplets will flash every 5.133 seconds, so we assign a clock to each Droplet once we have determined it is flashing and reset it every 5133 ms
	int dSize = ALL_DROPLETS.size();
	for(int x = 0; x < dSize; x++) {
		int ms = ALL_DROPLETS[x].t.ms_ellapsed();
		if(ALL_DROPLETS[x].t.ms_ellapsed() > 5133 && ALL_DROPLETS[x].isBonded) {
			ALL_DROPLETS[x].t.reset();
		}
	}


	Mat processedBinary;
	grayBinary.copyTo(processedBinary);
	vector<Droplet> dropletCandidates;
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;


	findContours(processedBinary, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);


	Mat contourOutlines = Mat(grayBinary.rows, grayBinary.cols, CV_8UC1);
	drawContours(contourOutlines, contours, -1, Scalar(255), 5);


	if (hierarchy.size() > 0) {
		int numObjects = hierarchy.size();

		if (numObjects < MAX_OBJECTS) {
			for (int i = 0; i >= 0; i = hierarchy[i][0]) {
				//gives us xy position and area
				Moments moment = moments((Mat) contours[i]);
				double area = moment.m00;
				//if contour area is not background noise, it is most likely a Droplet
				if (area > MIN_AREA) {
					Droplet droplet;
					try{
						RotatedRect rect = fitEllipse((Mat)contours[i]);
						droplet.setRotatedRect(rect);
					}
					catch(Exception& e){
					}
					droplet.setXPos(moment.m10 / area);
					droplet.setYPos(moment.m01 / area);
					droplet.location = Point(droplet.getXPos(), droplet.getYPos());

					droplet.boundingRect = boundingRect(contours[i]);
					dropletCandidates.push_back(droplet);
				}
			}
		}
	}

	filterCandidates2(contourOutlines, grayBinary, dropletCandidates);

//	t2 = clock();
//	time = (t2 - t1) / CLOCKS_PER_SEC;
//	__android_log_print(ANDROID_LOG_INFO, "TAG", "Time Elapsed: %d", time);

}

