#include "ChessSet.h"

#include <algorithm>
#include <iostream>
#include <SDL.h>

using namespace std;

ChessSet::ChessSet() {
	m_tileWidth = 24;
	m_tileHeight = 0;

	// all external resources to load
	extResources = {
		"resources/3dmodels/king.3DS",
		"resources/3dmodels/queen.3DS",
		"resources/3dmodels/bishop.3DS",
		"resources/3dmodels/knight.3DS",
		"resources/3dmodels/rook.3DS",
		"resources/3dmodels/pawn.3DS"
	};

	extCorrectionValues = {{
		{ { 61, 11, 0, 1, -90, 0, 0 } },	// king
		{ { 40, 11, 0, 1, -90, 0, 0 } },	// queen
		{ { 1,  19, 0, 1,   -90, 0, 0 } },	// bishop
		{ { 0,  0,  0, 0.4, -90, 0, 0 } },	// knight
		{ { 20, 11, 0, 1, -90, 0, 0 } },	// rook
		{ { 0,  12, 0, 1,   -90, 0, 0 } }	// pawn
	}};
}

ChessSet::~ChessSet() {
	m_loadCallback.disconnect_all_slots();
}

int ChessSet::getResourcesCount() {
	return extResources.size() + 1;	// +1 for the board
}

void ChessSet::loadResources() {
	// load models
	int i = 0;
	for (auto &resName : extResources) {
		// propagate what is loaded
		m_loadCallback(resName);

		// create the model
		models[i] = make_shared<Model>(resName);

		// set the correction values for the model
		models[i]->setCorrectionValues( 
			extCorrectionValues[i][0],
			extCorrectionValues[i][1],
			extCorrectionValues[i][2],
			extCorrectionValues[i][3],
			extCorrectionValues[i][4],
			extCorrectionValues[i][5],
			extCorrectionValues[i][6]
		);

		++i;
	}

	// prepare board list for faster rendering
	m_loadCallback("Preparing chessboard and models ...");
	createChessBoardList();
}

// triggers the callback, when a new source has been loaded
void ChessSet::registerLoadCallback(const boost::function<void(std::string)>& slot) {
	m_loadCallback.connect(slot);
}

void ChessSet::setState(std::array<Piece, 64> state) {
	// set position for models and keep it in display list
	glDeleteLists(m_modelsList, 1);
	m_modelsList = glGenLists(1);
	
	glNewList(m_modelsList, GL_COMPILE);
	int field = 0;
	for (auto &p : state) {
		if (p.type != PieceType::NoType) {
			models[p.type]->setColor(p.player == PlayerColor::White ? Model::Color::WHITE : Model::Color::BLACK);
			moveModelToTile(models[p.type], field % 8, field / 8);
			models[p.type]->draw();
		}
		++field;
	}
	glEndList();
}

/*
void ChessSet::updateModelPositions(std::array<Piece, 64> board) {
	// set position for models and keep it in display list
	m_modelsList = glGenLists(1);
	
	// @todo: delete list after not using it any more
	glNewList(m_modelsList, GL_COMPILE);
		// ===== bishop =====	
		models[0]->setColor(Model::Color::WHITE);
		moveModelToTile(models[0], 2, 0);
		models[0]->draw();

		moveModelToTile(models[0], 5, 0);
		models[0]->draw();

		models[0]->setColor(Model::Color::BLACK);
		models[0]->rotateY(180);
		moveModelToTile(models[0], 2, 7);
		models[0]->draw();

		moveModelToTile(models[0], 5, 7);
		models[0]->draw();

		// ===== king =====
		models[1]->setColor(Model::Color::WHITE);
		moveModelToTile(models[1], 4, 0);
		models[1]->draw();

		models[1]->setColor(Model::Color::BLACK);
		models[1]->rotateY(180);
		moveModelToTile(models[1], 4, 7);
		models[1]->draw();

		// ===== knight =====
		models[2]->setColor(Model::Color::WHITE);
		moveModelToTile(models[2], 1, 0);
		models[2]->draw();

		moveModelToTile(models[2], 6, 0);
		models[2]->draw();

		models[2]->setColor(Model::Color::BLACK);
		models[2]->rotateY(180);
		moveModelToTile(models[2], 1, 7);
		models[2]->draw();

		moveModelToTile(models[2], 6, 7);
		models[2]->draw();

		// ===== pawn =====
		models[3]->setColor(Model::Color::WHITE);
		moveModelToTile(models[3], 0, 1);
		models[3]->draw();

		moveModelToTile(models[3], 1, 1);
		models[3]->draw();

		moveModelToTile(models[3], 2, 1);
		models[3]->draw();

		moveModelToTile(models[3], 3, 1);
		models[3]->draw();

		moveModelToTile(models[3], 4, 1);
		models[3]->draw();

		moveModelToTile(models[3], 5, 1);
		models[3]->draw();

		moveModelToTile(models[3], 6, 1);
		models[3]->draw();

		moveModelToTile(models[3], 7, 1);
		models[3]->draw();

		models[3]->setColor(Model::Color::BLACK);
		models[3]->rotateY(180);
		moveModelToTile(models[3], 0, 6);
		models[3]->draw();

		moveModelToTile(models[3], 1, 6);
		models[3]->draw();

		moveModelToTile(models[3], 2, 6);
		models[3]->draw();

		moveModelToTile(models[3], 3, 6);
		models[3]->draw();

		moveModelToTile(models[3], 4, 6);
		models[3]->draw();

		moveModelToTile(models[3], 5, 6);
		models[3]->draw();

		moveModelToTile(models[3], 6, 6);
		models[3]->draw();

		moveModelToTile(models[3], 7, 6);
		models[3]->draw();

		// ===== queen =====
		models[4]->setColor(Model::Color::WHITE);
		moveModelToTile(models[4], 3, 0);
		models[4]->draw();

		models[4]->setColor(Model::Color::BLACK);
		models[4]->rotateY(180);
		moveModelToTile(models[4], 3, 7);
		models[4]->draw();

		// ===== rook =====
		models[5]->setColor(Model::Color::WHITE);
		moveModelToTile(models[5], 0, 0);
		models[5]->draw();

		moveModelToTile(models[5], 7, 0);
		models[5]->draw();

		models[5]->setColor(Model::Color::BLACK);
		models[5]->rotateY(180);
		moveModelToTile(models[5], 0, 7);
		models[5]->draw();

		moveModelToTile(models[5], 7, 7);
		models[5]->draw();
	glEndList();
}*/

void ChessSet::draw() {
	// models
	glCallList(m_modelsList);

	// chessboard
	glCallList(m_boardList);
}

void ChessSet::moveModelToTile(ModelPtr model, int row, int col) {
	model->setPosition(
		((row - 4) * m_tileWidth) + (m_tileWidth / 2) /* x */,
		0 /* y */,
		((col - 4) * m_tileWidth) + (m_tileWidth / 2) /* z */
	);
}

void ChessSet::createChessBoardList() {
	m_boardList = glGenLists(1);

	glNewList(m_boardList, GL_COMPILE);
		int x = 0;
		int y = 0;
		int z = 0;

		bool oddToggler = false;
		for (int i = -4; i < 4; i++) {
			z = i * m_tileWidth + m_tileWidth / 2;

			for (int j = -4; j < 4; j++) {
				x = j * m_tileWidth + m_tileWidth / 2;
			
				drawTile(x, y, z, oddToggler, false);
				oddToggler = !oddToggler;
			}

			oddToggler = !oddToggler;
		}
	glEndList();
}

void ChessSet::drawTile(int x, int y, int z, bool odd, bool highlight) {
	int halfWidth = m_tileWidth / 2;
	int halfHeight = m_tileHeight;
	
	glPushMatrix();
		glTranslatef(x, y, z);

		glBegin(GL_QUADS);
			if (odd) {
				glColor3f(0, 0, 0);
			} else {
				glColor3f(1, 1, 1);
			}

			GLfloat white[] = { 1.0f, 1.0f, 1.0f, 1.0f };
			GLfloat diffuse[] = { 1.0f, 1.0f, 1.0f, 1.f };
			glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
			glMaterialfv(GL_FRONT, GL_SPECULAR, white);

			GLfloat shininess[] = { 100 };
			glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

			// front face
			glNormal3f(0.0, 0.0, 1.0);
			glVertex3f(halfWidth, halfHeight, halfWidth);	// top right
			glVertex3f(-halfWidth, halfHeight, halfWidth);	// top left
			glVertex3f(-halfWidth, -halfHeight, halfWidth);	// bottom left
			glVertex3f(halfWidth, -halfHeight, halfWidth);	// bottom right

			// left face
			glNormal3f(-1.0, 0.0, 0.0);
			glVertex3f(-halfWidth, halfHeight, halfWidth);
			glVertex3f(-halfWidth, halfHeight, -halfWidth);
			glVertex3f(-halfWidth, -halfHeight, -halfWidth);
			glVertex3f(-halfWidth, -halfHeight, halfWidth);

			// back face
			glNormal3f(0.0, 0.0, -1.0);
			glVertex3f(halfWidth, halfHeight, -halfWidth);
			glVertex3f(-halfWidth, halfHeight, -halfWidth);
			glVertex3f(-halfWidth, -halfHeight, -halfWidth);
			glVertex3f(halfWidth, -halfHeight, -halfWidth);

			// right face
			glNormal3f(1.0, 0.0, 0.0);
			glVertex3f(halfWidth, halfHeight, -halfWidth);
			glVertex3f(halfWidth, halfHeight, halfWidth);
			glVertex3f(halfWidth, -halfHeight, halfWidth);
			glVertex3f(halfWidth, -halfHeight, -halfWidth);

			// top face
			glNormal3f(0.0, 1.0, 0.0);
			glVertex3f(halfWidth, halfHeight, halfWidth);
			glVertex3f(-halfWidth, halfHeight, halfWidth);
			glVertex3f(-halfWidth, halfHeight, -halfWidth);
			glVertex3f(halfWidth, halfHeight, -halfWidth);

			// bottom face
			glNormal3f(0.0, -1.0, 0.0);
			glVertex3f(halfWidth, -halfHeight, halfWidth);
			glVertex3f(-halfWidth, -halfHeight, halfWidth);
			glVertex3f(-halfWidth, -halfHeight, -halfWidth);
			glVertex3f(halfWidth, -halfHeight, -halfWidth);
		glEnd();
	glPopMatrix();
}