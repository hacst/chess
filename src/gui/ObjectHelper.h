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

private:
	ObjectHelper() {};
};

#endif // OBJECTHELPER_H