#pragma once
#include "ofMain.h"
#include "ofxSDLController.h"

class ofApp : public ofBaseApp {
public:
    void setup() override;
    void update() override;
    void draw() override;
    void exit() override;
    ofxSDLController controllers;
};
