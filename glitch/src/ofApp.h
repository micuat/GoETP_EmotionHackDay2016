#pragma once

#include "ofMain.h"
#include "ofxJpegGlitch.h"
#include "ofxCv.h"
#include "ofxFaceTrackerThreaded.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp {
public:
    void setup();
    void setupImages(string, string);
    void update();
    void updateDecision();
    void updateGlitch();
    void draw();
    void drawClassifier();
    void exit();

    void keyPressed(int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
    void dragEvent(ofDragInfo dragInfo);
    void gotMessage(ofMessage msg);

private:
    ofVideoGrabber grabber;
	ofxFaceTrackerThreaded tracker;
	ExpressionClassifier classifier;
    bool classifierInited;
    
    vector<ofImage> images;
    int imageIndex;
    int glitchCount;
    enum GlitchType {NONE, JPEG, GRAY} glitchType;
    ofxJpegGlitch jpeg;
    ofShader shader;
    
    ofxPanel gui;
    ofxFloatSlider ratioThreshold;
};
