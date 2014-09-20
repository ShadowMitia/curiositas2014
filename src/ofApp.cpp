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


	camera2.setPosition(ofVec3f(x, y, z));
	camera2.lookAt(ofVec3f(a, b, c), ofVec3f(xx, yy, zz));

	camera2.setFov(fov);

	/*
	cameraLookAtNode.setPosition(0,-1,0);
	camera.lookAt(cameraLookAtNode, ofVec3f(0, -1, 0));
	*/

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
	sceneLight.enable();
	camera2.begin();

	sceneLight.draw();
	

	camera2.setFov(fov);
	camera2.setPosition(x, y, z);

	camera2.lookAt(ofVec3f(a, b, c), ofVec3f(xx, yy, zz));

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
	
	ofSetColor(25, 0, 200);
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

	/*
	ofVec3f v1 = camera2.getGlobalPosition();
	ofVec3f v2 = cameraLookAtNode.getGlobalPosition();
    ofLine(v1,v2);
	*/
	ofSphere(0, 0, 0, 20);
	
	
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

	
}



//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	switch (key) {
		case 'd':
			bDrawDebug = !bDrawDebug;
			break;
		case 'f':
			fov++;
			break;
		case 'g':
			fov--;
			break;

		case 'p':
			y+=25;
			break;

		case 'm':
			y-=25;
			break;

		case OF_KEY_LEFT:
			x--;
			break;
		case OF_KEY_RIGHT:
			x++;
			break;

		case OF_KEY_UP:
			z++;
			break;

		case OF_KEY_DOWN:
			z--;
			break;

		case 'a':
			yy++;
			break;

		case 'z':
			yy--;
			break;

		case 'w':
			a++;
			break;
		case 'x':
			a--;
			break;
		case 'c':
			b++;
			break;
		case 'v':
			b--;
			break;

		case 'b':
			c++;
			break;

		case 'n':
			c--;
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