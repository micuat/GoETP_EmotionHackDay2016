#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxJpegGlitch.h"
#include "ofxCv.h"
#include "ofxFaceTrackerThreaded.h"
#include "ofxGui.h"

class ofApp : public ofBaseApp {
public:
    void setup();
    void update();
    void draw();
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
    int blinkCount;
    int clenchCount;
    ofxOscReceiver receiver;
    ofxJpegGlitch jpeg;
    ofVideoGrabber grabber;
    ofShader shader;
	ofxFaceTrackerThreaded tracker;
    float focusIndex;
    
	ExpressionClassifier classifier;
    bool classifierInited;
    
    vector<ofImage> images;
    int imageIndex;
    int glitchCount;
    
    ofxPanel gui;
    ofxFloatSlider ratioThreshold;
};
