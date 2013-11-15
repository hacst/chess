#include <limits>
#include "Rect.h"

using namespace std;

bool Rect::contains(Point2d point) const {
	return (point.x >= x && point.x - w <= x)
	        && (point.y >= y && point.y - h <= y);
}

void Rect::move(Point2d pos) const {
	x = pos.x;
	y = pos.y;
}

Point2d Rect::topLeft() const {
	return { x,y };
}

Point2d Rect::topRight() const {
	return { x + w, y };
}

Point2d Rect::bottomLeft() const {
	return { x, y + h };
}

Point2d Rect::bottomRight() const {
	return { x + w, y + h };
}

Rect Rect::infinite() {
	return {0, 0, numeric_limits<float>::infinity(), numeric_limits<float>::infinity() };
}
