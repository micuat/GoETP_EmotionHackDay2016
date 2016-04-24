#include "ofApp.h"

using namespace ofxCv;

// muse-io --device Muse-7042 --osc 'osc.tcp://localhost:12000' --osc-bp-urls 'osc.udp://localhost:14000'

//--------------------------------------------------------------
void ofApp::setup(){
    //ofHideCursor();
    ofShowCursor();
    
    ofSetFrameRate(30);
    
	gui.setup();
	gui.add(ratioThreshold.setup( "ratio", 0.75f, 0, 1 ));
    
    ofDirectory dir("images");
    dir.allowExt("png");
    dir.listDir();
    for(int i = 0; i < dir.size(); i++) {
        ofImage img;
        img.loadImage("images/" + dir.getName(i));
        images.push_back(img);
    }
    
    receiver.setup(14000);
    
	tracker.setup();
	tracker.setRescale(.5);
    classifier.reset();
    classifierInited = false;
    
    jpeg.setup(20, 40, 80);
    grabber.initGrabber(640, 480);
    //grabber.initGrabber(320, 240);
//	while(!grabber.isFrameNew()) {
//		grabber.update();
//		ofSleepMillis(100);
//	}
//	while(!grabber.isFrameNew()) {
//		grabber.update();
//		ofSleepMillis(100);
//	}
    
    shader.load("shaders/shader.vert", "shaders/shader.frag");
    
    blinkCount = 0;
    clenchCount = 0;
    focusIndex = 0;
    imageIndex = 0;
    glitchCount = 0;
}

//--------------------------------------------------------------
void ofApp::update(){
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    blinkCount = max(0, blinkCount - 1);
    clenchCount = max(0, clenchCount - 1);
    glitchCount = max(0, glitchCount - 1);
    
	while(receiver.hasWaitingMessages()){
		ofxOscMessage m;
		receiver.getNextMessage(&m);
        
		if(m.getAddress() == "/muse/elements/blink"){
            if(m.getArgAsInt32(0) == 1) {
                blinkCount = 5;
                
            }
        }
        
		if(m.getAddress() == "/muse/elements/jaw_clench"){
            if(m.getArgAsInt32(0) == 1) {
                clenchCount = 5;
            }
        }
		if(m.getAddress() == "/bci_art/svm/prediction"){
            if(m.getArgAsInt32(0) == 0) { // focused
                focusIndex = ofClamp(focusIndex + 0.05f, 0, 1);
            }
            else {
                focusIndex = ofClamp(focusIndex - 0.05f, 0, 1);
            }
        }
    }
    
    grabber.update();
    if(grabber.isFrameNew()) {
		tracker.update(toCv(grabber));
//		position = tracker.getPosition();
//		scale = tracker.getScale();
//		orientation = tracker.getOrientation();
//		rotationMatrix = tracker.getRotationMatrix();
    }
    
    classifier.classify(tracker);
    
	int n = classifier.size();
	int primary = classifier.getPrimaryExpression();
    float total = 0;
    float p = classifier.getProbability(primary);
    for(int i = 0; i < n; i++){
        total += classifier.getProbability(i);
    }

    if(p / total < ratioThreshold) {
        if(imageIndex == 0) {
            imageIndex = ofRandom(1, images.size());
            glitchCount = 10;
        }
        //clenchCount = 5;
    }
    else {
        imageIndex = 0;
    }
    
    if(grabber.isFrameNew() && imageIndex > 0) {
        if(!jpeg.getImage().isAllocated() || ofGetFrameNum() % 5 == 0) {
            //jpeg.setPixels(grabber.getPixelsRef());
            jpeg.setPixels(images.at(imageIndex - 1).getPixelsRef());
            //ofSeedRandom(ofGetMouseX());
            jpeg.glitch();
        }
    }
    
    
    if(tracker.getFound() && !classifierInited) {
        classifier.load("expressions");
        classifierInited = true;
    }
}

void ofApp::exit() {
    tracker.waitForThread();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    ofBaseDraws *baseDraw;
    ofSetColor(255, 255);
    
    shader.begin();
    shader.setUniform1f("timeValX", ofGetElapsedTimef() * 0.1 );
    shader.setUniform1f("timeValY", -ofGetElapsedTimef() * 0.18 );
    shader.setUniform2f("mouse", mouseX - ofGetWidth()/2, ofGetHeight()/2-mouseY );

    shader.setUniform1f("clenchCount", clenchCount );
    
    if(classifier.getProbability(0) > classifier.getProbability(2) && imageIndex > 0) {
        
        baseDraw = &grabber;
        shader.setUniformTexture("tex0", grabber.getTextureReference(), 0);
        shader.setUniform1f("clenchCount", 5 );
        
    }
    else if(jpeg.getImage().isAllocated() && (imageIndex > 0 || glitchCount > 0) && !ofGetKeyPressed(' ')) {
        baseDraw = &(jpeg.getImage());
        shader.setUniformTexture("tex0", jpeg.getImage().getTextureReference(), 0);
    } else {
        baseDraw = &grabber;
        shader.setUniformTexture("tex0", grabber.getTextureReference(), 0);
    }
    
    shader.setUniform1f("focusIndex", focusIndex);
    
    shader.setUniform2f("imageViewPort", baseDraw->getWidth(), baseDraw->getHeight());
    shader.setUniform2f("screenViewPort", ofGetWidth(), ofGetHeight());
    if(tracker.getFound()) {
        shader.setUniform2fv("facePosition", tracker.getPosition().getPtr());
    }
    else {
        shader.setUniform2fv("facePosition", ofVec2f(-1000, -1000).getPtr());
    }
    shader.setUniform2f("screenViewPort", ofGetWidth(), ofGetHeight());
    //baseDraw->draw(0, 0, ofGetWidth(), ofGetHeight());
    ofRect(0, 0, ofGetWidth(), ofGetHeight());
    
    shader.end();
    
    if(0 && tracker.getFound()) {
        ofPushMatrix();
        ofTranslate(ofGetWidth() * 0.5f, 0);
        ofScale(-1, 1);
        ofTranslate(-ofGetWidth() * 0.5f, 0);
        ofScale(ofGetWidth() / baseDraw->getWidth(), ofGetHeight() / baseDraw->getHeight());
		ofSetLineWidth(1);
		tracker.draw();
        ofPopMatrix();
	}
	int w = 100, h = 12;
	ofPushStyle();
	ofPushMatrix();
	ofTranslate(5, 100);
	int n = classifier.size();
	int primary = classifier.getPrimaryExpression();
    for(int i = 0; i < n; i++){
		ofSetColor(i == primary ? ofColor::red : ofColor::black);
		ofRect(0, 0, w * classifier.getProbability(i) + .5, h);
		ofSetColor(255);
		ofDrawBitmapString(classifier.getDescription(i), 5, 9);
		ofTranslate(0, h + 5);
    }
	ofPopMatrix();
	ofPopStyle();
    
    gui.draw();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == 'f') {
        ofToggleFullscreen();
    }
    // debug
    else if(key == '1') {
        blinkCount = 5;
    }
    else if(key == '2') {
        clenchCount = 5;
    }
    else if(key == 'l') {
        classifier.load("expressions");
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

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
