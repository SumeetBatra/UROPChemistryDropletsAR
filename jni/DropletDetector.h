/*
 * DropletDetector.h
 *
 *  Created on: Jul 8, 2016
 *      Author: JP BATRA Work
 */

#ifndef JNI_DROPLETDETECTOR_H_
#define JNI_DROPLETDETECTOR_H_

#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <string>
#include <vector>
#include <cmath>
#include <opencv2/features2d.hpp>
#include "Droplet.h"

using namespace std;
using namespace cv;


class DropletDetector {
public:
	DropletDetector();
	~DropletDetector();

	void setFrame(Mat& cameraFrame, Mat& darkFrame);
	void trackDropletsGrayScale(Mat grayBinary);
	void morphOps(Mat& binFrame);
	void processBinary(Mat& binaryMat);
	void filterCandidates2(Mat contourOutlines, Mat grayBinary, vector<Droplet> candidates);
	void processDroplet(Droplet droplet, bool& exists);
	void cleanup(Mat grayBinary);
	void drawDroplet(Droplet droplet);
	void sortBondedDroplets(vector<Droplet> boundedDroplets);
	void drawMolecule(vector<Droplet> syncedDroplets, Mat& cameraFeed);
	void onTouch(Point p);
	void rotateContour(double angle, vector<Point> &pts, const Point center);
	void rotatePoint(Point& p, const Point center, double angle);

	int distance(Droplet d1, Droplet d2);
	int distance(Point p1, Point p2);
	int average(vector<int> nums);
	int minElectronegativity(vector<Droplet> syncedDroplets);
	double getOrientation(const vector<Point> &pts, Point& center);
	bool isBiggerScalar(Scalar s1, Scalar s2);
	bool isFlashing(Droplet &droplet);
	RotatedRect calcRotatedRect(Droplet& droplet, Mat hsvBin, Point center);

private:
	int MAX_OBJECTS = 20;
	int MIN_AREA = 100;
	int DISTANCE = 80;
	vector<Droplet> ALL_DROPLETS;
	int OFFSET = 333;

};

#endif /* JNI_DROPLETDETECTOR_H_ */
