#pragma once

#include "ofMain.h"
#include <SDL2/SDL.h>
#include <string>
#include <vector>

// Call all methods on the main thread. Indices are positions in the current
// connected-device list and may change when a controller is disconnected.
class ofxSDLController {
public:
    ofxSDLController() = default;
    ~ofxSDLController();
    ofxSDLController(const ofxSDLController&) = delete;
    ofxSDLController& operator=(const ofxSDLController&) = delete;

    // Set to false when another SDL user consumes controller events.
    bool setup(bool discardControllerEvents = true);
    void update();
    void exit();
    int size() const;
    std::string name(int index) const;
    bool isMapped(int index) const;
    SDL_JoystickID instanceID(int index) const;

    // Mapped axes: sticks [-1,1], triggers [0,1]. Invalid indices return 0.
    float axis(int index, SDL_GameControllerAxis axis) const;
    bool button(int index, SDL_GameControllerButton button) const;
    int numRawAxes(int index) const;
    int numRawButtons(int index) const;
    int numRawHats(int index) const;
    Uint8 rawHat(int index, int hat) const;
    float rawAxis(int index, int axis) const;
    bool rawButton(int index, int button) const;
    void drawDebug(float x, float y) const;

private:
    struct Device {
        SDL_JoystickID id;
        SDL_Joystick* joystick;
        SDL_GameController* controller;
    };
    const Device* device(int index) const;
    void refresh();
    std::vector<Device> devices;
    bool initialized = false;
    bool discardEvents = true;
};
