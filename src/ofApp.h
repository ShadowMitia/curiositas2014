#pragma once

#include "ofMain.h"
#include "ofxBullet.h"

class ofApp : public ofBaseApp{

public:
	void setup();
	void update();
	void draw();

	void keyPressed  (int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);
	
	ofxBulletWorldRigid			world;
	ofxBulletBox*				ground;
	
	vector<ofxBulletBaseShape*> shapes;
	vector<ofxBulletJoint*>		joints;
	vector<ofColor>				shapeColors;
	ofColor						colors[4];
	
	bool						bDrawDebug;
	bool						bSpacebar;
	bool						bShapesNeedErase;
	
	float						jointLength;
	
	ofEasyCam camera;
	ofLight						light;
	ofVec3f						mousePos;

	int tiltCamera;
	int t;

	ofNode cameraLookAtNode;

	ofEasyCam camera2;


	float fov;
	float x, y, z;

	float xx, yy, zz;

	float a, b, c;

	ofLight sceneLight;
};
