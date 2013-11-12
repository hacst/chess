#include <iostream>
#include <array>
#include <string>
#include <vector>
#include "SOIL.h"

#include "Menu2DItem.h"
#include "AnimationHelper.h"

#ifdef _WIN32
#include <windows.h> // Needed for gl.h
#endif

#include <GL/gl.h>

#ifndef MENU2D_H
#define MENU2D_H

class Menu2D {
public:
	Menu2D(const int windowWidth, const int windowHeight);
	virtual ~Menu2D() { /* Nothing */ }

	void draw();
	void mouseMoved(const int x, const int y);
	void mouseClicked();
	void mouseReleased();

private:
	const int ITEM_STATE_TYPE_PRESSED = 1;
	const int ITEM_STATE_TYPE_RELEASED = 2;
	const int ITEM_STATE_TYPE_MOVED = 3;

	std::vector<Menu2DItem*> items;
	std::vector<string> menuOptions;	// 2 dim. array

	// config
	const int buttonWidth = 200;
	const int buttonHeight = 50;
	const int buttonMargin = 10;
	const int animationDuration = 2000;

	int height;						// menu height in total with margin
	int width;
	int marginLeft;					// distance from left
	int marginTop;					// distance from top

	struct mouseState {
		int x;
		int y;
		bool pressed;
	} mouseState;

	Menu2DItem* activeButton;
	AnimationHelper* animationHelper;

	void changeItemState(const int type);
	int LoadGLTextures();
};

#endif // MENU2D_H