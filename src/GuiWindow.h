#ifndef GUIWINDOW_H
#define GUIWINDOW_H

#include <SDL.h>
#include <iostream>
#include "Menu2D.h"

#ifdef _WIN32
#include <windows.h> // Needed for gl.h
#endif

#include <GL/gl.h>

class GuiWindow {
public:
	virtual ~GuiWindow() { /* Nothing */ }

	GuiWindow(std::string title, bool fullscreen, int width, int height);
	void exec();

private:
	void handleEvents();
	void display();
	void exit();
	
	void resetProjection();
	void resetModelViewMatrix();
	void makeFrustum(double fovY, double aspectRatio, double front, double back);
	
	void drawCube(float size);

	void set2DMode();
	void set3DMode();
	void draw2D();
	void draw3D();

	// SDL variables
	SDL_Window *window;
	SDL_Event evt;
	SDL_GLContext ogl;

	// menu smart pointer handle
	Menu2DPtr menu;

	// member variables
	float m_cX, m_cY, m_cZ;       // position of the camera
	bool m_quit = false;
	std::string m_title;
	bool m_fullscreen;
	int m_width;
	int m_height;
};

#endif // GUIWINDOW_H