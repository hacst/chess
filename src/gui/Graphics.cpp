#ifdef _WIN32
	#include <windows.h>
#endif

#include <GL/gl.h>
#include "Graphics.h"

namespace Graphics {

void addVertex(Point2D point) {
	glVertex2f(point.x, point.y);
}

void drawRect(Rect rect) {
	glBegin(GL_QUADS);
	addVertex(rect.topLeft());
	addVertex(rect.topRight());
	addVertex(rect.bottomRight());
	addVertex(rect.bottomLeft());
	glEnd();
}


} // namespace Graphics
