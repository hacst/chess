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

    extCorrectionValues = { {
        { 61, 11, 0, 1, -90, 0, 0 },	// king
        { 40, 11, 0, 1, -90, 0, 0 },	// queen
        { 1, 19, 0, 1, -90, 0, 0 },		// bishop
        { 0, 0, 0, 0.4, -90, 0, 0 },	// knight
        { 20, 11, 0, 1, -90, 0, 0 },	// rook
        { 0, 12, 0, 1, -90, 0, 0 }		// pawn
    } };
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
        auto& cor = extCorrectionValues[i];
        models[i]->setCorrectionValues(
            cor.x, cor.y, cor.z,
            cor.scale,
            cor.rotX, cor.rotY, cor.rotZ
        );

		// cache the model in a display list

		// white models
		models[i]->setColor(Model::Color::WHITE);
		m_modelList[i] = glGenLists(1);

		glNewList(m_modelList[i], GL_COMPILE);
			models[i]->draw();
		glEndList();

		// black models
		models[i]->setColor(Model::Color::BLACK);
		m_modelList[i+6] = glGenLists(1);

		glNewList(m_modelList[i+6], GL_COMPILE);
			models[i]->draw();
		glEndList();

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
	m_state = state;
}

void ChessSet::draw() {
	// 1) drawing models which have not been modified in position at first

	// 2) animating and drawing modified models here

	int field = 0;
	for (auto &p : m_state) {
		if (p.type != PieceType::NoType) {
			int row = field % 8;
			int col = field / 8;

			glPushMatrix();
				glTranslatef(((row - 4.f) * m_tileWidth) + (m_tileWidth / 2.f), 0, ((col - 4.f) * m_tileWidth) + (m_tileWidth / 2.f));
				glCallList(m_modelList[p.type + (p.player == PlayerColor::Black ? 6 : 0)]);
			glPopMatrix();
		}
		++field;
	}

	// DEPRECATED: models
	//glCallList(m_modelsList);

	// chessboard
	glCallList(m_boardList);
}

// replace by translation
void ChessSet::moveModelToTile(ModelPtr model, int row, int col) {
	/*model->setPosition(
	((row - 4) * m_tileWidth) + (m_tileWidth / 2),
	0,
	((col - 4) * m_tileWidth) + (m_tileWidth / 2)
	); */
	glTranslatef(((row - 4.f) * m_tileWidth) + (m_tileWidth / 2.f), 0, ((col - 4.f) * m_tileWidth) + (m_tileWidth / 2.f));
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
    glTranslatef(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
		
		glBegin(GL_QUADS);
			GLfloat emission[] = { 0.7f, 0.7f, 0.7f, 1.0f };		// material emits this color (not light-dependent)

			if (odd) {
				emission[0] = 0.0f;
				emission[1] = 0.0f;
				emission[2] = 0.0f;
			}

			GLfloat ambient[] = { 1.0f, 0.0f, 0.0f, 1.0f };			// representating the amount (percentage) of light which is reflected
			GLfloat diffuse[] = { 0.0f, 0.0f, 0.0f, 1.f };
			GLfloat specular[] = { 0.0f, 0.0f, 0.0f, 1.0f };
			GLfloat shininess[] = { 128 };

			glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
			glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
			glMaterialfv(GL_FRONT, GL_EMISSION, emission);	// OK
			glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

			// front face
			glVertex3i(halfWidth, halfHeight, halfWidth);	// top right
			glVertex3i(-halfWidth, halfHeight, halfWidth);	// top left
			glVertex3i(-halfWidth, -halfHeight, halfWidth);	// bottom left
			glVertex3i(halfWidth, -halfHeight, halfWidth);	// bottom right

			// left face
			glVertex3i(-halfWidth, halfHeight, halfWidth);
			glVertex3i(-halfWidth, halfHeight, -halfWidth);
			glVertex3i(-halfWidth, -halfHeight, -halfWidth);
			glVertex3i(-halfWidth, -halfHeight, halfWidth);

			// back face
			glVertex3i(halfWidth, halfHeight, -halfWidth);
			glVertex3i(-halfWidth, halfHeight, -halfWidth);
			glVertex3i(-halfWidth, -halfHeight, -halfWidth);
			glVertex3i(halfWidth, -halfHeight, -halfWidth);

			// right face
			glVertex3i(halfWidth, halfHeight, -halfWidth);
			glVertex3i(halfWidth, halfHeight, halfWidth);
			glVertex3i(halfWidth, -halfHeight, halfWidth);
			glVertex3i(halfWidth, -halfHeight, -halfWidth);

			// top face
			glVertex3i(halfWidth, halfHeight, halfWidth);
			glVertex3i(-halfWidth, halfHeight, halfWidth);
			glVertex3i(-halfWidth, halfHeight, -halfWidth);
			glVertex3i(halfWidth, halfHeight, -halfWidth);

			// bottom face
			glVertex3i(halfWidth, -halfHeight, halfWidth);
			glVertex3i(-halfWidth, -halfHeight, halfWidth);
			glVertex3i(-halfWidth, -halfHeight, -halfWidth);
			glVertex3i(halfWidth, -halfHeight, -halfWidth);
		glEnd();
	glPopMatrix();
}