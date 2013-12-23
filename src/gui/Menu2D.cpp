#include "gui/Menu2D.h"
#include "gui/GuiWindow.h"

using namespace std;

Menu2D::Menu2D(int windowWidth, int windowHeight)
	: m_windowWidth(windowWidth)
	, m_windowHeight(windowHeight)
	, m_btCount(0) {

	// create an animation object
	animationHelper = make_shared<AnimationHelper>(m_animationDuration);
}

Menu2D::~Menu2D() {
	for (auto& item : items) {
		item->unClick();
		item.reset();
	}
	
	items.clear();
}

Menu2DItemPtr Menu2D::addButton(std::string filename) {
	updateAbsolutePosition();	// update absolute menu position
	
	// we don't know how many buttons will be added in total, so we must update the position later (before drawing)
	Menu2DItemPtr item = make_shared<Menu2DItem>(m_btCount, filename, m_buttonWidth, m_buttonHeight, m_animationDuration * 2);
	items.push_back(item);

	++m_btCount;

	return item;
}

void Menu2D::updateAbsolutePosition() {
	m_height = (m_btCount + 1) * (m_buttonHeight + m_buttonMargin);	// total height of menu
	m_marginTop = (m_windowHeight / 2) - (m_height / 2);			// center in height
	m_marginLeft = (m_windowWidth / 2) - (m_buttonWidth / 2);		// center in width
}

void Menu2D::draw() {
	glEnable(GL_COLOR);
	glEnable(GL_BLEND);

	glPushMatrix();

	// now we know how many buttons are on our list, so we can position them right now
	int index = 0;
	for (auto& item : items) {
		int x = m_marginLeft;	// always the same distance to left
		int y = (index * m_buttonHeight) + ((index + 1) * m_buttonMargin) + m_marginTop - (m_buttonMargin / 2);

		item->setPosition(x, y);
		++index;
	}

	// draw a transparent background for the menu with an animation
	animationHelper->setStartNowOrKeepIt();
	glColor4f(0.2f, 0.2f, 0.3f, animationHelper->ease(AnimationHelper::EASE_LINEAR, 0.1f, 0.9f));

	glBegin(GL_QUADS);
		glVertex2i(m_marginLeft - 20,					m_marginTop - 20			);
		glVertex2i(m_marginLeft + m_buttonWidth + 20,	m_marginTop - 20			);
		glVertex2i(m_marginLeft + m_buttonWidth + 20,	m_marginTop + m_height + 20	);
		glVertex2i(m_marginLeft - 20,					m_marginTop + m_height + 20	);
	glEnd();

	// iterate over all items and let them draw
	for (auto& item : items) {
		item->draw();
	}

	glPopMatrix();

	glDisable(GL_BLEND);
	glDisable(GL_COLOR);
}

void Menu2D::mouseMoved(const int x, const int y) {
	mouseState.x = x;
	mouseState.y = y;

	for (auto& item : items) {
		item->mouseMoved(mouseState.x, mouseState.y);
	}
}

void Menu2D::mousePressed() {
	// clicked at already known x/y position
	for (auto& item : items) {
		item->mousePressed(mouseState.x, mouseState.y);
	}
}

void Menu2D::mouseReleased() {
	// released at already known x/y position
	for (auto& item : items) {
		item->mouseReleased(mouseState.x, mouseState.y);
	}
}

void Menu2D::windowResized(int newWidth, int newHeight) {
	m_windowWidth = newWidth;
	m_windowHeight = newHeight;

	updateAbsolutePosition();
}

void Menu2D::resetAnimation() {
	animationHelper->reset();
}