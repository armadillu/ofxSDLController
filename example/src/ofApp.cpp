#include "ofApp.h"

void ofApp::setup() { controllers.setup(); }
void ofApp::update() { controllers.update(); }
void ofApp::draw() {
    ofBackground(25);
    ofDrawBitmapStringHighlight("SDL controller test: move sticks and press buttons", 20, 25);
    controllers.drawDebug(20, 55);
}
void ofApp::exit() { controllers.exit(); }
