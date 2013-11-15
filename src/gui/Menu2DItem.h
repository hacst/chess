#include <string>
#include <memory>
#include <iostream>

#include "SOIL.h"

#include "gui/AnimationHelper.h"

#ifdef _WIN32
#include <windows.h> // Needed for gl.h
#endif

#include <GL/gl.h>

using namespace std;

#ifndef MENU2DITEM_H
#define MENU2DITEM_H

class Menu2DItem {
public:
	struct boundingBox {
		int top;
		int right;
		int bottom;
		int left;
	} bb;

	Menu2DItem(int index, string descr, int positionX, int positionY, int width, int height, int animationDuration);
	virtual ~Menu2DItem() { /* Nothing */ }

	Menu2DItem::boundingBox getBoundingBox();
	int getIndex();
	string getDescription();

	void draw();

	void setHoverState(bool flag);
	void setActiveState(bool flag);

private:
	// member variables
	int m_index;
	std::string m_descr;
	int m_positionX;
	int m_positionY;
	int m_width;
    int m_height;

	GLuint m_texture[3];

	bool m_hovered;
	bool m_activated = false;

	AnimationHelperPtr animationHelper;
};

using Menu2DItemPtr = std::shared_ptr<Menu2DItem>;

#endif // MENU2DITEM_H