#include "ofApp.h"

//--------------------------------------------------------------
void testApp::setup() {
    ofBackground(0,0,0);

	

	for (int i = 0; i < 1; i++) {
		kinect[i].setRegistration(true);
		kinect[i].init();
		kinect[i].open();
	}

	w = kinect[0].getWidth();
	h = kinect[0].getHeight();
    
    //reserve memory for cv images
    rgb.allocate(w, h);
    hsb.allocate(w, h);
    hue.allocate(w, h);
    sat.allocate(w, h);
    bri.allocate(w, h);
    filtered.allocate(w, h);

	red.allocate(w, h);
	green.allocate(w, h);
	blue.allocate(w, h);

}

//--------------------------------------------------------------
void testApp::update(){
    
    kinect[0].update();
    
	if (kinect[0].isFrameNew()) {
        
        //copy webcam pixels to rgb image
        rgb.setFromPixels(kinect[0].getPixels(), w, h);
        
		red.set(255, 0, 0);
		red &= rgb;

		green.set(0,255,0);
		green &= rgb;

		blue.set(0,0,255);
		blue &= rgb;

        //mirror horizontal
        rgb.mirror(false, true);
        
        //duplicate rgb
        hsb = rgb;
        
        //convert to hsb
        hsb.convertRgbToHsv();
        
        //store the three channels as grayscale images
        hsb.convertToGrayscalePlanarImages(hue, sat, bri);


		
		

        //filter image based on the hue value were looking for
        for (int i=0; i<w*h; i++) {
			filtered.getPixels()[i] = ofInRange(hue.getPixels()[i],findHue-5,findHue+5) && ofInRange(sat.getPixels()[i], findSat-2, findSat+2)  ? 255 : 0;
        }


        filtered.flagImageChanged();
        //run the contour finder on the filtered image to find blobs with a certain hue
        contours.findContours(filtered, 200, w*h/2, 1, false, true);
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    ofSetColor(255,255,255);
    
    //draw all cv images
	
    rgb.draw(0,0);
    /*
	hsb.draw(640,0);
    hue.draw(0,240);
    sat.draw(320,240);
    bri.draw(640,240);
	*/
    filtered.draw(0,h);
    contours.draw(0,0);

	/*
	red.draw(0,0);
	green.draw(w, 0);
	blue.draw(0, h);
	*/
    
    ofSetColor(255, 0, 0);
    ofFill();
    
    //draw red circles for found blobs
    for (int i=0; i<contours.nBlobs; i++) {
        ofCircle(contours.blobs[i].centroid.x, contours.blobs[i].centroid.y, 20);
    }    
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button) {
    
    //calculate local mouse x,y in image
    int mx = x % w;
    int my = y % h;
    
    //get hue value on mouse position
    findHue = hue.getPixels()[my*w+mx];

	findSat = sat.getPixels()[my*w+mx];

	findBri = bri.getPixels()[my*w+mx];

	findRed = red.getPixels()[my*w+mx];
	findBlue = blue.getPixels()[my*w+mx];
	findGreen = green.getPixels()[my*w+mx];
	
}