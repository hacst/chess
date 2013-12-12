#ifndef MENU2DITEM_H
#define MENU2DITEM_H

#include <string>
#include <memory>
#include <iostream>
#include <boost/signals2.hpp>

#ifdef _WIN32
#include <windows.h> // Needed for gl.h
#endif

#include <GL/gl.h>

#include "SOIL.h"
#include "gui/Menu2DItem.h"
#include "gui/AnimationHelper.h"

using namespace std;

class Menu2DItem {
public:
	struct boundingBox {
		int top;
		int right;
		int bottom;
		int left;
	} bb;

	Menu2DItem(int index, std::string filename, int width, int height, int animationDuration);
	virtual ~Menu2DItem();

	void setPosition(int x, int y);
	void draw();

	void mouseMoved(int x, int y);
	void mousePressed(int x, int y);
	void mouseReleased(int x, int y);

	void onClick(const boost::function<void()>& slot);
	void unClick();

private:
	// member variables
	int m_index;
	std::string m_filename;
	int m_positionX;
	int m_positionY;
	int m_width;
    int m_height;

	int m_mousePosX = 0;
	int m_mousePosY = 0;

	GLuint m_texture[3];

	bool m_hovered;
	bool m_activated = false;

	bool inBoundingBox();

	AnimationHelperPtr animationHelper;

	using Signal = boost::signals2::signal<void()>;
	Signal m_clicked;
};

using Menu2DItemPtr = std::shared_ptr<Menu2DItem>;

#endif // MENU2DITEM_H