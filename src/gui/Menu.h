#ifndef MENU_H
#define MENU_H

#include "Widget.h"
#include "Button.h"

class Menu: public Widget {
public:
	Menu(Widget* parent)
		: Widget(parent) {
	}

	Button* addButton(std::string text) {
		ButtonPtr button = std::unique_ptr<Button>(
		            text, { rect().x, rect().y + m_offsetY , 100, 50}, *this);
		addChild(button);
		m_offsetY += Y_OFFSET + butto->rect().h;
		return button.get();
	}

private:
	static const int Y_OFFSET = 20;
	int m_offsetY = 0;
};

#endif // MENU_H
