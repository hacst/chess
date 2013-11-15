#include "Menu2D.h"
#include "GuiWindow.h"

using namespace std;

Menu2D::Menu2D(int windowWidth, int windowHeight, GuiWindow& guiWindow) : m_window(guiWindow) {
	menuOptions.push_back("Neues Spiel");
	menuOptions.push_back("Spiel laden");
	menuOptions.push_back("Optionen");
	menuOptions.push_back("Spiel beenden");

	m_height = menuOptions.size() * (m_buttonHeight + m_buttonMargin);
	m_width = m_buttonWidth;
	m_marginTop = (windowHeight / 2) - (m_height / 2);		// center in height
	m_marginLeft = (windowWidth / 2) - (m_buttonWidth / 2);	// center in width

	// create foreach menuOption one button and add it to the vector collection
	int index = 0;
	for (auto& mo : menuOptions) {
		int btPositionY = (index * m_buttonHeight) + ((index + 1) * m_buttonMargin) + m_marginTop - (m_buttonMargin / 2);
		items.push_back(make_shared<Menu2DItem>(index, mo, m_marginLeft, btPositionY, m_buttonWidth, m_buttonHeight, m_animationDuration + (m_animationDuration / 2)));

		++index;
	}

	// create an animation object
	animationHelper = make_shared<AnimationHelper>(m_animationDuration);
}

void Menu2D::draw() {
	// draw a transparent background for the menu with an animation
	animationHelper->setStartNowOrKeepIt();
	glColor4f(1.0, 0.0, 0.0, animationHelper->easeLinear(0.1, 1.0));

	glBegin(GL_QUADS);
		glVertex2f(m_marginLeft - 20,				m_marginTop - 20			);
		glVertex2f(m_marginLeft + m_width + 20,		m_marginTop - 20			);
		glVertex2f(m_marginLeft + m_width + 20,		m_marginTop + m_height + 20	);
		glVertex2f(m_marginLeft - 20,				m_marginTop + m_height + 20	);
	glEnd();

	// iterate over all items and let them draw
	for (auto& item : items) {
		item->draw();
	}
}

void Menu2D::mouseMoved(const int x, const int y) {
	mouseState.x = x;
	mouseState.y = y;

	changeItemState(Menu2D::ITEM_STATE_TYPE_MOVED);
}

void Menu2D::mouseClicked() {
	// clicked at already known x/y position
	changeItemState(Menu2D::ITEM_STATE_TYPE_PRESSED);
}

void Menu2D::mouseReleased() {
	changeItemState(Menu2D::ITEM_STATE_TYPE_RELEASED);
}

void Menu2D::changeItemState(const int type) {
	Menu2DItem::boundingBox bb;
	for (auto& item : items) {
		bb = item->getBoundingBox();

		// for performance reasons we could disable this checks after first button is hovered
		// all others are then not hovered
		bool inX = mouseState.x >= bb.left && mouseState.x <= bb.right;
		bool inY = mouseState.y >= bb.top && mouseState.y <= bb.bottom;

		if (inX && inY) {
			if (type == Menu2D::ITEM_STATE_TYPE_RELEASED) {
				activeButton = item;

				cout << "released button #" << activeButton->getIndex() << ":" << activeButton->getDescription() << endl;
				// switch menu or call GuiWindow again to switch context

				item->setHoverState(true);
				item->setActiveState(false);
			} else if (type == Menu2D::ITEM_STATE_TYPE_MOVED) {
				item->setHoverState(true);
				item->setActiveState(false);
			} else if (type == Menu2D::ITEM_STATE_TYPE_PRESSED) {
				item->setHoverState(false);
				item->setActiveState(true);
			}
		} else {
			item->setHoverState(false);
			item->setActiveState(false);
		}
	}
}