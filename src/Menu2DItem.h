#include <string>
#include "AnimationHelper.h"

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
	int index;
	string descr;
	int positionX;
	int positionY;
	int width;
    int height;

	bool hovered, activated = false;
	AnimationHelper* animationHelper;
};

#endif // MENU2DITEM_H