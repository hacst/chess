#include "Menu2DItem.h"

#include <iostream>

using namespace std;

Menu2DItem::Menu2DItem(int index, string descr, int positionX, int positionY, int width, int height, int animationDuration) {
	this->index = index;
	this->positionX = positionX;
	this->positionY = positionY;
	this->width = width;
	this->height = height;
	this->descr = descr;

	// create an animation object
	animationHelper = new AnimationHelper(animationDuration);
}

Menu2DItem::boundingBox Menu2DItem::getBoundingBox() {
	boundingBox bb = {
		positionY,
		positionX + width,
		positionY + height,
		positionX
	};

	return bb;
}

string Menu2DItem::getDescription() {
	return descr;
}

int Menu2DItem::getIndex() {
	return index;
}

void Menu2DItem::setHoverState(bool flag) {
	hovered = flag;
}

void Menu2DItem::setActiveState(bool flag) {
	activated = flag;
}

void Menu2DItem::draw() {
	animationHelper->setStartNowOrKeepIt();

    // we could abstact the openGL binding with an abstract graphic class which implements OpenGL, DirectX, ...
	glBegin(GL_QUADS);
	    if (hovered) {
			glColor4f(0, 0, 1, animationHelper->easeLinear(0, 1));
		}
		else if (activated) {
			glColor4f(1, 0, 0, animationHelper->easeLinear(0, 1));
		}
	    else {
			glColor4f(0, 1, 0, animationHelper->easeLinear(0, 1));
	    }

	    glVertex2f(positionX, positionY);
		glVertex2f(positionX + width, positionY);
		glVertex2f(positionX + width, positionY + height);
		glVertex2f(positionX, positionY + height);
	glEnd();
}