#pragma once
#include "c:\users\apoil7\documents\of_v0.8.3_vs_release\addons\ofxkinect\src\ofxkinect.h"


class Kinect :
	public ofxKinect
{
public:
	Kinect(void);
	~Kinect(void);
	void setup();

private:
	ofxKinect kinect;
	int width, height;

};

