#include "ofApp.h"

#ifndef round
#define round(x) (x<0?ceil((x)-0.5):floor((x)+0.5))
#endif

#define SCREEN_WIDTH 1280.f
#define SCREEN_HEIGHT 800.f


//--------------------------------------------------------------
void ofApp::setup() {
	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	ofBackground( 0, 0, 0);
	
	camera.setPosition(ofVec3f(0, -4.f, -10.f));
	camera.lookAt(ofVec3f(0, 0, 0), ofVec3f(0, -1, 0));

	camera2.setPosition(ofVec3f(0.f, -24.f, 0.f));
	camera2.lookAt(ofVec3f(0, 0, 0), ofVec3f(0, -1, 0));

	cameraLookAtNode.setPosition(0,-1,0);
	camera.lookAt(cameraLookAtNode, ofVec3f(0, -1, 0));
	
	world.setup();
	world.setCamera(&camera);
	
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
}
//--------------------------------------------------------------
void ofApp::update() {
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

}

//--------------------------------------------------------------
void ofApp::draw() {
	ofDrawAxis(10.f);


	glEnable( GL_DEPTH_TEST );
	camera.begin();

	camera.draw();
	camera2.draw();

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
	
	ofSetColor(100, 100, 100);
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
	
	camera.end();
	glDisable(GL_DEPTH_TEST);
	
	ofSetColor(255, 255, 255);
	stringstream ss;
	ss << "framerate: " << ofToString(ofGetFrameRate(),0) << endl;
	ss << "num shapes: " << (shapes.size()) << endl;
	ss << "draw debug (d): " << ofToString(bDrawDebug, 0) << endl;
	ss << "break joints with spacebar: " << bSpacebar << endl;
	ofDrawBitmapString(ss.str().c_str(), 10, 10);

	
}



//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	switch (key) {
		case 'd':
			bDrawDebug = !bDrawDebug;
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