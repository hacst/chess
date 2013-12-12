#ifndef GUIWINDOW_H
#define GUIWINDOW_H

#include <SDL.h>
#include <iostream>

#ifdef _WIN32
#include <windows.h> // Needed for gl.h
#endif

#include <GL/gl.h>
#include <gui/FreeType.h>

#include "gui/StateMachine.h"
#include "gui/states/MenuMain.h"
#include "gui/interface/AbstractState.h"

class GuiWindow {
public:
	virtual ~GuiWindow() { /* Nothing */ }

	GuiWindow(std::string title, bool fullscreen, int width, int height);
	void exec();

	int getWidth();
	int getHeight();
	int getCameraDistanceToOrigin();
	bool isFullscreen();

	void set2DMode();
	void set3DMode();

	void swapFrameBufferNow();

	enum WindowMode {
		FULLSCREEN,
		WINDOW
	};

	void switchWindowMode(WindowMode mode);

	float m_cX, m_cY, m_cZ;									// position of the camera
	float m_cameraAngleX, m_cameraAngleY, m_cameraAngleZ;	// angle of the camera in degree
	float m_fov;											// field of view (fov)

	// font rendering
	void printHeadline(std::string text);
	void printSubHeadline(std::string text);
	void printTextCenter(float red, float green, float blue, std::string text);
	void printText(int x, int y, float red, float green, float blue, std::string text);

private:
	struct fontObject {
		int x;
		int y;
		float red;
		float green;
		float blue;
		int fontSize;
		freetype::font_data font;
		std::string text;
	};

	void handleEvents();
	void display();
	void init();
	void terminate();
	void exit();
	
	void makeFrustum(double fovY, double aspectRatio, double front, double back);

	void drawText(fontObject fo);

	// SDL variables
	SDL_Window *window;
	SDL_Event evt;
	SDL_GLContext ogl;

	// member variables
	int m_oldMouseX, m_oldMouseY;

	bool m_quit = false;

	std::string m_title;
	bool m_fullscreen;
	int m_width;
	int m_height;

	int m_colorBits;
	int m_depthBits;
	int m_antiAlias;

	float m_zNear, m_zFar;

	StateMachine& m_fsm;
	
	freetype::font_data fontHeadline, fontSubHeadline, fontText;
	
	enum fontSize {
		HEADLINE = 42,
		SUB_HEADLINE = 28,
		TEXT = 20
	};
};

#endif // GUIWINDOW_H