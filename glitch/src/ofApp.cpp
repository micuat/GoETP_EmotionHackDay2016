#include "ofApp.h"

using namespace ofxCv;

//--------------------------------------------------------------
void ofApp::setup(){
    ofShowCursor();
    
    ofSetFrameRate(30);
    
	gui.setup();
	gui.add(ratioThreshold.setup( "ratio", 0.75f, 0, 1 ));
    
    // load images in images folder
    ofDirectory dir("images");
    dir.allowExt("png");
    dir.listDir();
    for(int i = 0; i < dir.size(); i++) {
        ofImage img;
        img.loadImage("images/" + dir.getName(i));
        images.push_back(img);
    }
    
	tracker.setup();
	tracker.setRescale(.5);
    classifier.reset();
    classifierInited = false;
    
    jpeg.setup(20, 40, 80);
    grabber.initGrabber(640, 480);
    
    shader.load("shaders/shader.vert", "shaders/shader.frag");
    
    imageIndex = 0;
    glitchCount = 0;
}

//--------------------------------------------------------------
void ofApp::update(){
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    glitchCount = max(0, glitchCount - 1);
    
    grabber.update();
    if(grabber.isFrameNew()) {
		tracker.update(toCv(grabber));
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
    }
    else {
        imageIndex = 0;
    }
    
    if(grabber.isFrameNew() && imageIndex > 0) {
        if(!jpeg.getImage().isAllocated() || ofGetFrameNum() % 5 == 0) {
            jpeg.setPixels(images.at(imageIndex - 1).getPixelsRef());
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
    
    shader.setUniform1f("doGray", 0);
    if(classifier.getProbability(0) > classifier.getProbability(2) && imageIndex > 0) {
        baseDraw = &grabber;
        shader.setUniformTexture("tex0", grabber.getTextureReference(), 0);
        shader.setUniform1f("doGray", 1);
    }
    else if(jpeg.getImage().isAllocated() && (imageIndex > 0 || glitchCount > 0)) {
        baseDraw = &(jpeg.getImage());
        shader.setUniformTexture("tex0", jpeg.getImage().getTextureReference(), 0);
    } else {
        baseDraw = &grabber;
        shader.setUniformTexture("tex0", grabber.getTextureReference(), 0);
    }
    
    shader.setUniform2f("imageViewPort", baseDraw->getWidth(), baseDraw->getHeight());
    shader.setUniform2f("screenViewPort", ofGetWidth(), ofGetHeight());
    ofRect(0, 0, ofGetWidth(), ofGetHeight());
    
    shader.end();
    
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
