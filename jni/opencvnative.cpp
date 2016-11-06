#include <jni.h>
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <vector>
#include "DropletDetector.h"


using namespace std;
using namespace cv;

/*This is the intermediary between Java and C++ code. Java matrices and primitives are interpreted as C++ matrices and primitives  */

int GRAY_MIN = 220, GRAY_MAX = 254;
DropletDetector detector;


extern "C" {
JNIEXPORT void JNICALL Java_com_urop_chemistrydroplets_MainActivity_TrackDroplets(JNIEnv*, jobject, jlong addrDarkRgba, jlong addrGrayBin, jlong addrRgba);
JNIEXPORT void JNICALL Java_com_urop_chemistrydroplets_MainActivity_GetTouchedPoint(JNIEnv*, jobject, double x, double y);

JNIEXPORT void JNICALL Java_com_urop_chemistrydroplets_MainActivity_TrackDroplets(JNIEnv*, jobject, jlong addrDarkRgba, jlong addrGrayBin, jlong addrRgba)
{
	Mat *darkFrame = (Mat*)addrDarkRgba;
	Mat *cameraFrame = (Mat*)addrRgba;
	Mat *grayBinary = (Mat*)addrGrayBin;

	detector.setFrame(*cameraFrame, *darkFrame);
	detector.trackDropletsGrayScale(*grayBinary);

//	darkFrame->release();
//	grayBinary->release();

}

JNIEXPORT void JNICALL Java_com_urop_chemistrydroplets_MainActivity_GetTouchedPoint(JNIEnv*, jobject, double x, double y)
{
	Point touchedPos(x, y);
	detector.onTouch(touchedPos);
}
}
