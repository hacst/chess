#ifndef MENU2D_H
#define MENU2D_H

#include <iostream>
#include <array>
#include <string>

#include <vector>
#include <memory>
#include "boost/bind.hpp"

#include "gui/Menu2DItem.h"
#include "gui/AnimationHelper.h"

#ifdef _WIN32
#include <windows.h> // Needed for gl.h
#endif

#include <GL/gl.h>

class GuiWindow;

class Menu2D {
public:
	Menu2D(int windowWidth, int windowHeight);
	virtual ~Menu2D();

    Menu2DItemPtr addButton(std::string filename);

	void draw();
	void mouseMoved(const int x, const int y);
	void mousePressed();
	void mouseReleased();
	void windowResized(int newWidth, int newHeight);
	void resetAnimation();

private:

	std::vector<Menu2DItemPtr> items;

	// config
	const int m_buttonWidth = 200;
	const int m_buttonHeight = 50;
	const int m_buttonMargin = 10;
	const int m_animationDuration = 1500;

    int m_windowWidth;
	int m_windowHeight;
	int m_btCount;
	int m_height;						// menu height in total with margin
	int m_marginLeft;					// distance from left
	int m_marginTop;					// distance from top

	struct mouseState {
		int x;
		int y;
		bool pressed;
	} mouseState;
	
	Menu2DItemPtr activeButton;
	AnimationHelperPtr animationHelper;
	
	// methods
	void updateAbsolutePosition();
};

using Menu2DPtr = std::shared_ptr<Menu2D>;

#endif // MENU2D_H
