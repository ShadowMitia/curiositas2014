#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"

#include <vector>

#define USE_TWO_KINECTS

class testApp : public ofBaseApp{
public:
    void setup();
    void update();
    void draw();

    void mousePressed(int x, int y, int button);
	void keyPressed(int key);
    
    ofxCvColorImage rgbKinect1, hsbKinect1;
    ofxCvGrayscaleImage hueKinect1,satKinect1,briKinect1,filteredKinect1;

    ofxCvContourFinder contoursKinect1;

	ofxKinect kinectPlayer1;
	ofxKinect kinectPlayer2;
    
    ofxCvColorImage rgbKinect2, hsbKinect2;
    ofxCvGrayscaleImage hueKinect2,satKinect2,briKinect2,filteredKinect2;

    ofxCvContourFinder contoursKinect2;

	void kinectProcess(ofxKinect &kinect);

    
    int w,h;
    int findHue;
	int findSat;
	int findBri;

};

#endif