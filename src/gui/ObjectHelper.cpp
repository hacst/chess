#include "ObjectHelper.h"

using namespace std;

GLuint ObjectHelper::createCubeList(float size, float x, float y, float z) {
	float halfsize = size / 2.0f;

	GLuint list = glGenLists(1);

	glNewList(list, GL_COMPILE);
		glPushMatrix();
			glTranslatef(x, y, z);

			glBegin(GL_QUADS);
				glColor3f(1, 0, 0);

				GLfloat white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
				GLfloat cyan[] = { 0.f, .8f, .8f, 1.f };
				glMaterialfv(GL_FRONT, GL_DIFFUSE, cyan);
				glMaterialfv(GL_FRONT, GL_SPECULAR, white);

				GLfloat shininess[] = { 200 };
				glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

				// front face
				glNormal3f(0.0, 0.0, 1.0);
				glVertex3f(halfsize, halfsize, halfsize);	// top right
				glVertex3f(-halfsize, halfsize, halfsize);	// top left
				glVertex3f(-halfsize, -halfsize, halfsize);	// bottom left
				glVertex3f(halfsize, -halfsize, halfsize);	// bottom right

				// left face
				glNormal3f(-1.0, 0.0, 0.0);
				glVertex3f(-halfsize, halfsize, halfsize);
				glVertex3f(-halfsize, halfsize, -halfsize);
				glVertex3f(-halfsize, -halfsize, -halfsize);
				glVertex3f(-halfsize, -halfsize, halfsize);

				// back face
				glNormal3f(0.0, 0.0, -1.0);
				glVertex3f(halfsize, halfsize, -halfsize);
				glVertex3f(-halfsize, halfsize, -halfsize);
				glVertex3f(-halfsize, -halfsize, -halfsize);
				glVertex3f(halfsize, -halfsize, -halfsize);

				// right face
				glNormal3f(1.0, 0.0, 0.0);
				glVertex3f(halfsize, halfsize, -halfsize);
				glVertex3f(halfsize, halfsize, halfsize);
				glVertex3f(halfsize, -halfsize, halfsize);
				glVertex3f(halfsize, -halfsize, -halfsize);

				// top face
				glNormal3f(0.0, 1.0, 0.0);
				glVertex3f(halfsize, halfsize, halfsize);
				glVertex3f(-halfsize, halfsize, halfsize);
				glVertex3f(-halfsize, halfsize, -halfsize);
				glVertex3f(halfsize, halfsize, -halfsize);

				// bottom face
				glNormal3f(0.0, -1.0, 0.0);
				glVertex3f(halfsize, -halfsize, halfsize);
				glVertex3f(-halfsize, -halfsize, halfsize);
				glVertex3f(-halfsize, -halfsize, -halfsize);
				glVertex3f(halfsize, -halfsize, -halfsize);
			glEnd();
		glPopMatrix();
	glEndList();

	return list;
}

GLuint ObjectHelper::create2DRectList(float width, float height, float viewportX, float viewportY, float colorR, float colorG, float colorB) {
	GLuint list = glGenLists(1);

	glEnable(GL_COLOR);

	glNewList(list, GL_COMPILE);
		glPushMatrix();
			glBegin(GL_QUADS);
				glColor3f(colorR, colorG, colorB);

				glVertex3f(viewportX,			viewportY,				0.0f); // top left
				glVertex3f(viewportX + width,	viewportY,				0.0f); // top right
				glVertex3f(viewportX + width,	viewportY + height,		0.0f); // bottom right
				glVertex3f(viewportX,			viewportY + height,		0.0f); // bottom left
			glEnd();
		glPopMatrix();
	glEndList();

	glDisable(GL_COLOR);

	return list;
}

GLuint ObjectHelper::create2DGradientRectList(float width, float height, float viewportX, float viewportY, float fromColorR, float fromColorG, float fromColorB, float toColorR, float toColorG, float toColorB) {
	GLuint list = glGenLists(1);

	glEnable(GL_COLOR);

	glNewList(list, GL_COMPILE);
		glPushMatrix();
			glBegin(GL_QUADS);
				glColor3f(fromColorR, fromColorG, fromColorB);
				glVertex3f(viewportX,			viewportY,				0.0f); // top left
				glVertex3f(viewportX + width,	viewportY,				0.0f); // top right

				glColor3f(toColorR, toColorG, toColorB);
				glVertex3f(viewportX + width,	viewportY + height,		0.0f); // bottom right
				glVertex3f(viewportX,			viewportY + height,		0.0f); // bottom left
			glEnd();
		glPopMatrix();
	glEndList();

	glDisable(GL_COLOR);

	return list;
}