#include "Menu2D.h"

using namespace std;

GLuint texture[1];
int Menu2D::LoadGLTextures() {
	// load an image file directly as a new OpenGL texture
	texture[0] = SOIL_load_OGL_texture(
		"girl.jpg",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_INVERT_Y
	);
		
	if (texture[0] == 0) {
		return false;
	}

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	return true;
}

Menu2D::Menu2D(const int windowWidth, const int windowHeight) {
	menuOptions.push_back("Neues Spiel");
	menuOptions.push_back("Optionen");
	menuOptions.push_back("Spiel beenden");

	height = menuOptions.size() * (buttonHeight + buttonMargin);
	width = buttonWidth;
	marginTop = (windowHeight / 2) - (height / 2);		// center in height
	marginLeft = (windowWidth / 2) - (buttonWidth / 2);	// center in width

	// create foreach menuOption one button and add it to the vector collection
	int index = 0;
	for (auto& mo : menuOptions) {
		int btPositionY = (index * buttonHeight) + ((index + 1) * buttonMargin) + marginTop - (buttonMargin / 2);
		items.push_back(new Menu2DItem(index, mo, marginLeft, btPositionY, buttonWidth, buttonHeight, animationDuration + (animationDuration / 2)));

		++index;
	}

	// create an animation object
	animationHelper = new AnimationHelper(animationDuration);

	// load texture
	if (!LoadGLTextures()) {
		cout << "Failed to load image: " << SOIL_last_result() << endl;
	}
}

void Menu2D::draw() {
	// draw a transparent background for the menu with an animation
	animationHelper->setStartNowOrKeepIt();
	glColor4f(0, 0, 1, animationHelper->easeLinear(0.2, 0.8));

	// add texture
	glEnable(GL_TEXTURE_2D);
	//glBindTexture(GL_TEXTURE_2D, texture[0]);

	glBegin(GL_QUADS);
		glTexCoord2f(0, 0); glVertex2f(marginLeft - 20, marginTop - 20);
		glTexCoord2f(1, 0); glVertex2f(marginLeft + width + 20, marginTop - 20);
		glTexCoord2f(1, 1); glVertex2f(marginLeft + width + 20, marginTop + height + 20);
		glTexCoord2f(0, 1); glVertex2f(marginLeft - 20, marginTop + height + 20);
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