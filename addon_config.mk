meta:
	ADDON_NAME = ofxSDLController
	ADDON_DESCRIPTION = SDL2 controller input for openFrameworks

osx:
	ADDON_INCLUDES += /usr/local/include /opt/homebrew/include
	ADDON_LDFLAGS += -L/usr/local/lib -L/opt/homebrew/lib -lSDL2

linux64:
	ADDON_PKG_CONFIG_LIBRARIES += sdl2

linux:
	ADDON_PKG_CONFIG_LIBRARIES += sdl2
