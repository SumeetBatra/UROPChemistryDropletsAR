/*
 * Droplet.h
 *
 *  Created on: Jul 8, 2016
 *      Author: JP BATRA Work
 */

#ifndef JNI_DROPLET_H_
#define JNI_DROPLET_H_

#pragma once
#include <string>
#include <opencv2\core.hpp>
#include <time.h>
using namespace std;
using namespace cv;
class Droplet {
public:


	struct timer {
		clock_t start = clock();
		double duration;

		void reset() {
			start = clock();
		}

		double ms_ellapsed() {
			duration = (clock() - start) / (double)CLOCKS_PER_SEC;
			duration = duration * 1000;
			return duration;
		}
	};

	bool sorted;
	bool isBonded;
	bool isCenter;
	bool showInfo;
	Point location;
	Rect boundingRect;
	timer t;

	Droplet();
	Droplet(string name);
	~Droplet();

	int getXPos() {
		return xPos;
	}
	int getYPos() {
		return yPos;
	}
	int getRadius();

	void setXPos(int x);
	void setYPos(int y);
	void setRadius(int r);

	Scalar getHSVMin() {
		return HSVMin;
	}
	Scalar getHSVMax() {
		return HSVMax;
	}

	int getGrayMin() {
		return grayMin;
	}
	int getGrayMax() {
		return grayMax;
	}

	Scalar getColor() {
		return color;
	}
	void setColor(Scalar color);

	void setHSVMin(Scalar min);
	void setHSVMax(Scalar max);

	void setName(string n) {
		name = n;
	}
	string getName() {
		return name;
	}

	double getENeg() {
		return electronegativity;
	}
	void setENeg(double e) {
		electronegativity = e;
	}

	int getAtomicNumber() {
		return atomicNumber;
	}

	void setAtomicNumber(int n) {
		atomicNumber = n;
	}

	RotatedRect getRotatedRect() {
		return rect;
	}
	void setRotatedRect(RotatedRect r) {
		rect = r;
	}

private:
	Scalar HSVMin, HSVMax;
	Scalar color;
	string name;
	int xPos, yPos;
	int radius;
	int grayMin, grayMax;
	double electronegativity;
	int atomicNumber;
	RotatedRect rect;
};

#endif /* JNI_DROPLET_H_ */
