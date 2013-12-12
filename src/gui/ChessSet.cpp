#include "ChessSet.h"

#include <algorithm>
#include <iostream>
#include <SDL.h>

using namespace std;

ChessSet::ChessSet() {
	m_tileWidth = 24;
	m_tileHeight = 0;

	// load models
	bishop = make_shared<Model>("resources/3dmodels/bishop.3DS");
	king = make_shared<Model>("resources/3dmodels/king.3DS");
	knight = make_shared<Model>("resources/3dmodels/knight.3DS");
	pawn = make_shared<Model>("resources/3dmodels/pawn.3DS");
	queen = make_shared<Model>("resources/3dmodels/queen.3DS");
	rook = make_shared<Model>("resources/3dmodels/rook.3DS");

	// manual post process to correct
	bishop->setCorrectionValues(1, 19, 0, 1, -90, 0, 0);
	king->setCorrectionValues(61, 11, 0, 1, -90, 0, 0);
	knight->setCorrectionValues(0, 0, 0, 0.4, -90, 0, 0);
	pawn->setCorrectionValues(0, 12, 0, 1, -90, 0, 0);
	queen->setCorrectionValues(40, 11, 0, 1, -90, 0, 0);
	rook->setCorrectionValues(20, 11, 0, 1, -90, 0, 0);

	// prepare board list for faster rendering
	createChessBoardList();

	update(0);
}

// @todo: UPDATE here!
void ChessSet::update(int bitboard) {
	// set position for models and keep it in display list
	m_modelsList = glGenLists(1);
	
	// @todo: delete list after not using it any more
	glNewList(m_modelsList, GL_COMPILE);
		// ===== bishop =====	
		bishop->setColor(Model::Color::WHITE);
		moveModelToTile(bishop, 2, 0);
		bishop->draw();

		moveModelToTile(bishop, 5, 0);
		bishop->draw();

		bishop->setColor(Model::Color::BLACK);
		bishop->rotateY(180);
		moveModelToTile(bishop, 2, 7);
		bishop->draw();

		moveModelToTile(bishop, 5, 7);
		bishop->draw();

		// ===== king =====
		king->setColor(Model::Color::WHITE);
		moveModelToTile(king, 4, 0);
		king->draw();

		king->setColor(Model::Color::BLACK);
		king->rotateY(180);
		moveModelToTile(king, 4, 7);
		king->draw();

		// ===== knight =====
		knight->setColor(Model::Color::WHITE);
		moveModelToTile(knight, 1, 0);
		knight->draw();

		moveModelToTile(knight, 6, 0);
		knight->draw();

		knight->setColor(Model::Color::BLACK);
		knight->rotateY(180);
		moveModelToTile(knight, 1, 7);
		knight->draw();

		moveModelToTile(knight, 6, 7);
		knight->draw();

		// ===== pawn =====
		pawn->setColor(Model::Color::WHITE);
		moveModelToTile(pawn, 0, 1);
		pawn->draw();

		moveModelToTile(pawn, 1, 1);
		pawn->draw();

		moveModelToTile(pawn, 2, 1);
		pawn->draw();

		moveModelToTile(pawn, 3, 1);
		pawn->draw();

		moveModelToTile(pawn, 4, 1);
		pawn->draw();

		moveModelToTile(pawn, 5, 1);
		pawn->draw();

		moveModelToTile(pawn, 6, 1);
		pawn->draw();

		moveModelToTile(pawn, 7, 1);
		pawn->draw();

		pawn->setColor(Model::Color::BLACK);
		pawn->rotateY(180);
		moveModelToTile(pawn, 0, 6);
		pawn->draw();

		moveModelToTile(pawn, 1, 6);
		pawn->draw();

		moveModelToTile(pawn, 2, 6);
		pawn->draw();

		moveModelToTile(pawn, 3, 6);
		pawn->draw();

		moveModelToTile(pawn, 4, 6);
		pawn->draw();

		moveModelToTile(pawn, 5, 6);
		pawn->draw();

		moveModelToTile(pawn, 6, 6);
		pawn->draw();

		moveModelToTile(pawn, 7, 6);
		pawn->draw();

		// ===== queen =====
		queen->setColor(Model::Color::WHITE);
		moveModelToTile(queen, 3, 0);
		queen->draw();

		queen->setColor(Model::Color::BLACK);
		queen->rotateY(180);
		moveModelToTile(queen, 3, 7);
		queen->draw();

		// ===== rook =====
		rook->setColor(Model::Color::WHITE);
		moveModelToTile(rook, 0, 0);
		rook->draw();

		moveModelToTile(rook, 7, 0);
		rook->draw();

		rook->setColor(Model::Color::BLACK);
		rook->rotateY(180);
		moveModelToTile(rook, 0, 7);
		rook->draw();

		moveModelToTile(rook, 7, 7);
		rook->draw();
	glEndList();
}

void ChessSet::draw() {
	// models
	glCallList(m_modelsList);

	// chessboard
	glCallList(m_boardList);
}

void ChessSet::moveModelToTile(ModelPtr model, int row, int col) {
	model->setPosition(((row - 4) * m_tileWidth) + (m_tileWidth / 2) /* x */, 0 /* y */, ((col - 4) * m_tileWidth) + (m_tileWidth / 2) /* z */);
	/*glPushMatrix();
		glTranslatef(
			((row - 4) * m_tileWidth) + (m_tileWidth / 2), 
			0,
			((col - 4) * m_tileWidth) + (m_tileWidth / 2)
		);
		model->draw();
	glPopMatrix();*/
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


void ChessSet::makeDebug(int direction) {
	switch (direction) {
	case 1:
		y += 1;
		break;
	case 2:
		x += 1;
		break;
	case 3:
		y -= 1;
		break;
	case 4:
		x -= 1;
		break;
	default:
		break;
	}

	std::cout << y << std::endl;
}