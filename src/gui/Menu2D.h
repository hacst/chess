#include <iostream>
#include <array>
#include <string>
#ifndef MENU2D_H
#define MENU2D_H

#include <vector>
#include <memory>

#include "gui/Menu2DItem.h"
#include "gui/AnimationHelper.h"

#ifdef _WIN32
#include <windows.h> // Needed for gl.h
#endif

#include <GL/gl.h>

class GuiWindow;

class Menu2D {
public:
	Menu2D(int windowWidth, int windowHeight, GuiWindow& guiWindow);
	virtual ~Menu2D() { /* Nothing */ }

	void draw();
	void mouseMoved(const int x, const int y);
	void mouseClicked();
	void mouseReleased();

private:
	const int ITEM_STATE_TYPE_PRESSED = 1;
	const int ITEM_STATE_TYPE_RELEASED = 2;
	const int ITEM_STATE_TYPE_MOVED = 3;

	std::vector<Menu2DItemPtr> items;
	std::vector<string> menuOptions;	// 2 dim. array

	// config
	const int m_buttonWidth = 200;
	const int m_buttonHeight = 50;
	const int m_buttonMargin = 10;
	const int m_animationDuration = 2000;

	GuiWindow &m_window;
	int m_height;						// menu height in total with margin
	int m_width;
	int m_marginLeft;					// distance from left
	int m_marginTop;					// distance from top

	struct mouseState {
		int x;
		int y;
		bool pressed;
	} mouseState;
	
	Menu2DItemPtr activeButton;
	AnimationHelperPtr animationHelper;

	void changeItemState(const int type);
};

using Menu2DPtr = std::shared_ptr<Menu2D>;

#endif // MENU2D_H