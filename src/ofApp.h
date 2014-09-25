#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"

#include <vector>

class testApp : public ofBaseApp{
public:
    void setup();
    void update();
    void draw();

    void mousePressed(int x, int y, int button);

	ofxKinect kinect[2];
    
    ofxCvColorImage rgb, hsb;
    ofxCvGrayscaleImage hue,sat,bri,filtered;
	ofxCvColorImage red, green, blue;
    ofxCvContourFinder contours;
    
    int w,h;
    int findHue;
	int findSat;
	int findBri;

	int findRed, findBlue, findGreen;
};

#endif