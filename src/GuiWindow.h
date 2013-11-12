#include <SDL.h>
#include <iostream>
#include "Menu2D.h"

#ifdef _WIN32
#include <windows.h> // Needed for gl.h
#endif

#include <GL/gl.h>

#ifndef GUIWINDOW_H
#define GUIWINDOW_H

class GuiWindow {
public:
	virtual ~GuiWindow() { /* Nothing */ }

	GuiWindow(const char* title, const bool fullscreen, const int width, const int height);
	void exec();

private:
	void handleEvents();
	void display();
	
	void resetProjection();
	void resetModelViewMatrix();
	void makeFrustum(double fovY, double aspectRatio, double front, double back);
	
	void drawCube(float size);

	void set2DMode();
	void set3DMode();
	void draw2D();
	void draw3D();

	SDL_Window *window;
	SDL_Event evt;
	SDL_GLContext ogl;
	
	float cX = -10.0, cY = 10.0, cZ = 20.0;       // initial position of the camera
	bool quit = false;

	Menu2D* menu;

	struct config {
		char* title;
		bool fullscreen;
		int width;
		int height;
	} conf;
};

#endif // GUIWINDOW_H