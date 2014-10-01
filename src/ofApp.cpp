#include "ofApp.h"

//--------------------------------------------------------------
void testApp::setup() {
	ofLogLevel(OF_LOG_VERBOSE);
	cout << "listening for osc messages on port " << PORT << "\n";
	receiver.setup(PORT);

    ofBackground(0,0,0);
    



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
	sphere->create(world.world, ofVec3f(0, -1000, 0), 5000, 30);
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

	UP = 0.0;
	FORWARD = 0.0;
	LEFT = 0.0;

	oldPositionKinect1 = ofVec3f(0, 0, 0);
	oldPositionKinect2 = ofVec3f(0, 0, 0);
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

		
	//move kinematic objects http://www.bulletphysics.org/Bullet/phpBB3/viewtopic.php?p=&f=9&t=2355
	//rotate kinematic objects http://bulletphysics.org/Bullet/phpBB3/viewtopic.php?t=6282

	// a kinematic object is a bullet object with no mass
	// you also need to "activate" it with activate()



	//btTransform coord;
	//racketPlayer1->getRigidBody()->getMotionState()->getWorldTransform(coord);

	//btQuaternion rotation = coord.getRotation();

	//btVector3 offset = coord.getOrigin();
	//rotation.setRotation( rotation.getAxis(), rotation.getAngle() + 90);
	//coord.setOrigin(btVector3(0., 0., 0.));
	//coord.setRotation(rotation);
	//coord.setOrigin(offset);
	//racketPlayer1->getRigidBody()->getMotionState()->setWorldTransform(coord);

	btTransform newCoordinate;

	int zModulo = (int)(height / 2);

	racketPlayer1->getRigidBody()->getMotionState()->getWorldTransform(newCoordinate);
	newCoordinate.getOrigin() += btVector3(  oldPositionKinect1.x - positionKinect1.x ,  oldPositionKinect2.y - positionKinect2.y , 0);
	racketPlayer1->getRigidBody()->getMotionState()->setWorldTransform(newCoordinate);

	racketPlayer1->activate();


	while (receiver.hasWaitingMessages()) {
		ofxOscMessage message;

		receiver.getNextMessage(&message);

		positionKinect1 = ofVec3f(0, 0, 0);
		positionKinect2 = ofVec3f(0, 0, 0);


		 if (message.getAddress() == "/kinect1/position") {
			positionKinect1.x = message.getArgAsFloat(0);
			positionKinect1.y = message.getArgAsFloat(1);
			positionKinect1.z = message.getArgAsFloat(2);
			cout <<"kinect 1 pos: " << positionKinect1 << endl;
		}

		 if (message.getAddress() == "/kinect2/position") {
			positionKinect2.x = message.getArgAsFloat(0);
			positionKinect2.y = message.getArgAsFloat(1);
			positionKinect2.z = message.getArgAsFloat(2);
			cout <<"kinect 2 pos: " << positionKinect2 << endl;
		}

		oldPositionKinect1.set(positionKinect1);
		oldPositionKinect2.set(positionKinect2);

		 
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
    
    cam[numberCamera].end();
    
    if(numberCamera==0){
       fluid.draw(); 
    }
    
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

		ss << endl << "centroid: " << positionKinect1 << endl;
		ss << endl << "old " << oldPositionKinect1 << endl;

    ofSetColor(255, 0, 0);
	ofDrawBitmapString(ss.str().c_str(), 10, 10);

}

void testApp::keyPressed(int key) {
	switch (key) {

	case OF_KEY_LEFT:
		FORWARD -= 10;
		break;

	case OF_KEY_RIGHT:
		FORWARD += 10;
		break;

	case OF_KEY_UP:
		UP -= 10;
		break;

	case OF_KEY_DOWN:
		UP += 10;
		break;

	// Camera 0
    case OF_KEY_F1:
        numberCamera=0;
        break;
    
	// Camera 1
	case OF_KEY_F2:
        numberCamera=1;
        break;
    

	// toggle fluid running
    case 'e':
            actFluid= ! actFluid;   
		 break;

	case 'd':
		//debug stuff
		drawDebug = ! drawDebug;
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

	if (ground == cdata) {
		cout << "ground collision" << endl;
	}
}

