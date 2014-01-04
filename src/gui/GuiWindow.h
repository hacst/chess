#ifndef GUIWINDOW_H
#define GUIWINDOW_H

#include <SDL.h>
#include <iostream>

#include "gui/StateMachine.h"
#include "gui/states/MenuMain.h"
#include "gui/interface/AbstractState.h"

#ifdef _WIN32
#include <windows.h> // Needed for gl.h
#endif

#include <GL/gl.h>
#include <gui/FreeType.h>

/**
 * This class is a wrapper which holds the window with the OpenGL context.
 * The GuiWindow will handle keyboard and mouse events and provides methods
 * to switch OpenGL matrix modes and camera position.
 * The window can also toggle between fullscreen and window mode.
 */
class GuiWindow {
public:
	virtual ~GuiWindow();

	/**
	 * @brief Creates a new GUI window.
	 * @param title The title/name of the window which is shown in the top window location.
	 * @param fullscreen True to start in fullscreen, false to start in window mode.
	 * @param width The width of the window.
	 * @param height The height of the window.
	 */
	GuiWindow(std::string title, bool fullscreen, int width, int height);

	/**
	 * @brief Inits the window and starts the execution loop.
	 */
	void exec();

	/**
	 * @brief Gets the width of the window.
	 * @return The width of the window.
	 */
	int getWidth();

	/**
	 * @brief Gets the height of the window.
	 * @return The height of the window.
	 */
	int getHeight();

	/**
	 * @brief Gets the distance between the camera and the world coordinate origin.
	 * @return The distance between camera and world origin.
	 */
	int getCameraDistanceToOrigin();

	/**
	 * @brief Checks if the window is currently in fullscreen mode.
	 * @return True if the window is in fullscreen mode, false if not.
	 */
	bool isFullscreen();

	/**
	 * @brief Set the model view matrix to draw 2D.
	 */
	void set2DMode();

	/**
	 * @brief Set the model view matrix to draw 3D.
	 */
	void set3DMode();

	/**
	 * @brief The frame buffer will normally swapped at the end of the execution loop.
	 * If you want to swap it earlier, use this method to force a frame buffer swap
	 * immediately.
	 */
	void swapFrameBufferNow();

	//! Available window modes.
	enum WindowMode {
		FULLSCREEN,
		WINDOW
	};

	/**
	 * @brief Switches the window mode to one of the available modes.
	 * @param mode A window mode, see above.
	 */
	void switchWindowMode(WindowMode mode);

	//! Camera position in world coordinates
	float m_cX, m_cY, m_cZ;

	//! Camera angle in degree
	float m_cameraAngleX, m_cameraAngleY, m_cameraAngleZ;

	//! field of view (is the extent of the observable world that is seen at any given moment)
	float m_fov;

	/**
	 * @brief Prints the headline text at the top left location.
	 * @param text The text to draw.
	 */
	void printHeadline(std::string text);

	/**
	 * @brief Prints the subheadline text at the top left location directly under the headline.
	 * @param text The text to draw.
	 */
	void printSubHeadline(std::string text);

	/**
	 * @brief Prints text at the center of the window's viewport.
	 * @param red The red amount of color between 0.0 and 1.0
	 * @param green The green amount of color between 0.0 and 1.0
	 * @param blue The blue amount of color between 0.0 and 1.0
	 * @param text The text to draw.
	 */
	void printTextCenter(float red, float green, float blue, std::string text);

	/**
	 * @brief Prints text at the given position of the window's viewport with normal text size.
	 * @param x The x location in the viewport.
	 * @param y The y location in the viewport.
	 * @param red The red amount of color between 0.0 and 1.0
	 * @param green The green amount of color between 0.0 and 1.0
	 * @param blue The blue amount of color between 0.0 and 1.0
	 * @param text The text to draw.
	 *
	 * @note The origin of the viewport is the upper left corner.
	 */
	void printText(int x, int y, float red, float green, float blue, std::string text);

	/**
	 * @brief Prints text at the given position of the window's viewport with small text size.
	 * @param x The x location in the viewport.
	 * @param y The y location in the viewport.
	 * @param red The red amount of color between 0.0 and 1.0
	 * @param green The green amount of color between 0.0 and 1.0
	 * @param blue The blue amount of color between 0.0 and 1.0
	 * @param text The text to draw.
	 *
	 * @note The origin of the viewport is the upper left corner.
	 */
	void printTextSmall(int x, int y, float red, float green, float blue, std::string text);

	//! The available font sizes
	enum fontSize {
		HEADLINE = 42,
		SUB_HEADLINE = 28,
		TEXT = 20,
		TEXT_SMALL = 15
	};

private:
	//! Describes a whole font object through color, size, position, font type and text
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

	/**
	 * @brief Handles mouse and keyboard events by using the SDL library.
	 */
	void handleEvents();

	/**
	 * @brief Inits the window by using the SDL, creates the OpenGL context and
	 * call loadFonts() to load all the needed fonts.
	 */
	void init();

	/**
	 * @brief Loads all the fonts.
	 * @note This method is called by init().
	 */
	void loadFonts();

	/**
	 * @brief Destroys the OpenGL context and closes the window.
	 */
	void terminate();

	/**
	 * @brief Sets a flag to close the window. This is a soft quit and will
	 * let the execution loop execute the last round softly.
	 */
	void exit();
	
	/**
	 * @brief Calculates the frustum by providing the field of view (FOV), an 
	 * aspect ratio and a front and back clipping plane.
	 * @param fovY The field of view in degree.
	 * @param aspectRatio The ratio between width and height (width/height)
	 * @param front The position of the front clipping plane.
	 * @param back The position of the back clipping plane.
	 */
	void makeFrustum(double fovY, double aspectRatio, double front, double back);

	/**
	 * @brief Draws the given font object.
	 * @param fo The configured font object.
	 */
	void drawText(fontObject fo);

	//! The pointer to the SDL window
	SDL_Window *window;

	//! SDL events
	SDL_Event evt;

	//! SDL OpenGL context
	SDL_GLContext ogl;

	//! The old mouse coordinates
	int m_oldMouseX, m_oldMouseY;

	//! Flag that indicates whether the execution loop should end.
	bool m_quit = false;

	//! The window title
	std::string m_title;

	//! Flag, if the window is in fullscreen mode
	bool m_fullscreen;

	//! The width of the window
	int m_width;

	//! The height of the window
	int m_height;

	//! Old window dimensions (needed when switching window modes).
	int m_widthOld, m_heightOld;

	//! Bitdepth
	int m_depthBits;

	//! Anti-Alis factor
	int m_antiAlias;

	//! Flag that indicates of the window mode changed, so that all textures must be initialized again.
	bool m_reloadState;

	//! Near and far clipping plane
	float m_zNear, m_zFar;

	//! The state machine
	StateMachine& m_fsm;
	
	//! The different fonts with different sizes
	freetype::font_data fontHeadline, fontSubHeadline, fontText, fontTextSmall;
};

#endif // GUIWINDOW_H