#include "ofApp.h"

//--------------------------------------------------------------
void testApp::setup() {
	ofLogLevel(OF_LOG_VERBOSE);

    ofBackground(0,0,0);
	
	w = kinectPlayer1.getWidth();
	h = kinectPlayer1.getHeight();

	resize = 4;
    
    //reserve memory for cv images
	hueKinect1.allocate(w, h);
	filteredKinect1.allocate(w, h);

	//reserve memory for cv images
	hueKinect2.allocate(w, h);
	filteredKinect2.allocate(w, h);


	/////////////////////////////////////////
	/////////////////////////////////////////
	/////////////////////////////////////////

	numberCamera=0;
    x=y=z=0;
    width=1280;
    height=800;
    float fov=0.0001;
    m=s=0;

    //Main camera 
    cam[0].setFov(fov);
    fov=((fov/2)*PI)/180;
	cam[0].setFarClip((height/2)/tan(fov)+100);
    cam[0].setNearClip((height/2)/tan(fov)-1000);
	cam[0].setPosition(ofVec3f(0, -(height/2)/tan(fov), 0));
    cam[0].lookAt(ofVec3f(0.000001, 0, 0), ofVec3f(0, -1, 0));
    
    //Camera debug
	/*
	fov=0.0001;
	cam[1].setFov(fov);
	fov=((fov/2)*PI)/180;
	cam[1].setFarClip((height/2)/tan(fov)+10000000);
    cam[1].setNearClip((height/2)/tan(fov)-1000000);
	//cam[1].setPosition(ofVec3f(0, -(height/2)/tan(fov), 0));
	cam[1].setPosition(ofVec3f(0, -1000000, -(height/2)/tan(fov)));
	//cam[1].lookAt(ofVec3f(0.000001, 0, 0), ofVec3f(0, -1, 0));
    cam[1].lookAt(ofVec3f(-0.001, 0, -0), ofVec3f(0, -1, 0));
    */
	cam[1].setNearClip(0.01f);
	cam[1].setFarClip(500000);
	cam[1].setPosition(ofVec3f(18400, 4000, 0));
    cam[1].lookAt(ofVec3f(-1, 1, -1), ofVec3f(0, -1, 0));
    cam[1].setFov(4);

	// set up bullet
    world.setup();
	world.enableGrabbing();
	world.enableDebugDraw();
    world.setCamera(&cam[0]); //world is seen through main camera
    
	world.enableCollisionEvents();
	ofAddListener(world.COLLISION_EVENT, this, &testApp::onCollision);
	// change gravity (mouahahaha)
    gravity=ofVec3f(0,100,0);
	world.setGravity(gravity);

	// create the ground
    ground.create( world.world, ofVec3f(0., 0., 0.), 0., height, -1.f, width );
	ground.setProperties(.25, .95);
	ground.add();

	/*
	areaWalls[0].create( world.world, ofVec3f(0. , 0., 0.), 0., 2000, 5, width / 2);
	areaWalls[0].add();

	areaWalls[1].create( world.world, ofVec3f(0. , 0., 0.), 0.,2000, 5, -width / 2);
	areaWalls[1].add();

	*/

    // sphere
    sphere = new ofxBulletSphere();
	sphere->create(world.world, ofVec3f(0, -1000, 0), 5, 30);
	sphere->setProperties(10, 1.5);
	sphere->add();


	int racketMass = 0;
	int racketRadius = 90;
	int racketHeight = 5;

	// kinematic objects
	// http://forum.openframeworks.cc/t/how-to-rotate-and-translate-a-ofxbullet-body/7954/4

	racketPlayer1 = new ofxBulletCylinder();
	racketPlayer1->create(world.world, ofVec3f(0, -25, 0), racketMass, racketRadius, racketHeight);
	racketPlayer1->setProperties(.25, .95);
	racketPlayer1->add();
	racketPlayer1->enableKinematic();
	racketPlayer1->activate();

	//world.world->addRigidBody(racketPlayer1->getRigidBody());

	btTransform coord;
	racketPlayer1->getRigidBody()->getMotionState()->getWorldTransform(coord);

	btQuaternion rotation = coord.getRotation();

	btVector3 offset = coord.getOrigin();
	rotation.setRotation( rotation.getAxis(), PI / 4);
	coord.setOrigin(btVector3(0., 0., 0.));
	coord.setRotation(rotation);
	coord.setOrigin(offset);
	racketPlayer1->getRigidBody()->getMotionState()->setWorldTransform(coord);

	racketPlayer2 = new ofxBulletCylinder();
	racketPlayer2->create(world.world, ofVec3f(200, -100, 0), racketMass, racketRadius, racketHeight);
	racketPlayer2->add();
    
    // fluid 
    fluid.allocate(width,height);
    fluid.dissipation = 0.9;
    fluid.velocityDissipation = 0.99;
    fluid.setGravity(ofPoint(0,0));

	drawDebug = false;
	showVideoFeed = false;

	oldCentroidX = 0;
	oldCentroidY = 0;
	oldCentroidZ = 0;

	centroidX = 0;
	centroidY = 0;
	centroidZ = 0;

	xMov = 0;
	yMov = 0;
	zMov = 0;



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
        
			hsbKinect1.resize(w, h);
			//copy webcam pixels to rgb image
			hsbKinect1.setFromPixels(kinectPlayer1.getPixels(), w, h);
			hsbKinect1.resize(w, h);

			//convert to hsb
			hsbKinect1.convertRgbToHsv();
			hsbKinect1.convertToGrayscalePlanarImage(hueKinect1, 0);

			for (int i = 0; i < w * h; i++) {
				filteredKinect1.getPixels()[i] = ofInRange(hueKinect1.getPixels()[i],findHue-10,findHue+10) ? 255 : 0;
			}

			//filteredKinect1.setFromPixels(filteredKinect1.getRoiPixels(), roiRect.width, roiRect.height);


			filteredKinect1.flagImageChanged();
			//run the contour finder on the filtered image to find blobs with a certain hue
			contoursKinect1.findContours(filteredKinect1, 100, w*h, 1, false, true);

			if (contoursKinect1.nBlobs > 0) {

				centroidX = contoursKinect1.blobs[0].centroid.x;
				centroidY = contoursKinect1.blobs[0].centroid.y;

				//centroidZ = movie.getDistanceAt(centroidX, centroidY) + (height / 2);

				//centroidZ = movie.getDistanceAt(centroidX, centroidY) + (400 / 2);
				width = contoursKinect1.blobs[0].boundingRect.width / 2;
				height = contoursKinect1.blobs[0].boundingRect.height / 2;

				top = ofVec3f(centroidX, centroidY - height / 2);
				bottom = ofVec3f(centroidX, centroidY +  height / 2 );

				left = ofVec3f(centroidX + width / 2, centroidY);
				right = ofVec3f(centroidX - width / 2, centroidY);
			}
		}

		
		//move kinematic objects http://www.bulletphysics.org/Bullet/phpBB3/viewtopic.php?p=&f=9&t=2355
		//rotate kinematic objects http://bulletphysics.org/Bullet/phpBB3/viewtopic.php?t=6282

		// a kinematic object is a bullet object with no mass
		// you also need to "activate" it with activate()

		if (contoursKinect1.nBlobs > 0) {

			/*
			btTransform coord;
			racketPlayer1->getRigidBody()->getMotionState()->getWorldTransform(coord);

			btQuaternion rotation = coord.getRotation();

			btVector3 offset = coord.getOrigin();
			rotation.setRotation( rotation.getAxis(), rotation.getAngle() + 90);
			coord.setOrigin(btVector3(0., 0., 0.));
			coord.setRotation(rotation);
			coord.setOrigin(offset);
			racketPlayer1->getRigidBody()->getMotionState()->setWorldTransform(coord);
			*/

			btTransform newCoordinate;

			int zModulo = (int)(height / 2);

			racketPlayer1->getRigidBody()->getMotionState()->getWorldTransform(newCoordinate);
			newCoordinate.getOrigin() += btVector3( /*-1 * (centroidX - oldCentroidX) % (int)width*/ 0, /* (centroidY - oldCentroidY) % (int)(height / 3)*/ 0, (centroidZ - oldCentroidZ) % zModulo);
			racketPlayer1->getRigidBody()->getMotionState()->setWorldTransform(newCoordinate);

			racketPlayer1->activate();
		}

	}

	
}

//--------------------------------------------------------------
void testApp::draw(){
	


    ofSetColor(255, 0, 0);
    ofFill();

	/*
    
    //draw red circles for found blobs
	for (int i=0; i<contoursKinect1.nBlobs; i++) {
		ofCircle(contoursKinect1.blobs[i].centroid.x, contoursKinect1.blobs[i].centroid.y, 20);
    } 
	    //draw red circles for found blobs
	for (int i=0; i<contoursKinect2.nBlobs; i++) {
		ofCircle(contoursKinect2.blobs[i].centroid.x, contoursKinect2.blobs[i].centroid.y, 20);
    }   

	*/

    ofSetColor(255,255,255);

	if (drawDebug) {
		world.drawDebug();
	}
	
	//Camera

	ofSetColor(0, 0, 0);
    
    //light.setOrientation(ofVec3f(x,y,z));
    cam[numberCamera].begin();





    
    ofSetColor(0, 100, 0, 255);
	ground.draw();
    
    ofSetColor(225, 0, 225, 255);
	sphere->draw();

	

	ofSetColor(0, 0, 255, 255);
	racketPlayer1->draw();


	//racketPlayer2->draw();

	ofSetColor(255, 255, 255);

    glPointSize(3);
	ofPushMatrix();
	// the projected points are 'upside down' and 'backwards' 
	ofScale(1, -1, -1);
	ofTranslate(0, 0, -1000); // center the points a bit
	ofEnableDepthTest();
	racket1Mesh.drawVertices();
	ofDisableDepthTest();
	ofPopMatrix();


    
    cam[numberCamera].end();
    /*
    if(numberCamera==0){
       fluid.draw(); 
    }
    */

  
	//////////////////////////////////////////////////////



	stringstream ss;
	btVector3 vec = sphere->getRigidBody()->getLinearVelocity();
	

	ss << "ball: velocity (x,y,z): " << vec.getX() << " " << vec.getY() << " " << vec.getZ() << endl;
    ss << endl;
	ss << "w :" << w << "h: " << h << endl;
	ss << "framerate " << ofToString(ofGetFrameRate(), 0) << endl;
	ss << "racket 1: " << racketPlayer1->getPosition() <<endl;
	ss << "sphere: " << sphere->getPosition() << endl;
	ss << endl;

	if (kinectPlayer1.isConnected() && contoursKinect1.nBlobs > 0) {
		ss << "top: " << vertices[0] << endl;
		ss << "bottom: " << vertices[2] << endl;
		ss << "angle " << racket1Angle << endl;
		ss << endl;
		ss << endl << "centroid: " << contoursKinect1.blobs[0].centroid.x << " " << contoursKinect1.blobs[0].centroid.y << " " << centroidZ << " " << endl;
		ss << endl << "old " << oldCentroidX << " " << oldCentroidY << " " << oldCentroidZ << endl;
	}

    ofSetColor(255, 0, 0);
	ofDrawBitmapString(ss.str().c_str(), 10, 10);


	
	cout << "w/" << resize << ": " << w / resize <<  " " << " h/" << resize << ": " << h / resize << endl;


	ofSetColor(255, 0, 0);
			stringstream output;
			if (contoursKinect1.nBlobs){
				output.clear();
				output << "rectangle violet" << endl;
				output << "x: " << contoursKinect1.blobs[0].centroid.x << " y: " << contoursKinect1.blobs[0].centroid.y << endl;
				output << kinectPlayer1.getWorldCoordinateAt(contoursKinect1.blobs[0].centroid.x, contoursKinect1.blobs[0].centroid.y) << endl;
				
				output << "left: " << left << " right: " << right << endl;
				output <<  kinectPlayer1.getWorldCoordinateAt(left.x, left.y) << " =  " << kinectPlayer1.getWorldCoordinateAt(right.x, right.y) << endl;
				ofVec3f tmp = kinectPlayer1.getWorldCoordinateAt(right.x, right.y);
				ofVec3f tmp2 = kinectPlayer1.getWorldCoordinateAt(left.x, left.y);
				output << "delta: " << tmp.z - tmp2.z << endl;
			}
			ofDrawBitmapString(output.str(), w+50, 400);
			if (contoursKinect1.nBlobs > 0) {
				int w = 640;
				int h = 480;
				ofMesh mesh;
				mesh.setMode(OF_PRIMITIVE_POINTS);
				int step = 2;
				for(int y = contoursKinect1.blobs[0].boundingRect.y; y < contoursKinect1.blobs[0].boundingRect.height * resize; y += step) {
					for(int x = contoursKinect1.blobs[0].boundingRect.x; x < contoursKinect1.blobs[0].boundingRect.width * resize; x += step) {
						if(kinectPlayer1.getDistanceAt(x, y) > 0) {
							mesh.addColor(kinectPlayer1.getColorAt(x,y));
							mesh.addVertex(kinectPlayer1.getWorldCoordinateAt(x, y));

						}
					}
				}
				easyDebugCam.begin();
				glPointSize(3);
				ofPushMatrix();
				// the projected points are 'upside down' and 'backwards' 
				ofScale(1, -1, -1);
				ofTranslate(0, 0, -1000); // center the points a bit
				ofEnableDepthTest();
				mesh.drawVertices();
				ofDisableDepthTest();
				ofPopMatrix();
				easyDebugCam.end();
			}


		//draw all cv images

		if (kinectPlayer1.isConnected()){

			if (showVideoFeed) {
				filteredKinect1.draw(0,0);
				contoursKinect1.draw(0,0);
			}
		



		}
	

		if (kinectPlayer2.isConnected() ){
			if (showVideoFeed) {
			filteredKinect2.draw(w, h);
			contoursKinect2.draw(w, 0);
			}
		}
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button) {
    /*
    //calculate local mouse x,y in image
    int mx = x % w;
    int my = y % h;
    
    //get hue value on mouse position
	findHue = hueKinect1.getPixels()[my*w+mx];
	findSat = satKinect1.getPixels()[my*w+mx];
	findBri = briKinect1.getPixels()[my*w+mx];
	*/
	
}

void testApp::keyPressed(int key) {
	switch (key) {
	case '1':
		cout << "Trying to load Kinect 1" << endl;
		kinectPlayer1.setRegistration(true);
		kinectPlayer1.init();
		kinectPlayer1.open();
		roiRect.x = 0;
		roiRect.y = 0;
		roiRect.width = w;
		roiRect.height = h;
		break;

	case '2':
		cout << "Trying to load Kinect 2" << endl;
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

	case 'v':
		showVideoFeed = ! showVideoFeed;
	break;
	
	
	////////////////////////////////////////////////////

	// Camera 0,1
    case 'a':
        numberCamera=0;
        break;
        
    case 'z':
        numberCamera=1;
        break;
            
    case 'e':
            actFluid= ! actFluid;   
		 break;

	case 'd':
		//debug stuff
		drawDebug = ! drawDebug;
		break;

	case OF_KEY_LEFT:
		racketPlayer1goesLeft = true;
		xMov += 10;
		break;

	case OF_KEY_RIGHT:
		racketPlayer1goesLeft = false;
		xMov -= 10;
		break;

	case OF_KEY_UP:
		racketPlayer1goesUp = true;
		yMov += 10;
		break;

	case OF_KEY_DOWN:
		racketPlayer1goesUp = false;
		yMov -= 10;
		break;
	}
}

void testApp::onCollision(ofxBulletCollisionData& cdata){
	if (*sphere == cdata){
		cout << "sphere collision" << endl;
	}

	if (*racketPlayer1 == cdata){
		cout << "racket 1 collision" << endl;
	}

	if (*racketPlayer2 == cdata){
		cout << "racket 2 collision" << endl;
	}
}

