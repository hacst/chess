#ifndef OBJECTHELPER_H
#define OBJECTHELPER_H

#ifdef _WIN32
#include <windows.h> // Needed for gl.h
#endif

#include <GL/gl.h>
#include <memory>

class ObjectHelper {
public:
	// methods
	static GLuint createCubeList(float size, float x, float y, float z);
	static GLuint create2DRectList(float width, float height, float viewportX, float viewportY, float colorR, float colorG, float colorB);
	static GLuint create2DGradientRectList(float width, float height, float viewportX, float viewportY, float fromColorR, float fromColorG, float fromColorB, float toColorR, float toColorG, float toColorB);

private:
	ObjectHelper() {};
};

#endif // OBJECTHELPER_H