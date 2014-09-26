#ifndef _TEST_APP
#define _TEST_APP

#include "ofMain.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofxBullet.h"
#include "ofxAssimpModelLoader.h"
#include "ofxFluid.h"


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

	ofMesh racketMeshKinect1;
	ofMesh racketMeshKinect2;
    
    ofxCvColorImage rgbKinect2, hsbKinect2;
    ofxCvGrayscaleImage hueKinect2,satKinect2,briKinect2,filteredKinect2;

    ofxCvContourFinder contoursKinect2;

	void kinectProcess(ofxKinect &kinect);

	bool showVideoFeed;



    
    int w,h;
    int findHue;
	int findSat;
	int findBri;

	////////////////////////////////////////////

	// Cam
	ofCamera    cam[2];

    int         numberCamera;

    
    //Physique
    ofVec3f                             gravity;
    ofxBulletWorldRigid                 world;
	ofxBulletBox                        ground;

	ofxBulletBox						areaWalls[2];


	bool								drawDebug;



    ofxBulletSphere*                    sphere; // the ball
    
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
    ofxFluid                   fluid;

    

};

#endif