#include "ofApp.h"

#ifndef round
#define round(x) (x<0?ceil((x)-0.5):floor((x)+0.5))
#endif

#define SCREEN_WIDTH 1280.f
#define SCREEN_HEIGHT 800.f


//--------------------------------------------------------------
void ofApp::setup() {

	ofLogLevel(OF_LOG_VERBOSE);


	/////// KINECT STUFF
	
	// enable depth->video image calibration
	kinectPlayer1.setRegistration(true);


	// open a kinect by id, starting with 0 (sorted by serial # lexicographically))

	kinectPlayer1.init();
	kinectPlayer1.open(0);




	if (kinectPlayer1.isConnected()) {
		ofLogNotice() << "Kinect Player 1 " << kinectPlayer1.getSerial();
		ofLogNotice() << "sensor-emitter dist: " << kinectPlayer1.getSensorEmitterDistance() << "cm";
		ofLogNotice() << "sensor-camera dist:  " << kinectPlayer1.getSensorCameraDistance() << "cm";
		ofLogNotice() << "zero plane pixel size: " << kinectPlayer1.getZeroPlanePixelSize() << "mm";
		ofLogNotice() << "zero plane dist: " << kinectPlayer1.getZeroPlaneDistance() << "mm";
	}

	/*

	/////// OPENNI2

	openNISystem.setup();

	openNISystem.start();

	///// END OPENNI


	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	ofBackground( 0, 0, 0);

	fov = 10;
	x = 0.f;
	z = 0.f;
	y = -100.f;


	xx = 0;	
	yy = -1;
	zz = 0;

	a = 0;
	b = 0;
	c = 0;


	camera2.setDistance(3.f);
	camera2.setNearClip(0.01f);
	camera2.setFarClip(500.0f);
	camera2.setPosition(ofVec3f(x, y, z));
	//camera2.lookAt(ofVec3f(0.000001, b, c), ofVec3f(0, -1, 0));
	camera2.lookAt(ofVec3f(0.0000001, 0, 0), ofVec3f(0, -1, 0));

	camera2.setFov(fov);


	world.setup();
	world.setCamera(&camera2);
	
	ground = new ofxBulletBox();
	ground->create( world.world, ofVec3f(0., 5.5, 0.), 0., SCREEN_WIDTH, 1.f, SCREEN_HEIGHT);
	ground->setProperties(.25, .95);
	ground->add();
	
	colors[0] = ofColor(15,197,138);
	colors[1] = ofColor(220, 0, 220);
	colors[2] = ofColor(220, 180, 60);
	colors[3] = ofColor(255, 20, 50);
	
	jointLength = .35f;
	
	shapes.push_back( new ofxBulletSphere() );
	((ofxBulletSphere*)shapes[0])->create( world.world, ofVec3f(0, 2, 0), 0.4f, .5f );
	shapes[0]->add();
	
	shapeColors.push_back( colors[(int)round(ofRandom(0, 3))] );
	
	joints.push_back( new ofxBulletJoint() );
	joints[joints.size()-1]->create( world.world, shapes[0], ofVec3f(0, 0, 0));
	joints[joints.size()-1]->add();
	
	bDrawDebug	= false;
	bSpacebar	= false;
	bShapesNeedErase = false;
	
	ofHideCursor();

	sceneLight.setPosition(0.f, -20.f, 0.f);





	*/






	///////// OPENCV



	colorImg.allocate(kinectPlayer1.width, kinectPlayer1.height);
	grayImage.allocate(kinectPlayer1.width, kinectPlayer1.height);
	grayThreshNear.allocate(kinectPlayer1.width, kinectPlayer1.height);
	grayThreshFar.allocate(kinectPlayer1.width, kinectPlayer1.height);
	
	nearThreshold = 230;
	farThreshold = 70;
	bThreshWithOpenCV = true;
	
	ofSetFrameRate(60);
	
	// zero the tilt on startup
	angle = 0;
	kinectPlayer1.setCameraTiltAngle(angle);
	
	// start from the front
	bDrawPointCloud = false;


	    //reserve memory for cv images
    rgb.allocate(kinectPlayer1.width, kinectPlayer1.height);
    hsb.allocate(kinectPlayer1.width, kinectPlayer1.height);
    hue.allocate(kinectPlayer1.width, kinectPlayer1.height);
    sat.allocate(kinectPlayer1.width, kinectPlayer1.height);
    bri.allocate(kinectPlayer1.width, kinectPlayer1.height);
    filtered.allocate(kinectPlayer1.width, kinectPlayer1.height);



}
//--------------------------------------------------------------
void ofApp::update() {
	///// OPENCV
	kinectPlayer1.update();

// there is a new frame and we are connected
	if(kinectPlayer1.isFrameNew()) {
		
		// load grayscale depth image from the kinect source
		grayImage.setFromPixels(kinectPlayer1.getDepthPixels(), kinectPlayer1.width, kinectPlayer1.height);
		
		// we do two thresholds - one for the far plane and one for the near plane
		// we then do a cvAnd to get the pixels which are a union of the two thresholds

		grayThreshNear = grayImage;
		grayThreshFar = grayImage;
		grayThreshNear.threshold(nearThreshold, true);
		grayThreshFar.threshold(farThreshold);
		cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);

		

        //copy webcam pixels to rgb image
		rgb.setFromPixels(kinectPlayer1.getPixels(),kinectPlayer1.getWidth(), kinectPlayer1.getHeight());
        
        //duplicate rgb
        hsb = rgb;
        
        //convert to hsb
        hsb.convertRgbToHsv();
        
        //store the three channels as grayscale images
        hsb.convertToGrayscalePlanarImages(hue, sat, bri);
        
        //filter image based on the hue value were looking for
        for (int i=0; i<kinectPlayer1.width*kinectPlayer1.height; i++) {
            filtered.getPixels()[i] = ofInRange(hue.getPixels()[i],targetHue-5,targetHue+5) ? 255 : 0;
        }

		//cvAnd(filtered.getCvImage(), grayImage.getCvImage(), grayImage.getCvImage());
		
		// update the cv images
		grayImage.flagImageChanged();
		
		// find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
		// also, find holes is set to true so we will get interior contours as well....
		contourFinder.findContours(grayImage, 10, (kinectPlayer1.width*kinectPlayer1.height)/2, 20, false);
	}

	/////// END OPENCV

	/*
	world.update();
	ofSetWindowTitle(ofToString(ofGetFrameRate(), 0));
	
	mousePos = camera.screenToWorld( ofVec3f((float)ofGetMouseX(), (float)ofGetMouseY(), 0) );
	joints[0]->updatePivotPos( mousePos, 2.f );
	
	if(bSpacebar) {
		for (int i = 1; i < joints.size(); i++) {
			delete joints[i];
		}
		joints.erase(joints.begin()+1, joints.end());
		bSpacebar = false;
		bShapesNeedErase = true;
	}
	
	for (int i = 1; i < joints.size(); i++) {
		if(i == 1) {
			joints[i]->updatePivotPos( shapes[i-1]->getPosition(), jointLength*2.f );
		} else {
			joints[i]->updatePivotPos( shapes[i-1]->getPosition(), jointLength );
		}
	}
	*/


}

//--------------------------------------------------------------
void ofApp::draw() {

	//////////// OPENCV

	ofSetColor(255,255,255);
    
	
	if(bDrawPointCloud) {
		easyCam.begin();
		drawPointCloud();
		easyCam.end();
	} else {
		// draw from the live kinect
		kinectPlayer1.drawDepth(10, 10, 400, 300);
		kinectPlayer1.draw(420, 10, 400, 300);
		
		grayImage.draw(10, 320, 400, 300);
		contourFinder.draw(10, 320, 400, 300);
	}
	 

	// draw instructions
	ofSetColor(255, 255, 255);
	stringstream reportStream;
        
    if(kinectPlayer1.hasAccelControl()) {
        reportStream << "accel is: " << ofToString(kinectPlayer1.getMksAccel().x, 2) << " / "
        << ofToString(kinectPlayer1.getMksAccel().y, 2) << " / "
        << ofToString(kinectPlayer1.getMksAccel().z, 2) << endl;
    } else {
        reportStream << "Note: this is a newer Xbox Kinect or Kinect For Windows device," << endl
		<< "motor / led / accel controls are not currently supported" << endl << endl;
    }
    
	reportStream << "press p to switch between images and point cloud, rotate the point cloud with the mouse" << endl
	<< "using opencv threshold = " << bThreshWithOpenCV <<" (press spacebar)" << endl
	<< "set near threshold " << nearThreshold << " (press: + -)" << endl
	<< "set far threshold " << farThreshold << " (press: < >) num blobs found " << contourFinder.nBlobs
	<< ", fps: " << ofGetFrameRate() << endl
	<< "press c to close the connection and o to open it again, connection is: " << kinectPlayer1.isConnected() << endl;

    if(kinectPlayer1.hasCamTiltControl()) {
    	reportStream << "press UP and DOWN to change the tilt angle: " << angle << " degrees" << endl
        << "press 1-5 & 0 to change the led mode" << endl;
    }

	reportStream << "HUE: " << targetHue << endl;
    
	ofDrawBitmapString(reportStream.str(), 20, 652);
    


	//////////// END OPENCV


	/*
	ofDrawAxis(10.f);


	ofEnableDepthTest();
	sceneLight.enable();
	camera2.begin();

	sceneLight.draw();
	

	camera2.setFov(fov);
	camera2.setPosition(x, y, z);

	ofRotateY(90);

	ofSetColor(255, 0, 0);

	//camera2.draw();

	ofDrawAxis(10.f);
	
	ofSetLineWidth(1.f);
	if(bDrawDebug){
		world.drawDebug();
	}
	
	ofSetColor(255, 255, 255);
	ofSphere(mousePos, .15f);
	
	ofEnableLighting();
	light.enable();
	light.setPosition( mousePos );
	
	ofSetColor(0, 255, 0);
	ground->draw();
	
	ofSetColor(255, 0, 255);
	for(int i = 0; i < shapes.size(); i++) {
		ofSetColor(shapeColors[i]);
		shapes[i]->draw();
	}
	ofSetColor(220, 220, 220);
	for(int i = 0; i < joints.size(); i++) {
		joints[i]->draw();
	}
	light.disable();
	ofDisableLighting();

	ofVec3f v1 = camera2.getGlobalPosition();
	ofVec3f v2 = cameraLookAtNode.getGlobalPosition();
    ofLine(v1,v2);

	
	
	camera2.end();
	sceneLight.disable();
	glDisable(GL_DEPTH_TEST);
	
	ofSetColor(255, 255, 255);
	stringstream ss;
	ss << "framerate: " << ofToString(ofGetFrameRate(),0) << endl;
	ss << "num shapes: " << (shapes.size()) << endl;
	ss << "draw debug (d): " << ofToString(bDrawDebug, 0) << endl;
	ss << "break joints with spacebar: " << bSpacebar << endl;
	ss << "FOV: " << fov << endl;
	ss << "x :" << x << " (" << camera2.getX() << ") " << "y: " << y << " (" << camera2.getY() << ") " << "z: " << z<< " (" << camera2.getZ() << ") " << endl;
	ss << "yy : " << yy << endl;
	ss << "a: " << a << " b: " << b << " c: " << c << endl;
	ofDrawBitmapString(ss.str().c_str(), 10, 10);
	*/
	
}

void ofApp::drawPointCloud() {
	int w = 640;
	int h = 480;
	ofMesh mesh;
	mesh.setMode(OF_PRIMITIVE_POINTS);
	int step = 2;
	for(int y = 0; y < h; y += step) {
		for(int x = 0; x < w; x += step) {
			if(kinectPlayer1.getDistanceAt(x, y) > 0) {
				mesh.addColor(kinectPlayer1.getColorAt(x,y));
				mesh.addVertex(kinectPlayer1.getWorldCoordinateAt(x, y));
			}
		}
	}
	glPointSize(3);
	ofPushMatrix();
	// the projected points are 'upside down' and 'backwards' 
	ofScale(1, -1, -1);
	ofTranslate(0, 0, -1000); // center the points a bit
	ofEnableDepthTest();
	mesh.drawVertices();
	ofDisableDepthTest();
	ofPopMatrix();
}




//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	switch (key) {

		case ' ':
			bThreshWithOpenCV = !bThreshWithOpenCV;
			break;
			
		case'p':
			bDrawPointCloud = !bDrawPointCloud;
			break;
			
		case '>':
		case '.':
			farThreshold ++;
			if (farThreshold > 255) farThreshold = 255;
			break;
			
		case '<':
		case ',':
			farThreshold --;
			if (farThreshold < 0) farThreshold = 0;
			break;
			
		case '+':
		case '=':
			nearThreshold ++;
			if (nearThreshold > 255) nearThreshold = 255;
			break;
			
		case '-':
			nearThreshold --;
			if (nearThreshold < 0) nearThreshold = 0;
			break;
			
		case 'w':
			kinectPlayer1.enableDepthNearValueWhite(!kinectPlayer1.isDepthNearValueWhite());
			break;
			
		case 'o':
			kinectPlayer1.setCameraTiltAngle(angle); // go back to prev tilt
			kinectPlayer1.open();
			break;
			
		case 'c':
			kinectPlayer1.setCameraTiltAngle(0); // zero the tilt
			kinectPlayer1.close();
			break;
			
		case '1':
			kinectPlayer1.setLed(ofxKinect::LED_GREEN);
			break;
			
		case '2':
			kinectPlayer1.setLed(ofxKinect::LED_YELLOW);
			break;
			
		case '3':
			kinectPlayer1.setLed(ofxKinect::LED_RED);
			break;
			
		case '4':
			kinectPlayer1.setLed(ofxKinect::LED_BLINK_GREEN);
			break;
			
		case '5':
			kinectPlayer1.setLed(ofxKinect::LED_BLINK_YELLOW_RED);
			break;
			
		case '0':
			kinectPlayer1.setLed(ofxKinect::LED_OFF);
			break;
			
		case OF_KEY_UP:
			angle++;
			if(angle>30) angle=30;
			kinectPlayer1.setCameraTiltAngle(angle);
			break;
			
		case OF_KEY_DOWN:
			angle--;
			if(angle<-30) angle=-30;
			kinectPlayer1.setCameraTiltAngle(angle);
			break;
		
			
		default:
			cout << "keyPressed " << (char)key << " " << key << endl;
			break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
	
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
	
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
	
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	//calculate local mouse x,y in image
	int mx = x % (int)kinectPlayer1.getWidth();
    int my = y % (int)kinectPlayer1.getHeight();

	    //get hue value on mouse position
    targetHue = hue.getPixels()[my*w+mx];
  
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
	
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {
	
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) { 
	
}


//--------------------------------------------------------------
void ofApp::exit() {
	kinectPlayer1.setCameraTiltAngle(0); // zero the tilt on exit
	kinectPlayer1.close();

}