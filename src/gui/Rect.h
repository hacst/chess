#ifndef RECT_H
#define RECT_H

#include "Point2d.h"

struct Rect {
	float x,y,w,h;
	
	bool contains(Point2d point) const;
	void move(Point2d pos) const;
	
	Point2d topLeft() const;
	Point2d topRight() const;
	Point2d bottomLeft() const;
	Point2d bottomRight() const;
	
	static Rect infinite();
};

#endif // RECT_H
