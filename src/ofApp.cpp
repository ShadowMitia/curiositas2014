#include "ofApp.h"

//--------------------------------------------------------------
void testApp::setup() {
	ofLogLevel(OF_LOG_VERBOSE);

    ofBackground(0,0,0);

	/*
	kinectPlayer1.setRegistration(true);
	kinectPlayer1.init();
	kinectPlayer1.open();

	//kinectPlayer2.setRegistration(true);
	kinectPlayer2.init();
	kinectPlayer2.open();

	*/
	
	w = kinectPlayer1.getWidth();
	h = kinectPlayer1.getHeight();

    
    //reserve memory for cv images
	rgbKinect1.allocate(w, h);
	hsbKinect1.allocate(w, h);
	hueKinect1.allocate(w, h);
	satKinect1.allocate(w, h);
	briKinect1.allocate(w, h);
	filteredKinect1.allocate(w, h);

	    //reserve memory for cv images
	rgbKinect2.allocate(w, h);
	hsbKinect2.allocate(w, h);
	hueKinect2.allocate(w, h);
	satKinect2.allocate(w, h);
	briKinect2.allocate(w, h);
	filteredKinect2.allocate(w, h);

}

//--------------------------------------------------------------
void testApp::update(){

	if (kinectPlayer1.isConnected()) {
		
		kinectPlayer1.update();

    
		if (kinectPlayer1.isFrameNew()) {
        
			//copy webcam pixels to rgb image
			rgbKinect1.setFromPixels(kinectPlayer1.getPixels(), w, h);
        
			//mirror horizontal
			rgbKinect1.mirror(false, true);
        
			//duplicate rgb
			hsbKinect1 = rgbKinect1;
        
			//convert to hsb
			hsbKinect1.convertRgbToHsv();
        
			//store the three channels as grayscale images
			hsbKinect1.convertToGrayscalePlanarImages(hueKinect1, satKinect1, briKinect1);


		
		

			//filter image based on the hue value were looking for
			for (int i=0; i<w*h; i++) {
				filteredKinect1.getPixels()[i] = ofInRange(hueKinect1.getPixels()[i],findHue-5,findHue+5) ? 255 : 0;
			}


			filteredKinect1.flagImageChanged();
			//run the contour finder on the filtered image to find blobs with a certain hue
			contoursKinect1.findContours(filteredKinect1, 200, w*h/2, 1, false, true);
		}
	} else {
		ofSetColor(255, 0, 0);
		ofDrawBitmapString("Kinect 1 not found", 50, 50);
	}
    
	if (kinectPlayer2.isConnected() ) {
		kinectPlayer2.update();

    
		if (kinectPlayer2.isFrameNew()) {
        
			//copy webcam pixels to rgb image
			rgbKinect2.setFromPixels(kinectPlayer2.getPixels(), w, h);
        
			//mirror horizontal
			rgbKinect2.mirror(false, true);
        
			//duplicate rgb
			hsbKinect2 = rgbKinect2;
        
			//convert to hsb
			hsbKinect2.convertRgbToHsv();
        
			//store the three channels as grayscale images
			hsbKinect2.convertToGrayscalePlanarImages(hueKinect2, satKinect2, briKinect2);


		
		

			//filter image based on the hue value were looking for
			for (int i=0; i<w*h; i++) {
				filteredKinect2.getPixels()[i] = ofInRange(hueKinect2.getPixels()[i],findHue-5,findHue+5) ? 255 : 0;
			}


			filteredKinect2.flagImageChanged();
			//run the contour finder on the filtered image to find blobs with a certain hue
			contoursKinect2.findContours(filteredKinect2, 200, w*h/2, 1, false, true);
		}
	} else {
		ofSetColor(255, 0, 0);
		ofDrawBitmapString("Kinect 2 not found", 50, 100);
	}
}

//--------------------------------------------------------------
void testApp::draw(){
    ofSetColor(255,255,255);
    
    //draw all cv images

	if (kinectPlayer1.isConnected()){
		rgbKinect1.draw(0,0);
		filteredKinect1.draw(0,h);
		contoursKinect1.draw(0,0);
	}
	

	if (kinectPlayer2.isConnected() ){
		rgbKinect2.draw(w,0);
		filteredKinect2.draw(w, h);
		contoursKinect2.draw(w, 0);
	}


    ofSetColor(255, 0, 0);
    ofFill();
    
    //draw red circles for found blobs
	for (int i=0; i<contoursKinect1.nBlobs; i++) {
		ofCircle(contoursKinect1.blobs[i].centroid.x, contoursKinect1.blobs[i].centroid.y, 20);
    } 
	    //draw red circles for found blobs
	for (int i=0; i<contoursKinect2.nBlobs; i++) {
		ofCircle(contoursKinect2.blobs[i].centroid.x, contoursKinect2.blobs[i].centroid.y, 20);
    }    

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button) {
    
    //calculate local mouse x,y in image
    int mx = x % w;
    int my = y % h;
    
    //get hue value on mouse position
	findHue = hueKinect1.getPixels()[my*w+mx];
	findSat = satKinect1.getPixels()[my*w+mx];
	findBri = briKinect1.getPixels()[my*w+mx];
	
	
}

void testApp::keyPressed(int key) {
	switch (key) {
	case '1':
		cout << "Tring to load Kinect 1" << endl;
		kinectPlayer1.setRegistration(true);
		kinectPlayer1.init();
		kinectPlayer1.open();
		break;

	case '2':
		cout << "Tring to load Kinect 2" << endl;
		kinectPlayer2.setRegistration(true);
		kinectPlayer2.init();
		kinectPlayer2.open();
		break;

	case '4':
		cout << "Closing Kinect 1" << endl;
		kinectPlayer1.setCameraTiltAngle(0);
		kinectPlayer1.close();
		break;
	case '5':
		cout << "Closing Kinect 2" << endl;
		kinectPlayer2.setCameraTiltAngle(0);
		kinectPlayer2.close();
		break;

	}
}