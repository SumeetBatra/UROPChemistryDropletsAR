#include "Droplet.h"



Droplet::Droplet()
{
	grayMin = 235;
	grayMax = 254;
	xPos = 0;
	yPos = 0;
	sorted = false;
	showInfo = false;
}

Droplet::Droplet(string n) {
	name = n;
	grayMin = 221;
	grayMax = 254;
	xPos = 0;
	yPos = 0;
	sorted = false;
	showInfo = false;

	if (name == "RED") {
		setHSVMin(Scalar(0, 170, 90));
		setHSVMax(Scalar(50, 255, 110));
	}
	if (name == "H") {
		setHSVMin(Scalar(90, 84, 255));
		setHSVMax(Scalar(93, 255, 255));
		setColor(Scalar(0, 255, 0));
	}
	if (name == "O") {
		setHSVMin(Scalar(134, 22, 255));
		setHSVMax(Scalar(150, 103, 255));
		setColor(Scalar(255, 0, 255));
	}
}


Droplet::~Droplet()
{
}

int Droplet::getRadius() {
	if (radius > 0) {
		return radius;
	}
	else {
		return 20;
	}
}

void Droplet::setColor(Scalar c) {
	color = c;
}

void Droplet::setHSVMin(Scalar min) {
	HSVMin = min;
}

void Droplet::setHSVMax(Scalar max) {
	HSVMax = max;
}

void Droplet::setXPos(int x) {
	xPos = x;
}

void Droplet::setYPos(int y) {
	yPos = y;
}

void Droplet::setRadius(int r) {
	radius = r;
}


