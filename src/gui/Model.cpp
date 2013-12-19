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
	m_correctRotation = { rotateX, rotateY, rotateZ };
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
		bool flipModelDirection = false;
		GLfloat emission[] = { 0.3f, 0.3f, 0.3f, 1.0f };
		GLfloat ambient[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		GLfloat diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		GLfloat specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };

		if (m_color == Color::BLACK) {
			flipModelDirection = true;

			emission[0] = 0.0f;
			emission[1] = 0.0f;
			emission[2] = 0.0f;
			emission[3] = 0.2f;

			ambient[0] = 0.0f;
			ambient[1] = 0.0f;
			ambient[2] = 0.0f;
			ambient[3] = 0.2f;

			diffuse[0] = 0.2f;
			diffuse[1] = 0.2f;
			diffuse[2] = 0.2f;
			diffuse[3] = 1.0f;

			specular[0] = 0.0f;
			specular[1] = 0.0f;
			specular[2] = 0.0f;
			specular[3] = 1.0f;
		}

		GLfloat shininess[] = { 100 };

		glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
		glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
		glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
		glMaterialfv(GL_FRONT, GL_EMISSION, emission);
		glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

		// note: transformation matrices are multiplied from right -> last matrix first!
		glTranslatef((float)m_position.x, (float)m_position.y, (float)m_position.z);							// 5) translate to world space coordinates
		glScalef(m_correctScaling, m_correctScaling, m_correctScaling);					// 4) scale
		glRotatef(flipModelDirection ? 180.0f : 0.0f, 0.0f, 1.0f, 0.0f);					// 3) correct rotation y (assertion: we are now in (0,0,0) local space because of step 2)
        glTranslatef((float) m_correctPosition.x, (float) m_correctPosition.y, (float) m_correctPosition.z);	// 2) correct local origin
		glRotatef(-90.0, 1.0f, 0.0f, 0.0f);												// 1) correct rotation around x-axis to "stand-up" the model

		model->drawScene();																// 6) draw the scene
	glPopMatrix();
}
