#include "ofApp.h"

using namespace ofxCv;

//--------------------------------------------------------------
void ofApp::setup(){
    ofShowCursor();
    
    ofSetFrameRate(30);
    
    jpeg.setup(20, 40, 80);
    
    shader.load("shaders/shader.vert", "shaders/shader.frag");

    gui.setup();
    gui.add(ratioThreshold.setup( "ratio", 0.75f, 0, 1 ));
    
    setupImages("images", "png");
    
	tracker.setup();
	tracker.setRescale(.5);
    classifier.reset();
    classifierInited = false;
    
    grabber.initGrabber(640, 480);
    
    imageIndex = 0;
    glitchCount = 0;
}

//--------------------------------------------------------------
void ofApp::setupImages(string dirName, string ext) {
    // load images in images folder
    ofDirectory dir(dirName);
    dir.allowExt(ext);
    dir.listDir();
    for(int i = 0; i < dir.size(); i++) {
        images.push_back(ofImage());
        images.back().loadImage(dirName + "/" + dir.getName(i));
    }
}

//--------------------------------------------------------------
void ofApp::update(){
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    
    grabber.update();
    if(grabber.isFrameNew()) {
		tracker.update(toCv(grabber));
    }
    
    if(tracker.getFound() && !classifierInited) {
        classifier.load("expressions");
        classifierInited = true;
    }
    
    updateDecision();
    updateGlitch();
}

//--------------------------------------------------------------
void ofApp::updateDecision(){
    classifier.classify(tracker);
    
    // decide what glitch we want
	int primary = classifier.getPrimaryExpression();
    float pTotal = 0;
    float pPrime = classifier.getProbability(primary);
    for(int i = 0; i < classifier.size(); i++){
        pTotal += classifier.getProbability(i);
    }
    
    // check if the result is not confident enough
    // (Strictly speaking this is too optimistic, and instead entropy should be used.
    //  But since we only have 3 labels, this is not a huge issue.)
    if(pPrime / pTotal < ratioThreshold) {
        // if eyebrows are raised rather than smiling, apply gray
        if(classifier.getProbability(0) > classifier.getProbability(2)) {
            glitchType = GRAY;
        }
        // if smiling
        else {
            glitchType = JPEG;
        }
    }
    // confident enough; no glitch
    else {
        glitchType = NONE;
    }
}

//--------------------------------------------------------------
void ofApp::updateGlitch(){
    // countdown; lasts for 10 frames
    glitchCount = max(0, glitchCount - 1);
    
    if(glitchType == JPEG) {
        if(imageIndex == 0) {
            imageIndex = ofRandom(1, images.size());
            glitchCount = 10;
        }
        if(!jpeg.getImage().isAllocated() || ofGetFrameNum() % 5 == 0) {
            jpeg.setPixels(images.at(imageIndex - 1).getPixelsRef());
            jpeg.glitch();
        }
    }
    else {
        imageIndex = 0;
    }
}

//--------------------------------------------------------------
void ofApp::exit() {
    tracker.waitForThread();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    ofSetColor(255, 255);
    
    shader.begin();
    ofBaseDraws *baseDraw;
    
    switch(glitchType) {
        case GRAY:
            // draw camera feed in gray
            baseDraw = &grabber;
            shader.setUniformTexture("tex0", grabber.getTextureReference(), 0);
            shader.setUniform1f("doGray", 1);
            break;
        case JPEG:
            // draw images with glitch
            baseDraw = &(jpeg.getImage());
            shader.setUniformTexture("tex0", jpeg.getImage().getTextureReference(), 0);
            shader.setUniform1f("doGray", 0);
            break;
        case NONE:
        default:
            // draw camera feed in color
            baseDraw = &grabber;
            shader.setUniformTexture("tex0", grabber.getTextureReference(), 0);
            shader.setUniform1f("doGray", 0);
            break;
    }
    
    shader.setUniform2f("imageViewPort", baseDraw->getWidth(), baseDraw->getHeight());
    shader.setUniform2f("screenViewPort", ofGetWidth(), ofGetHeight());
    
    // draw empty rect (fragment shader will map tex0 i.e. baseDraw)
    ofRect(0, 0, ofGetWidth(), ofGetHeight());
    
    shader.end();

    gui.draw();
    drawClassifier();
}

//--------------------------------------------------------------
void ofApp::drawClassifier() {
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
