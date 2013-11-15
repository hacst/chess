#include "Menu2DItem.h"

#include <iostream>

using namespace std;

Menu2DItem::Menu2DItem(int index, string descr, int positionX, int positionY, int width, int height, int animationDuration) {
	m_index = index;
	m_positionX = positionX;
	m_positionY = positionY;
	m_width = width;
	m_height = height;
	m_descr = descr;

	// create an animation object
	animationHelper = make_shared<AnimationHelper>(animationDuration);

	// load normal state texture
	m_texture[0] = SOIL_load_OGL_texture(
		("resources/bt" + to_string(m_index) + "n.png").c_str(),
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y
	);

	// load hover state texture
	m_texture[1] = SOIL_load_OGL_texture(
		("resources/bt" + to_string(m_index) + "h.png").c_str(),
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y
		);

	// load active state texture
	m_texture[2] = SOIL_load_OGL_texture(
		("resources/bt" + to_string(m_index) + "a.png").c_str(),
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y
		);


	// @todo: check if file exists
}

Menu2DItem::boundingBox Menu2DItem::getBoundingBox() {
	boundingBox bb = {
		m_positionY,
		m_positionX + m_width,
		m_positionY + m_height,
		m_positionX
	};

	return bb;
}

string Menu2DItem::getDescription() {
	return m_descr;
}

int Menu2DItem::getIndex() {
	return m_index;
}

void Menu2DItem::setHoverState(bool flag) {
	m_hovered = flag;
}

void Menu2DItem::setActiveState(bool flag) {
	m_activated = flag;
}

void Menu2DItem::draw() {
	animationHelper->setStartNowOrKeepIt();

	// add texture
	int state_index;
	if (m_hovered) {
		//glColor4f(0, 0, 1, animationHelper->easeLinear(0, 1));
		state_index = 1;
	} else if (m_activated) {
		//glColor4f(1, 0, 0, animationHelper->easeLinear(0, 1));
		state_index = 2;
	} else {
		//glColor4f(0, 1, 0, animationHelper->easeLinear(0, 1));
		state_index = 0;
	}

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_texture[state_index]);

    // we could abstact the openGL binding with an abstract graphic class which implements OpenGL, DirectX, ...
	glColor4f(1, 1, 1, 1);
	glBegin(GL_QUADS);
		glTexCoord2f(0, 1); glVertex2f(m_positionX, m_positionY);
		glTexCoord2f(1, 1); glVertex2f(m_positionX + m_width, m_positionY);
		glTexCoord2f(1, 0); glVertex2f(m_positionX + m_width, m_positionY + m_height);
		glTexCoord2f(0, 0); glVertex2f(m_positionX, m_positionY + m_height);
	glEnd();

	glDisable(GL_TEXTURE_2D);
}