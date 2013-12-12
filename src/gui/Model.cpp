#include <iostream>

#include "gui/Model.h"

using namespace std;

Model::Model(string file) :	m_file(file) {
	loadScene();

	m_correctPosition = { 0, 0, 0 };
	m_correctScaling = 1.0f;
	m_correctRotation = { 0, 0, 0 };

	m_rotateY = 0;

	m_position = { 0, 0, 0 };
}

void Model::setCorrectionValues(int localX, int localY, int localZ, 
								float scaleFactor,
								int rotateX, int rotateY, int rotateZ) {
	m_correctPosition = { localX, localY, localZ };
	m_correctScaling = scaleFactor;
	m_correctRotation = { rotateX, rotateY, rotateY };
}

void Model::setColor(Color color) {
	m_color = color;
}

void Model::setPosition(int globalX, int globalY, int globalZ) {
	m_position = { globalX, globalY, globalZ };
}

void Model::rotateY(int degree) {
	m_rotateY = degree;
}

void Model::loadScene() {
	model = make_shared<AssimpHelper>();
	model->importScene(m_file);
}

void Model::draw() {
	glPushMatrix();
		// see http://www.opengl.org/sdk/docs/man2/xhtml/glMaterial.xml
		
		GLfloat emission[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		GLfloat ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		if (m_color == Color::BLACK) {
			emission[0] = 0.0f;
			emission[1] = 0.0f;
			emission[2] = 0.0f;

			ambient[0] = 1.0f;
			ambient[1] = 1.0f;
			ambient[2] = 1.0f;
		}
		
		
		GLfloat diffuse[] = { 1.0f, 0.0f, 0.0f, 1.f };
		GLfloat specular[] = { 1.0f, 1.0f, 0.5f, 1.f };

		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
		glMaterialfv(GL_FRONT, GL_EMISSION, emission);
		
		GLfloat shininess[] = { 120 };
		glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

		glTranslatef(m_position.x, m_position.y, m_position.z);							// global position	
		glTranslatef(m_correctPosition.x, m_correctPosition.y, m_correctPosition.z);	// correct local origin
		glRotatef(m_correctRotation.x, 1, 0, 0);										// correct rotation
		glRotatef(m_correctRotation.y, 0, 1, 0);
		glRotatef(m_correctRotation.z, 0, 0, 1);
		glScalef(m_correctScaling, m_correctScaling, m_correctScaling);					// correct scaling
		m_color == Color::WHITE ? glColor3f(1, 1, 1) : glColor3f(0, 0, 0);				// color
		//glRotatef(m_rotateY, 0, 1, 0);													// local rotation

		model->drawScene();
	glPopMatrix();
}
