#ifndef BUTTON_H
#define BUTTON_H

#include "Widget.h"
#include <boost/signals2.hpp>

class Button: public Widget {
public:
	Button(std::string label, Rect rect, Widget* parent)
		: Widget(parent)
		, m_pressed(false)
		, m_pressedSignal()
		, m_label(label) {
		
		setRect(rect);
	}

	virtual Button* whenClicked(const Signal::slot_type& slot) {
		m_pressed.connect(slot);
		return this;
	}

	virtual void mousePressed(Point2d pos) override {
		if (rect().inside(pos) && !m_pressed) {
			m_pressed = true;
			pressed();
		} else if (m_pressed) {
			m_pressed = false;
		}
		
		Widget::mousePressed(pos);
	}
	
	virtual void draw() override;
	
protected:
	virtual void pressed() {
		m_pressedSignal();
	}

	virtual void mouseEnter() override {
		// Start hover
	}
	
	virtual void mouseLeave() override {
		// End hover
	}
	
private:
	using Signal = boost::signal<void()>;

	bool m_pressed;
	Signal m_pressedSignal;
	std::string m_label;
};

using ButtonPtr = std::unique_ptr<Button>;

#endif // BUTTON_H
