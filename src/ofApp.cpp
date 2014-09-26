#include "ofApp.h"

//--------------------------------------------------------------
void testApp::setup() {
	ofLogLevel(OF_LOG_VERBOSE);

    ofBackground(0,0,0);

	/* DOESN'T LOAD AT START, LOAD IT LATER!!!
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


	/////////////////////////////////////////
	/////////////////////////////////////////
	/////////////////////////////////////////

	NumCamera=0;
    x=y=z=0;
    width=1024;
    height=768;
    float fov=0.0001;
    m=s=0;
    // Light
    /*
    light.setDirectional();
    light.setPosition(0, -300, 0);
    light.enable();
    light.setDiffuseColor(ofFloatColor(255,255,255));
    */
    //Camera principal
    cam[0].setFov(fov);
    fov=((fov/2)*PI)/180;
	cam[0].setFarClip((height/2)/tan(fov)+100);
    cam[0].setNearClip((height/2)/tan(fov)-1000);
	cam[0].setPosition(ofVec3f(0, -(height/2)/tan(fov), 0));
    cam[0].lookAt(ofVec3f(0.000001, 0, 0), ofVec3f(0, -1, 0));
    
    //Camera debug
    cam[1].setNearClip(0.01f);
	cam[1].setFarClip(500000);
	cam[1].setPosition(ofVec3f(18400, 4000, 0));
    cam[1].lookAt(ofVec3f(-1, 1, -1), ofVec3f(0, -1, 0));
    cam[1].setFov(4);
    
    world.setup();
	world.enableGrabbing();
	world.enableDebugDraw();
    world.setCamera(&cam[0]);
    
    gravity=ofVec3f(0,100,0);
    
    ground.create( world.world, ofVec3f(0., 0, 0.), 0., height, -1.f, width );
	ground.setProperties(.25, .95);
	ground.add();
    // sphere
    sphere = new ofxBulletSphere();
	sphere->create(world.world, ofVec3f(100, -1000, 0), 1000, 30);
	sphere->add();
    
    // fluid 
    fluid.allocate(width,height);
    fluid.dissipation = 0.9;
    fluid.velocityDissipation = 0.99;
    fluid.setGravity(ofPoint(0,0));
    
    



}

//--------------------------------------------------------------
void testApp::update(){


	ofSetWindowTitle(ofToString(ofGetFrameRate(), 0));
    world.update();
	
    
    //debugSphere.setPosition(0, 0, height*cos(ofGetElapsedTimef()/5));
    //debugSphere.setPosition(0, 0, 768/2);

    fluid.dissipation = 0.9;
	fluid.addTemporalForce(ofPoint(width/3,height/3), ofPoint(100,0), ofFloatColor(0.5,0.1,0.8),6.f);
   
	fluid.begin();
    ofSetColor(0,0);
    ofSetColor(255);
    ofCircle(width/3,height/2 , 20);
    fluid.end();
    fluid.setUseObstacles(true);
	
	
    
    if( actFluid==false){
        fluid.dissipation = 0.95;
        fluid.addTemporalForce(ofPoint(1024/2,768/2), ofPoint(100*cos(ofGetElapsedTimef()*10),100*sin(ofGetElapsedTimef()*10)), ofFloatColor(0.,0.8,0.5),6.f);
    }

    fluid.update();



	//// KINECT + OPENCV

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


	////// END KINECT + OPENCV
}

//--------------------------------------------------------------
void testApp::draw(){
    ofSetColor(255,255,255);


	  //Camera
    
    world.setGravity(gravity);
    
    //light.setOrientation(ofVec3f(x,y,z));
    cam[NumCamera].begin();
    
    ofSetColor(0, 0, 0, 255);
	ground.draw();
    
    ofSetColor(225, 0, 225);
	sphere->draw();
    /*
    if (NumCamera==1) {
        cam[NumCamera].setNearClip(near);
        cam[NumCamera].setFov(fov);
        cam[NumCamera].setFarClip(far);
        cam[NumCamera].setVFlip(flip);
        cam[NumCamera].setPosition(6400,y,0);
        
    }
    */
    
    
    cam[NumCamera].end();
    
      if(NumCamera==0){
       fluid.draw(); 
    }
    
    stringstream ss;
	ss << endl <<cam[0].getY()<<endl;
    ofSetColor(255, 0, 0);
	ofDrawBitmapString(ss.str().c_str(), 10, 10);
   

	//////////////////////////////////////////////////////
    
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
	

	////////////////////////////////////////////////////

	// Camera 0,1
    case 'a':
        NumCamera=0;
        break;
        
    case 'z':
        NumCamera=1;
        break;
            
    case 'e':
            actFluid=!actFluid;
            
		 break;

	}
}