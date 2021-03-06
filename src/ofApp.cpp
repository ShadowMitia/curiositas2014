#include "ofApp.h"


//--------------------------------------------------------------
void testApp::setup() {
	ofLogLevel(OF_LOG_VERBOSE);
	cout << "listening for osc messages on port " << PORT << "\n";
	receiver.setup(PORT);
	sender.setup(HOST, SOUND_PORT);


    ofBackground(0,0,0);
    

	ofAddListener(world.COLLISION_EVENT, this, &testApp::onCollision);

	/////////////////////////////////////////
	/////////////////////////////////////////
	/////////////////////////////////////////

	numberCamera=0;
    x=y=z=0;
    width=1280;
    height=800;
    float fov=0.0001;
    m=s=0;

	 h = height / 2;
	 w = width / 2;

    //Main camera 
    cam[0].setFov(fov);
    fov=((fov/2)*PI)/180;
	cam[0].setFarClip((height/2)/tan(fov)+100);
    cam[0].setNearClip((height/2)/tan(fov)-1000);
	cam[0].setPosition(ofVec3f(0, -(height/2)/tan(fov), 0));
    cam[0].lookAt(ofVec3f(0.000001, 0, 0), ofVec3f(0, -1, 0));
    
 
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
	

	// change gravity (mouahahaha)
    gravity=ofVec3f(0,400,0);
	world.setGravity(gravity);

	// create the ground
    ground.create( world.world, ofVec3f(0., 0., 0.), 0., height, 1.f, width );
	ground.setProperties(.25, 0);
	ground.add();

	


    // sphere
	sphere = new ofxBulletSphere();
	sphere->create(world.world, ofVec3f(0, -300, 400), 50, 30);
	sphere->getRigidBody()->setLinearVelocity(btVector3(0, 0, -500));
	sphere->setProperties(5, 0);
	sphere->add();   
		


	posFluid=ofPoint(sphere->getPosition().z,sphere->getPosition().x);


	int racketMass = 35000;
	int racketRadius = 180;
	int racketHeight = 5;

	// kinematic objects
	// http://forum.openframeworks.cc/t/how-to-rotate-and-translate-a-ofxbullet-body/7954/4

	/* RACKET 1 */

	racketPlayer1 = new ofxBulletCylinder();
	racketPlayer1->create(world.world, ofVec3f(0, 200, -4000), racketMass, racketRadius, racketHeight);
	racketPlayer1->setProperties(.25, 0);
	racketPlayer1->add();
	racketPlayer1->enableKinematic();
	racketPlayer1->activate();

	btTransform coord;
	racketPlayer1->getRigidBody()->getMotionState()->getWorldTransform(coord);

	btQuaternion rotation = coord.getRotation();

	btVector3 offset = coord.getOrigin();
	rotation.setRotation( rotation.getAxis(), PI / 2 );
	coord.setOrigin(btVector3(0., 0., 0.));
	coord.setRotation(rotation);
	coord.setOrigin(offset);
	racketPlayer1->getRigidBody()->getMotionState()->setWorldTransform(coord);


	/* RACKET 2 */

	racketPlayer2 = new ofxBulletCylinder();
	racketPlayer2->create(world.world, ofVec3f(0, 200, 4000), racketMass, racketRadius, racketHeight);
	racketPlayer2->setProperties(.25, .95);
	racketPlayer2->add();
	racketPlayer2->enableKinematic();
	racketPlayer2->activate();



	btTransform coord2;
	racketPlayer2->getRigidBody()->getMotionState()->getWorldTransform(coord2);

	btQuaternion rotation2 = coord2.getRotation();

	btVector3 offset2 = coord2.getOrigin();
	rotation2.setRotation( rotation2.getAxis(), PI / 2 );
	coord2.setOrigin(btVector3(0., 0., 0.));
	coord2.setRotation(rotation2);
	coord2.setOrigin(offset2);
	racketPlayer2->getRigidBody()->getMotionState()->setWorldTransform(coord2);


	
	areaWalls[0].create( world.world, ofVec3f(height / 2 , 0., 0.), 0., 15, width * 2, width );
	areaWalls[0].add();
	areaWalls[0].enableKinematic();
	areaWalls[0].activate();

	/*
	 ofVec3f pos = areaWalls[0].getPosition();
	btTransform trans;
	trans.setOrigin( btVector3( btScalar( pos.x), btScalar( pos.y), btScalar( pos.z) ) );
	ofQuaternion rotQuat;
	rotQuat = areaWalls[0].getRotationQuat();
	trans.setRotation( btQuaternion(btVector3(1, 0, 0), PI / 2) );
	areaWalls[0].getRigidBody()->getMotionState()->setWorldTransform( trans );
	areaWalls[0].activate();
	*/


	areaWalls[1].create( world.world, ofVec3f(-height / 2 , 0., 0.), 0., 15 , width * 2, width );
	areaWalls[1].add();
	areaWalls[1].enableKinematic();
	areaWalls[1].activate();

	/*
	pos = areaWalls[1].getPosition();
	trans.setOrigin( btVector3( btScalar( pos.x), btScalar( pos.y), btScalar( pos.z) ) );
	rotQuat = areaWalls[1].getRotationQuat();
	trans.setRotation( btQuaternion(btVector3(1, 0, 0), PI / 2) );
	areaWalls[1].getRigidBody()->getMotionState()->setWorldTransform( trans );
	areaWalls[1].activate();
	*/

    
    // fluid 
    fluid.allocate(width,height);
    fluid.dissipation = 0.99;
    fluid.velocityDissipation = 0.99;
    fluid.setGravity(ofPoint(0,0));


	oldPositionKinect1 = ofVec3f(0, 0, 0);
	oldPositionKinect2 = ofVec3f(0, 0, 0);



	player1TouchedBallLast = false;






    /*
    rose[0]=0;
    rose[1]=5;
    posRose.push_back(rose);

    
    advance(rose, 0);
    */
    
    
	offsetAngleH = 0;
	offsetAngleV = 0;



	service = true;

	playerOneServed = true;
	playerTwoServed = false;


	drawDebug = false;

}



//--------------------------------------------------------------
void testApp::update(){



	/////// destor espace boullet /////
	if (sphere->getPosition().x > 600 || sphere->getPosition().x < -600 || sphere->getPosition().y > 50 || sphere->getPosition().z > 1380 || sphere->getPosition().z < -1380){
		createSphere(!player1TouchedBallLast);
		service = true;

		playerOneServed = ! playerOneServed;
		playerTwoServed = ! playerTwoServed;

	}
    
    
    ////////////////////////////////////

	ofSetWindowTitle(ofToString(ofGetFrameRate(), 0));
    world.update();
	
    
    
    
	btVector3 speed = sphere->getRigidBody()->getLinearVelocity();
	speed.setY(0);
	
	speed.normalize();
	//speed *= 50;

	int maxSpeed = 500;

	btVector3 velocity = sphere->getRigidBody()->getLinearVelocity();

	float norm = sqrt( velocity.getX() * velocity.getX() +  velocity.getZ() * velocity.getZ());

	if (norm !=0)  {
			velocity.setX( ( velocity.getX() * maxSpeed / norm ) );
			velocity.setZ( ( velocity.getZ()  * maxSpeed / norm ));
	}
	speed.setX(speed.getX() * -1);
	speed.setZ(speed.getZ() * -1);
    cout<< "vitesse"<<speed.getZ()<< endl;
	sphere->getRigidBody()->setLinearVelocity(velocity);

                                        //////// Fluid ALEX /////////
    
        /// Fluid Balle ///
    
    fluid.dissipation = 0.99;
	fluid.velocityDissipation = 0.99;

	fluid.addTemporalForce(ofPoint((sphere->getPosition().z+width/2),sphere->getPosition().x+height/2),ofPoint(50*speed.getZ(),50* speed.getX()),ofFloatColor(0.5,0.1,0.8),sphere->getPosition().y*9/-1000+5);


    // obstacle //
    
    fluid.begin();
		ofSetColor(0,0);
		ofSetColor(255);
		ofCircle(sphere->getPosition().z+width/2-speed.getZ()
                 ,sphere->getPosition().x+height/2-speed.getX()
                 ,(sphere->getPosition().y*-45/1000+5)/2 ); // colision Fluid
    fluid.end();
    fluid.setUseObstacles(true);
    
        // Fuild Rebond update //
    for (int i=0; i< posRose.size(); i++) {
        rose.clear();
        auto listpos = posRose.begin();
        advance(listpos, i);
        rose=*listpos;
        
        fluid.dissipation = 0.95;
		//fluid.velocityDissipation=0.99;
        fluid.velocityDissipation = 0.89;
        fluid.addTemporalForce(ofPoint (rose[1],rose[2]), ofPoint(100*cos(ofGetElapsedTimef()*10+rose[6]),100*sin(ofGetElapsedTimef()*10+rose[7])), ofFloatColor(rose[3],rose[4],rose[5]),5.f);
        
    
        
    }
    

        /// Fuild Rebond delect ///
    
    for (int i=0; i< posRose.size(); i++) {
        rose.clear();
        auto range_begin = posRose.begin();
        advance(range_begin,i);
        rose=*range_begin;
        if ((rose[0]+10<ofGetElapsedTimef() || (abs(rose[1]-(sphere->getPosition().z+width/2))< 20 && abs(rose[2]-(sphere->getPosition().x+height/2))< 20)) && rose[0]+1< ofGetElapsedTimef())
        {
        

            posRose.erase(range_begin);
        }
    }

    
    posFluid=ofPoint(sphere->getPosition().z+width/2,sphere->getPosition().x+height/2);
    
    
                                            fluid.update();

    
    
                                    ////// Fin Fuild ALEX   //////
	
    
    
    
    //// KINECT + OPENCV

	//rotate kinematic objects http://bulletphysics.org/Bullet/phpBB3/viewtopic.php?t=6282

	// a kinematic object is a bullet object with no mass
	// you also need to "activate" it with activate()

	//http://forum.openframeworks.cc/t/how-to-rotate-and-translate-a-ofxbullet-body/7954/4
	
	//move kinematic objects http://www.bulletphysics.org/Bullet/phpBB3/viewtopic.php?p=&f=9&t=2355

	/*  KINECT 1 */


	ofVec3f pos = racketPlayer1->getPosition();

	btTransform trans;
	trans.setOrigin( btVector3( btScalar( pos.x), btScalar( pos.y), btScalar( pos.z) ) );

	ofQuaternion rotQuat = racketPlayer1->getRotationQuat();

	/*
	/// BEGIN MAGIC

	/// DO NOT TOUCH !!!!

	trans.setRotation( btQuaternion(btVector3(1, 0, 0), racket1AngleHori * 2 * PI / 180 * -1) * btQuaternion(btVector3(0, 0, 1), racket1AngleVerti  * 2 * PI / 180 ));


	/// END MAGIC
	*/

	trans.setRotation( btQuaternion(btVector3(1, 0, 0), PI / 2) * btQuaternion(btVector3(0, 0, 1), racket1AngleVerti  * 2 * PI / 180 ));

	racketPlayer1->getRigidBody()->getMotionState()->setWorldTransform( trans );

	racketPlayer1->activate();


	btTransform newCoordinate;
	
	racketPlayer1->getRigidBody()->getMotionState()->getWorldTransform(newCoordinate);

	//newCoordinate.getOrigin() = btVector3(-positionKinect1.x*(400/320),positionKinect1.y,-positionKinect1.z);
	newCoordinate.getOrigin() = btVector3(positionKinect1.x*(400/320),sphere->getPosition().y,positionKinect1.z);

	racketPlayer1->getRigidBody()->getMotionState()->setWorldTransform(newCoordinate);
	racketPlayer1->activate();



	

	/*  KINECT 2 */
	
	 ofVec3f pos2;
	 pos2 = racketPlayer2->getPosition();


	 btTransform trans2;
	trans2.setOrigin( btVector3( btScalar( pos2.x), btScalar( pos2.y), btScalar( pos2.z) ) );

	ofQuaternion rotQuat2;
	rotQuat2 = racketPlayer2->getRotationQuat();


	/*
	/// BEGIN MAGIC

	/// DO NOT TOUCH !!!!

	trans2.setRotation( btQuaternion(btVector3(1, 0, 0), racket2AngleHori * 2 * PI / 180 * -1) * btQuaternion(btVector3(0, 0, 1), racket2AngleVerti  * 2 * PI / 180 ));

	/// END MAGIC
	*/

	trans2.setRotation( btQuaternion(btVector3(1, 0, 0), PI / 2) * btQuaternion(btVector3(0, 0, 1), racket2AngleVerti  * 2 * PI / 180 ));

	racketPlayer2->getRigidBody()->getMotionState()->setWorldTransform( trans2 );

	racketPlayer2->activate();



	
	racketPlayer2->getRigidBody()->getMotionState()->getWorldTransform(newCoordinate);

	// newCoordinate.getOrigin() = btVector3(-positionKinect2.x*(400/320),positionKinect2.y,-positionKinect2.z);
	newCoordinate.getOrigin() = btVector3(-positionKinect2.x*(400/320),sphere->getPosition().y,-positionKinect2.z);

	racketPlayer2->getRigidBody()->getMotionState()->setWorldTransform(newCoordinate);
	racketPlayer2->activate();


	while (receiver.hasWaitingMessages()) {
		ofxOscMessage message;

		receiver.getNextMessage(&message);


		 if (message.getAddress() == "/kinect1/position") {
			positionKinect1.x = message.getArgAsFloat(0);
			positionKinect1.y = message.getArgAsFloat(1);
			positionKinect1.z = message.getArgAsFloat(2);
			racket1AngleHori = message.getArgAsFloat(3); // correction angle
			racket1AngleVerti = message.getArgAsFloat(4);

			/*cout << "angle hori " << racket1AngleHori << endl;
			cout << "angle verti " << racket1AngleVerti << endl;*/
		 }

		 if (message.getAddress() == "/kinect2/position") {
			positionKinect2.x = message.getArgAsFloat(0);
			positionKinect2.y = message.getArgAsFloat(1);
			positionKinect2.z = message.getArgAsFloat(2);
			racket2AngleHori = message.getArgAsFloat(3);
			racket2AngleVerti = message.getArgAsFloat(4);
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
    if (numberCamera == 1){

	cam[numberCamera].begin();

    ofSetColor(0, 100, 0, 255);
	ground.draw();
    
    ofSetColor(225, 0, 225, 255);
	sphere->draw();

	/*
	areaWalls[0].draw();
	areaWalls[1].draw();
	*/
			
	ofSetColor(0, 0, 255, 255);
	racketPlayer1->draw();
	ofSetColor(255,0,0);
	racketPlayer2->draw();

	areaWalls[0].draw();
	areaWalls[1].draw();

    
	cam[numberCamera].end();}
    
    if(numberCamera==0){
      fluid.draw();
	  ofSetColor(255,0,0);
	 // ofCircle (sphere->getPosition().z+width/2,sphere->getPosition().x+height/2,sphere->getPosition().y*-45/1000+5); //ivi
    }
    
	//////////////////////////////////////////////////////


	/*
	stringstream ss;
	btVector3 vec = sphere->getRigidBody()->getLinearVelocity();
	

	//ss << "ball: velocity (x,y,z): " << vec.getX() << " " << vec.getY() << " " << vec.getZ() << endl;
    //ss << endl;
	
	ss << "framerate " << ofToString(ofGetFrameRate(), 0) << endl;
	ss << "racket 1: " << racketPlayer1->getPosition() <<endl;
	ss << " angle hori " << racket1AngleHori << " offset " << offsetAngleH << endl;
	ss << " angle verti " << racket1AngleVerti << endl;

	ss << endl << "centroid: " << positionKinect1 << endl;
	ss << "delta 1"<< racketPlayer1->getPosition()-positionKinect1 << endl;
	ss << endl;
	ss << "racket 2: " << racketPlayer2->getPosition() <<endl;
	ss << "sphere: " << sphere->getPosition() << endl;
	//ss << "areaWall 0 " << areaWalls[0].getPosition() << endl;
	//ss << "areaWall 1 " << areaWalls[1].getPosition() << endl;
	//ss << endl;
	ss << endl << "centroid 2: " << positionKinect2 << endl;
	ss << "delta 2"<< racketPlayer2->getPosition()-positionKinect2 << endl;

	ss << "sphere velocity " << sphere->getRigidBody()->getLinearVelocity().getX() << " " 
		<< sphere->getRigidBody()->getLinearVelocity().getY() 
		<< sphere->getRigidBody()->getLinearVelocity().getZ() 
		<< endl;

	ss << endl;


	

	//ss << endl << endl << "speed (x, y, z) " << speed.getX() << " " << speed.getY() << " " << speed.getZ() << endl;


	
	
	//ss << endl << "old " << oldPositionKinect1 << endl;
	
	//ss << endl << "angle hori" << racket1AngleHori << endl;
	//ss << endl << "angle verti" << racket1AngleVerti << endl;

	
	//ss << endl << "old 2 " << oldPositionKinect2 << endl;
	//ss << endl << "delta 2 " << racketPlayer2->getPosition().z - positionKinect2.z << endl;
	//ss << endl << "angle hori 2 " << racket2AngleHori << endl;
	//ss << endl << "angle verti 2 " << racket2AngleVerti << endl;
	

    ofSetColor(255, 0, 0);
	ofDrawBitmapString(ss.str().c_str(), 10, 10);
	ofDrawBitmapString(speedy.str(), 100, 10);
	*/
}

void testApp::keyPressed(int key) {
	switch (key) {

	// Camera 0
    case OF_KEY_F1:
        numberCamera=0;
        break;
    
	// Camera 1
	case OF_KEY_F2:
        numberCamera=1;
        break;

	case 'f':
		offsetAngleH += 2;
		break;

	case 'g':
		offsetAngleH -= 2;
		break;
    


	case OF_KEY_RETURN:
		createSphere(!player1TouchedBallLast);
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

	if (*racketPlayer1 == cdata) {
		cout << "racket 1 collision" << endl;
		player1TouchedBallLast = true;

	}

	if (*racketPlayer2 == cdata){
		cout << "racket 2 collision" << endl;
		player1TouchedBallLast = false;
	}

	if (ground == cdata) {
		cout << "ground collision" << endl;
	}

	if (ground == cdata && *sphere == cdata) {
		cout << " ground + sphere " << endl;
        
        /// Creat Rose Fluid ///
        rose.clear();
        
        rose.push_back(ofGetElapsedTimef());
        rose.push_back(sphere->getPosition().z+width/2);
        rose.push_back(sphere->getPosition().x+height/2);
        
        rose.push_back((float) rand() / (float)RAND_MAX);
        rose.push_back((float) rand() / (float)RAND_MAX);
        rose.push_back((float) rand() / (float)RAND_MAX);
        
        rose.push_back(2*PI*(float) rand() / (float)RAND_MAX) ;
        rose.push_back(2*PI*(float) rand() / (float)RAND_MAX) ;
        
        
        
        posRose.push_back(rose);
       
        
    }

	if ( (*racketPlayer1 == cdata && *sphere == cdata) || (*racketPlayer2 == cdata && *sphere == cdata) ) {
		cout << "racket + sphere " << endl;
		service = false;
	}

	if (areaWalls[0] == cdata || areaWalls[1] == cdata ) {
		cout << "wall + sphere collision " << endl;
		
		btVector3 velocity = sphere->getRigidBody()->getLinearVelocity();
		velocity.setZ( velocity.getZ() * 3);
		/*	float norm = sqrt( velocity.getX() * velocity.getX() +  velocity.getZ() * velocity.getZ());

		if (norm !=0)  {
				velocity.setX( ( velocity.getX() * (500 / norm ) ) );
				velocity.setZ( ( velocity.getZ()  * (500 / norm) ));
		}*/
		sphere->getRigidBody()->setLinearVelocity(velocity);

		
	}

	if (areaWalls[0] == cdata) {
		cout << "wall 0 collision " << endl;
	}

	if (areaWalls[1] == cdata) {
		cout << "wall 1 collision " << endl;
	}
}

void testApp::createSphere(bool playerOne){
	
	   sphere->remove();
	   delete sphere;
		sphere = new ofxBulletSphere();
		
		if (playerOneServed && !playerTwoServed) {
			sphere->create(world.world, ofVec3f(0, -300, 400 ), 50, 30);
			sphere->getRigidBody()->setLinearVelocity(btVector3(0, 0, -500 ));
		}
		if (!playerOneServed && playerTwoServed) {
			sphere->create(world.world, ofVec3f(0, -300, -400 ), 50, 30);
			sphere->getRigidBody()->setLinearVelocity(btVector3(0, 0, 500));
		}

		sphere->setProperties(5, 0);
		sphere->add();   
		
		

}
