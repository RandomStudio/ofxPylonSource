#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){

    ofSetLogLevel(OF_LOG_VERBOSE);

    camWidth = 1024;
    camHeight = 1024;
    numChannels = 3;

    try {
		mPylonSource = PylonSource::create();
		mPylonSource->start(camWidth, camHeight, numChannels);
	}
	catch (GenericException &exc) {
		ofLogFatalError("Failed to init capture");
	}

    camImage.allocate(camWidth, camHeight, OF_IMAGE_COLOR);
}

//--------------------------------------------------------------
void ofApp::update(){

    if (mPylonSource && mPylonSource->isRunning()) {
		void * data = mPylonSource->getFrameData();
        camImage.setFromPixels((const unsigned char*)data, camWidth, camHeight, OF_IMAGE_COLOR);
        camImage.update();
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
    camImage.draw(0, 0, ofGetWindowWidth(), ofGetWindowHeight());
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
