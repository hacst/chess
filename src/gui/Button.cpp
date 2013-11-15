#include "Graphics.h"
#include "Button.h"

void Button::draw() {
	if (m_pressed) {
		glColor4f(1, 0, 0, 1);
	}
	else if (isMouseInside()) {
		glColor4f(0, 1, 0, 1);
	}
	else {
		glColor4f(0, 0, 1, 1);
	}
	
	drawRect(rect());
}


