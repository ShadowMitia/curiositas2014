#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofxBullet.h"
#include "ofxFluid.h"
#include "ofxOsc.h"

#include <cmath>
#include <vector>

// listen on port 12345
#define PORT 12345
#define SOUND_PORT 3000
#define HOST "localhost"


class testApp : public ofBaseApp{
public:
    void setup();
    void update();
    void draw();
	void createSphere(bool playerOne);
	void keyPressed(int key);
    
	// kinect stuff
	ofVec3f positionKinect1;
	ofVec3f positionKinect2;

	ofVec3f oldPositionKinect1;
	ofVec3f oldPositionKinect2;

	bool kinect1Connected;
	bool kinect2Connected;


	ofPoint top, left, bottom, right;

	float racket1AngleHori;
	float racket1AngleVerti;

	float racket2AngleHori;
	float racket2AngleVerti;
    
    int w,h; // width and height of kinects


    int findHue;
	int findSat;
	int findBri;


	////////////////////////////////////////////

	// Cam
	ofEasyCam    cam[2];

	ofEasyCam	 easyDebugCam;

    int			 numberCamera;

    
    //Bullet physics
    ofVec3f                             gravity;
    ofxBulletWorldRigid                 world;
	ofxBulletBox                        ground;

	ofxBulletBox						areaWalls[2];


	bool								drawDebug;



    ofxBulletSphere*                    sphere; // the ball
	ofxBulletCylinder*                  racketPlayer1; // the racket for player 1
	ofxBulletCylinder*                  racketPlayer2; // the racket for player 2
    
    //Light
    ofLight                             light;
    
    
    //Debug
    
    float                               x,y,z;
    int                                 debugMouv;
    float                               width, height; // width and height of ground (aka game area)
    bool                                actFluid;
	int                                 m,s;
	
    ofSpherePrimitive                   debugSphere;
    
    //fluid
    ofxFluid						fluid;
	ofPoint						 posFluid;
	ofPoint						colFluid;

	// ofEvent stuff

	void onCollision(ofxBulletCollisionData& cdata);

	// OSC stuff

	ofxOscReceiver receiver;

	bool player1TouchedBallLast;

	ofPoint collisionPoint;

	int offsetAngleH;
	int offsetAngleV;

	stringstream speedy;

	bool service;
	bool playerOneServed;
	bool playerTwoServed;



	ofxOscSender sender;


};

#endif