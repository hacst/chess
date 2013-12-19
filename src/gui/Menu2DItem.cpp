#include "gui/Menu2DItem.h"

#include <iostream>
#include <sstream>

using namespace std;

Menu2DItem::Menu2DItem(int index, std::string filename, int width, int height, int animationDuration)
	: m_index(index)
	, m_width(width)
	, m_height(height)
	, m_filename(filename) {

	// create an animation object
	animationHelper = make_shared<AnimationHelper>(animationDuration);

	// load normal state texture
	m_texture[0] = SOIL_load_OGL_texture(
		("resources/bt_n" + m_filename).c_str(),
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y
	);

	// load hover state texture
	m_texture[1] = SOIL_load_OGL_texture(
		("resources/bt_h" + m_filename).c_str(),
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y
		);

	// load active state texture
	m_texture[2] = SOIL_load_OGL_texture(
		("resources/bt_a" + m_filename).c_str(),
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y
		);
}

Menu2DItem::~Menu2DItem() {
	m_clicked.disconnect_all_slots();
}

void Menu2DItem::setPosition(int x, int y) {
	m_positionX = x;
	m_positionY = y;
}

bool Menu2DItem::inBoundingBox() {
	bool inX = m_mousePosX >= m_positionX && m_mousePosX <= m_positionX + m_width;
	bool inY = m_mousePosY >= m_positionY && m_mousePosY <= m_positionY + m_height;

	return inX && inY;
}

void Menu2DItem::draw() {
	animationHelper->setStartNowOrKeepIt();

	// add texture
	int state_index;
	if (m_hovered) {
		state_index = 1;
	} else if (m_activated) {
		state_index = 2;
	} else {
		state_index = 0;
	}

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, m_texture[state_index]);

    // we could abstact the openGL binding with an abstract graphic class which implements OpenGL, DirectX, ...
	glColor4f(1.f, 1.f, 1.f, 1.f);
	glBegin(GL_QUADS);
		glTexCoord2i(0, 1); glVertex2i(m_positionX, m_positionY);
		glTexCoord2i(1, 1); glVertex2i(m_positionX + m_width, m_positionY);
		glTexCoord2i(1, 0); glVertex2i(m_positionX + m_width, m_positionY + m_height);
		glTexCoord2i(0, 0); glVertex2i(m_positionX, m_positionY + m_height);
	glEnd();

	glDisable(GL_TEXTURE_2D);
}

void Menu2DItem::mouseMoved(int x, int y) {
	m_mousePosX = x;
	m_mousePosY = y;

	if (inBoundingBox()) {
		m_hovered = true;
		m_activated = false;
	} else {
		m_hovered = false;
		m_activated = false;
	}
}

void Menu2DItem::mousePressed(int x, int y) {
	m_mousePosX = x;
	m_mousePosY = y;

	if (inBoundingBox()) {
		m_hovered = false;
		m_activated = true;
	}
}

void Menu2DItem::mouseReleased(int x, int y) {
	m_mousePosX = x;
	m_mousePosY = y;

	if (inBoundingBox()) {
		m_hovered = true;
		m_activated = false;

		m_clicked();	// boost signal
	}
}

void Menu2DItem::onClick(const boost::function<void()>& slot) {
	m_clicked.connect(slot);
}

void Menu2DItem::unClick() {
    m_clicked.disconnect_all_slots();
}

string Menu2DItem::toString() const {
    stringstream ss;
    ss << "Nenu2DItem(index=" << m_index << ")";
    return ss.str();
}
