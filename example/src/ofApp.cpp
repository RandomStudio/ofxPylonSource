#include "ofApp.h"


//--------------------------------------------------------------
void ofApp::setup(){

    camWidth = 1920;
    camHeight = 1200;
    numChannels = 1;


    try {
		mPylonSource = PylonSource::create();
		mPylonSource->start(camWidth, camHeight, numChannels, ofFilePath::getAbsolutePath("acA1920-40um.pfs").c_str());
	}
	catch (GenericException &exc) {
		ofLogFatalError("Failed to init capture");
	}

    camImage.allocate(camWidth, camHeight, OF_IMAGE_GRAYSCALE);
}

//--------------------------------------------------------------
void ofApp::update(){

    if (mPylonSource && mPylonSource->isRunning()) {
		void * data = mPylonSource->getFrameData();
        camImage.setFromPixels((const unsigned char*)data, camWidth, camHeight, OF_IMAGE_GRAYSCALE);
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
