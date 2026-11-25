# ofxSDLController

Controller input for openFrameworks using SDL2, independently of the GLFW window. Provides mapped gamepad controls and raw joystick values, and handles hot-plugging. All calls must be made on the main thread.

## Setup

Install SDL2 2.0.14 or newer (`brew install sdl2` on macOS; `libsdl2-dev` on Debian/Ubuntu). Add this addon to your project with the openFrameworks project generator, or add its source and SDL2 include/library paths to your build. On macOS the addon configuration searches the standard Intel and Apple Silicon Homebrew prefixes; use an SDL2 build matching your app's architecture. The dynamic SDL2 library must be available when the app runs.

The `example` folder contains a minimal GLFW-window test. In this workspace, run `make` from that folder, then launch the resulting app in `bin`. Alternatively, create an openFrameworks project, add this addon, and use the example source files. Connect the controller, launch the app, and move sticks and press buttons to inspect the raw values. It can also be connected after launch.

## Usage

```cpp
#include "ofxSDLController.h"
ofxSDLController controllers;

// setup, update and exit run on the main thread:
controllers.setup();                  // returns false if SDL initialization fails
controllers.update();                 // every frame
controllers.drawDebug(20, 40);        // in draw()
float x = controllers.axis(0, SDL_CONTROLLER_AXIS_LEFTX);
bool a = controllers.button(0, SDL_CONTROLLER_BUTTON_A);
controllers.exit();                   // before application shutdown
```

Device indices are positions in the current list and can change on disconnect. Use `instanceID()` to identify a device while connected. Mapped axes are normalized to [-1, 1] for sticks and [0, 1] for triggers; raw axes use [-1, 1]. A device without an SDL mapping is still accessible through `rawAxis` and `rawButton`. Invalid indices return zero/false. SDL's button names follow its standardized layout, which may differ from labels printed on a Switch controller.

By default, `update()` discards SDL joystick and controller events after updating the polled state, preventing an unconsumed queue from filling. If another part of the application handles those SDL events, call `setup(false)` instead and have that code drain the SDL event queue. Other SDL event types are not discarded. Call `exit()` on the main thread before destroying the application; the destructor is a fallback.
