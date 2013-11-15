#ifndef WIDGET_H
#define WIDGET_H

#include "Point2d.h"
#include "Rect.h"

#include <vector>
#include <memory>
#include <boost/noncopyable.hpp>

class Widget : public boost::noncopyable {
public:
	Widget(Widget* parent)
		: m_rect(Rect::infinite())
		, m_mouseInside(false)
		, m_children()
		, m_parent(parent) {
		// Nothing
	}

	virtual ~Widget() {}

	virtual void mouseMoved(Point2d pos) {
		if (rect().contains(pos) && !m_mouseInside) {
			mouseEnter();
			m_mouseInside = true;
		} else if (m_mouseInside) {
			mouseLeave();
			m_mouseInside = false;
		}
		
		for (auto& child: children) {
			child.handleMouse(pos);
		}
	}

	virtual void mousePressed(Point2d pos) {
		for (auto& child: children) {
			child.mousePressed(pos);
		}
	}

	virtual void draw() {
		for (auto& child: children) {
			child.draw();
		}
	}

	virtual Widget* addChild(WidgetPtr child) {
		m_children.push_back(child);
		return child.get();
	}

	virtual void setRect(Rect rect) {
		m_rect = rect;
	}
	
	virtual void setPos(Point2d pos) {
		m_rect.move(pos);
	}
	
	virtual const Rect& rect() const {
		return m_rect;
	}
	
	virtual bool isMouseInside() {
		return m_mouseInside;
	}
	
protected:
	virtual void mouseEnter() { /* Nothing */ }
	virtual void mouseLeave() { /* Nothing */ }

private:
	Rect m_rect;
	
	bool m_mouseInside;

	std::vector<WidgetPtr> m_children;
	Widget* m_parent;
};

using WidgetPtr = std::unique_ptr<Widget>;

#endif // WIDGET_H
