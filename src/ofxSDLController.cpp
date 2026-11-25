#include "ofxSDLController.h"
#include <algorithm>

namespace {
float normalized(Sint16 value) {
    return value < 0 ? value / 32768.0f : value / 32767.0f;
}
}

ofxSDLController::~ofxSDLController() { exit(); }

bool ofxSDLController::setup(bool discardControllerEvents) {
    if (initialized) return true;
    // GLFW owns the window. Allow SDL to poll controllers without an SDL window.
    SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS, "1");
    if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) != 0) {
        ofLogError("ofxSDLController") << "SDL initialization failed: " << SDL_GetError();
        return false;
    }
    initialized = true;
    discardEvents = discardControllerEvents;
    refresh();
    return true;
}

void ofxSDLController::update() {
    if (!initialized) return;
    SDL_PumpEvents();
    SDL_JoystickUpdate();
    refresh();
    if (discardEvents) {
        SDL_FlushEvents(SDL_JOYAXISMOTION, SDL_JOYBATTERYUPDATED);
        SDL_FlushEvents(SDL_CONTROLLERAXISMOTION, SDL_CONTROLLERSENSORUPDATE);
    }
}

void ofxSDLController::refresh() {
    for (auto it = devices.begin(); it != devices.end();) {
        if (!SDL_JoystickGetAttached(it->joystick)) {
            if (it->controller) SDL_GameControllerClose(it->controller);
            else SDL_JoystickClose(it->joystick);
            it = devices.erase(it);
        } else ++it;
    }
    const int count = SDL_NumJoysticks();
    for (int i = 0; i < count; ++i) {
        SDL_JoystickID id = SDL_JoystickGetDeviceInstanceID(i);
        if (id < 0 || std::any_of(devices.begin(), devices.end(), [id](const Device& d) { return d.id == id; })) continue;
        SDL_GameController* controller = SDL_IsGameController(i) ? SDL_GameControllerOpen(i) : nullptr;
        SDL_Joystick* joystick = controller ? SDL_GameControllerGetJoystick(controller) : SDL_JoystickOpen(i);
        if (!joystick) {
            ofLogWarning("ofxSDLController") << "Could not open controller " << i << ": " << SDL_GetError();
            continue;
        }
        devices.push_back({id, joystick, controller});
        ofLogNotice("ofxSDLController") << "Connected: " << SDL_JoystickName(joystick) << " (instance " << id << ")";
    }
}

void ofxSDLController::exit() {
    if (!initialized) return;
    for (auto& d : devices) {
        if (d.controller) SDL_GameControllerClose(d.controller);
        else SDL_JoystickClose(d.joystick);
    }
    devices.clear();
    SDL_QuitSubSystem(SDL_INIT_GAMECONTROLLER);
    initialized = false;
}

const ofxSDLController::Device* ofxSDLController::device(int i) const {
    return i >= 0 && i < size() ? &devices[i] : nullptr;
}
int ofxSDLController::size() const { return static_cast<int>(devices.size()); }
std::string ofxSDLController::name(int i) const {
    auto d = device(i);
    const char* value = d ? SDL_JoystickName(d->joystick) : nullptr;
    return value ? value : "";
}
bool ofxSDLController::isMapped(int i) const { auto d = device(i); return d && d->controller; }
SDL_JoystickID ofxSDLController::instanceID(int i) const { auto d = device(i); return d ? d->id : -1; }
float ofxSDLController::axis(int i, SDL_GameControllerAxis a) const {
    auto d = device(i);
    return d && d->controller && a >= 0 && a < SDL_CONTROLLER_AXIS_MAX ? normalized(SDL_GameControllerGetAxis(d->controller, a)) : 0.f;
}
bool ofxSDLController::button(int i, SDL_GameControllerButton b) const {
    auto d = device(i);
    return d && d->controller && b >= 0 && b < SDL_CONTROLLER_BUTTON_MAX && SDL_GameControllerGetButton(d->controller, b);
}
int ofxSDLController::numRawAxes(int i) const { auto d = device(i); return d ? SDL_JoystickNumAxes(d->joystick) : 0; }
int ofxSDLController::numRawButtons(int i) const { auto d = device(i); return d ? SDL_JoystickNumButtons(d->joystick) : 0; }
int ofxSDLController::numRawHats(int i) const { auto d = device(i); return d ? SDL_JoystickNumHats(d->joystick) : 0; }
Uint8 ofxSDLController::rawHat(int i, int hat) const {
    auto d = device(i);
    return d && hat >= 0 && hat < numRawHats(i) ? SDL_JoystickGetHat(d->joystick, hat) : SDL_HAT_CENTERED;
}
float ofxSDLController::rawAxis(int i, int a) const {
    auto d = device(i);
    return d && a >= 0 && a < numRawAxes(i) ? normalized(SDL_JoystickGetAxis(d->joystick, a)) : 0.f;
}
bool ofxSDLController::rawButton(int i, int b) const {
    auto d = device(i);
    return d && b >= 0 && b < numRawButtons(i) && SDL_JoystickGetButton(d->joystick, b);
}

void ofxSDLController::drawDebug(float x, float y) const {
    ofPushStyle();
    ofSetColor(255);
    if (devices.empty()) ofDrawBitmapString("No SDL controllers connected", x, y);
    for (int i = 0; i < size(); ++i) {
        ofDrawBitmapString(name(i) + "  index " + ofToString(i) + "  instance " + ofToString(instanceID(i)) +
            (isMapped(i) ? "  mapped" : "  raw only"), x, y);
        y += 20;
        for (int a = 0; a < numRawAxes(i); ++a) {
            ofDrawBitmapString("axis " + ofToString(a) + ": " + ofToString(rawAxis(i, a), 3), x, y);
            y += 16;
        }
        std::string buttons = "buttons: ";
        for (int b = 0; b < numRawButtons(i); ++b) buttons += ofToString(b) + (rawButton(i, b) ? "[X] " : "[ ] ");
        ofDrawBitmapString(buttons, x, y);
        y += 32;
    }
    ofPopStyle();
}
