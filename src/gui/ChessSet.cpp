#include "ChessSet.h"
#include "AnimationHelper.h"

#include <algorithm>
#include <iostream>
#include <SDL.h>

using namespace std;

ChessSet::ChessSet()
	: m_tileWidth(24) 
	, m_tileHeight(2)
	, m_turnMoveShowDuration(2000)
	, m_turnMoveShownSince(0)
	, m_internalState(InternalState::STATIC)
	, m_animationElevationHeight(20.f)
	, m_animationElevationStrikeHeight(200.f)
	, m_firstRun(true) {

	// all external resources to load
	m_extResources = {
		"resources/3dmodels/king.3DS",
		"resources/3dmodels/queen.3DS",
		"resources/3dmodels/bishop.3DS",
		"resources/3dmodels/knight.3DS",
		"resources/3dmodels/rook.3DS",
		"resources/3dmodels/pawn.3DS"
	};

	m_extCorrectionValues = { {
        { 61, 12, 0, 1, -90, 0, 0 },	// king
        { 40, 12, 0, 1, -90, 0, 0 },	// queen
        { 1, 21, 0, 1, -90, 0, 0 },		// bishop
        { 0, 0, 0, 0.4, -90, 0, 0 },	// knight
        { 20, 12, 0, 1, -90, 0, 0 },	// rook
        { 0, 13, 0, 1, -90, 0, 0 }		// pawn
    } };

	m_animationHelperModelX = make_shared<AnimationHelper>(m_turnMoveShowDuration);
	m_animationHelperModelY = make_shared<AnimationHelper>(m_turnMoveShowDuration / 2);
	m_animationHelperModelZ = make_shared<AnimationHelper>(m_turnMoveShowDuration);
	m_animationHelperModelStrike = make_shared<AnimationHelper>(m_turnMoveShowDuration);
}

ChessSet::~ChessSet() {
	m_loadCallback.disconnect_all_slots();
}

int ChessSet::getResourcesCount() {
	return static_cast<int>(m_extResources.size()) + 1;	// +1 for the board
}

void ChessSet::loadResources() {
	// load models
	int i = 0;
	for (auto &resName : m_extResources) {
		// propagate what is loaded
		m_loadCallback(resName);

		// create the model
		m_models[i] = make_shared<Model>(resName);

		// set the correction values for the model
		auto& cor = m_extCorrectionValues[i];
		m_models[i]->setCorrectionValues(
            cor.x, cor.y, cor.z,
            cor.scale,
            cor.rotX, cor.rotY, cor.rotZ
        );

		// cache the model in a display list

		// white models
		m_models[i]->setColor(Model::Color::WHITE);
		m_modelList[i] = glGenLists(1);

		glNewList(m_modelList[i], GL_COMPILE);
			m_models[i]->draw();
		glEndList();

		// black models
		m_models[i]->setColor(Model::Color::BLACK);
		m_modelList[i+6] = glGenLists(1);

		glNewList(m_modelList[i+6], GL_COMPILE);
			m_models[i]->draw();
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

void ChessSet::setState(std::array<Piece, 64> state, PlayerColor lastPlayer, Turn lastTurn) {
	m_lastState = m_state;	// save old state
	m_state = state;
	m_lastTurn = lastTurn;

	if (!m_firstRun) {
		// gather all models who was striked in the last turn
		m_modelStrikes.clear();
		for (size_t i = 0; i < m_state.size(); i++) {
			if (m_state[i].type != m_lastState[i].type /* field is different as before */ && 
					m_lastState[i].type != PieceType::NoType /* lastState must had a model on the field */ &&
					static_cast<Field>(i) != m_lastTurn.from /* only those fields which are not the .from field of the turn */) {
				StrikedModel m = { m_lastState[i], static_cast<Field>(i) };
				m_modelStrikes.push_back(m);
			}
		}
	}

	m_firstRun = false;

	m_lastPlayer = lastPlayer;

	if (m_lastPlayer == PlayerColor::NoPlayer) {
		// only on first round
		m_internalState = InternalState::STATIC;
		createModelsList(false);
	} else {
		if (m_lastTurn.action == Turn::Action::Move || m_lastTurn.action == Turn::Action::Castle) {
			m_internalState = InternalState::ANIMATING;
			
			m_animationHelperModelX->reset();
			m_animationHelperModelY->reset();
			m_animationHelperModelZ->reset();
			m_animationHelperModelStrike->reset();

			m_animationDirectionY = Elevation::UP;

			createModelsList(true);
			m_turnMoveShownSince = SDL_GetTicks();
		}
	}
}

void ChessSet::draw() {
	drawModels();
	drawBoard();
}

void ChessSet::drawModels() {
	if (m_internalState == InternalState::STATIC) {
		glCallList(m_modelsList);
	}

	if (m_internalState == InternalState::ANIMATING) {
		bool timeIsUp = (SDL_GetTicks() - m_turnMoveShownSince) > m_turnMoveShowDuration;

		if (!timeIsUp) {
			animateModelTurn();
		} else {
			createModelsList(false);
			m_internalState = InternalState::STATIC;
		}

		glCallList(m_modelsList);
	}
}

void ChessSet::drawBoard() {
	glCallList(m_boardList);
}

/**
 * The chess board is sectioned in the following way.
 * The origin is in the center of the board.
 *
 *  x/z:
 *
 * -96/-96                   +96/-96
 *    +__ __ __ __   __ __ __ __+
 *    |__|__|__|__| |__|__|__|__|
 *    |__|__|__|__| |__|__|__|__|
 *    |__|__|__|__| |__|__|__|__|
 *    |__|__|__|__| |__|__|__|__|
 *     __ __ __ __0/0__ __ __ __ 
 *    |__|__|__|__| |__|__|__|__|
 *    |__|__|__|__| |__|__|__|__|
 *    |__|__|__|__| |__|__|__|__|
 *    |__|__|__|__| |__|__|__|__|
 *    +                         +
 *  -96/+96                  +96/+96
 *
 */
ChessSet::Coord3D ChessSet::calcCoordinatesForTileAt(Field which) {
	Coord3D coord;

	int col = static_cast<int>(which) % 8;
	int row = 7 - static_cast<int>(which) / 8;

	coord.x = static_cast<float>(((col - 4) * m_tileWidth) + (m_tileWidth / 2));
	coord.y = 0.f;
	coord.z = static_cast<float>(((row - 4) * m_tileWidth) + (m_tileWidth / 2));

	return coord;
}

void ChessSet::drawActionTileAt(Field which, TileStyle style) {
	Coord3D coord = calcCoordinatesForTileAt(which);
	drawTile(coord, false, style);
}

// create the list only the first time
// to update single model positions
void ChessSet::createModelsList(bool withoutTurnDependentModels) {
	m_animationCapsules.clear();

	// draw all models without the model on position .from and .to
	// these models will be drawn in a separate method with animation
	// -> fade out .to model
	// -> move animation .from model
	m_modelsList = glGenLists(1);
	glNewList(m_modelsList, GL_COMPILE);
		int field = 0;
		for (auto p : m_state) {
			if (p.type != PieceType::NoType) {
				glPushMatrix();
                    if ((p.type != PieceType::NoType) &&
                            (!withoutTurnDependentModels /* all */ ||
                            (withoutTurnDependentModels && field != m_lastTurn.from && field != m_lastTurn.to) /* all but without destination and target field */)) {
						// move model to tile
						drawModelAt(static_cast<Field>(field), p.type, p.player);
					} else {
						AnimationCapsule ac;
						ac.field = static_cast<Field>(field);
						ac.piece = p;
						ac.turn = m_lastTurn;
						m_animationCapsules.push_back(ac);
					}
				glPopMatrix();
			}
			++field;
		}
	glEndList();
}

void ChessSet::drawModelAt(Field field, PieceType type, PlayerColor color) {
	Coord3D coords = calcCoordinatesForTileAt(field);
	glTranslatef(coords.x, coords.y, coords.z);

	// draw model via list index
	int listIndex = type + (color == PlayerColor::Black ? 6 : 0);
	glCallList(m_modelList[listIndex]);
}

void ChessSet::drawModelAt(Coord3D coords, PieceType type, PlayerColor color) {
	glTranslatef(coords.x, coords.y, coords.z);

	// draw model via list index
	int listIndex = type + (color == PlayerColor::Black ? 6 : 0);
	glCallList(m_modelList[listIndex]);
}

// will only be called if we're in the animation state
void ChessSet::animateModelTurn() {
	for (auto& ac : m_animationCapsules) {
		// strikes
		for (auto& sm : m_modelStrikes) {
			Coord3D coords = calcCoordinatesForTileAt(sm.field);
			animateModelStrike(coords, sm.piece);
		}

		// turned model
		Coord3D coords = calcCoordinatesForTileAt(ac.turn.from);
		animateModelTurn(coords, ac);
	}
}

void ChessSet::animateModelStrike(Coord3D coords, Piece piece) {
	m_animationHelperModelStrike->setStartNowOrKeepIt();

	coords.y = (coords.y + 0.f) + m_animationHelperModelStrike->ease(AnimationHelper::EASE_OUTSINE, 0.f, m_animationElevationStrikeHeight); // UP

	glPushMatrix();
		drawModelAt(coords, piece.type, piece.player);
	glPopMatrix();
}

void ChessSet::animateModelTurn(Coord3D coordsFrom, AnimationCapsule animCapsule) {
	// ============================ up and down ============================
	m_animationHelperModelY->setStartNowOrKeepIt();

	if (m_animationDirectionY == Elevation::UP && m_animationHelperModelY->hasStopped()) {
		// we reached the top -> move down now
		m_animationHelperModelY->reset();
		m_animationHelperModelY->setStartNowOrKeepIt();
		m_animationDirectionY = Elevation::DOWN;
	}

	if (m_animationDirectionY == Elevation::UP) {
		coordsFrom.y = (coordsFrom.y + 0.f) + m_animationHelperModelY->ease(AnimationHelper::EASE_OUTSINE, 0.f, m_animationElevationHeight); // UP
	}
	else {
		coordsFrom.y = (coordsFrom.y + m_animationElevationHeight) - m_animationHelperModelY->ease(AnimationHelper::EASE_OUTSINE, 0.f, m_animationElevationHeight);	// DOWN
	}


	Coord3D coordsTo = calcCoordinatesForTileAt(animCapsule.turn.to);
	// first normalize the range from -96 - +96 --> 0 - +192
	float normFactor = m_tileWidth * 4.f;

	// ============================ back and forth ============================
	float newFromZ = coordsFrom.z + normFactor;
	float newToZ = coordsTo.z + normFactor;
	float distZ = abs(newFromZ - newToZ);

	float currAnimationDistanceZ = m_animationHelperModelZ->ease(AnimationHelper::EASE_OUTSINE, 0.f, distZ);
	if (newFromZ > newToZ) {
		// forth to back
		coordsFrom.z -= currAnimationDistanceZ;
	} else {
		// back to forth
		coordsFrom.z += currAnimationDistanceZ;
	}

	// ============================ left and right ============================
	float newFromX = coordsFrom.x + normFactor;
	float newToX = coordsTo.x + normFactor;
	float distX = abs(newFromX - newToX);

	float currAnimationDistanceX = m_animationHelperModelX->ease(AnimationHelper::EASE_OUTSINE, 0.f, distX);
	if (newFromX > newToX) {
		// right to left
		coordsFrom.x -= currAnimationDistanceX;
	}
	else {
		// left to right
		coordsFrom.x += currAnimationDistanceX;
	}

	glPushMatrix();
		drawModelAt(coordsFrom, animCapsule.piece.type, animCapsule.piece.player);
	glPopMatrix();
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
				
				Coord3D coords = { static_cast<float>(x), static_cast<float>(y), static_cast<float>(z) };
				drawTile(coords, oddToggler, TileStyle::NORMAL);
				oddToggler = !oddToggler;
			}

			oddToggler = !oddToggler;
		}
	glEndList();
}

void ChessSet::drawTile(Coord3D coords, bool odd, TileStyle style) {
	float halfWidth_t = m_tileWidth / 2.0f;
	float halfHeight_t = m_tileHeight / 2.0f;
	float halfWidth, halfHeight;

	glPushMatrix();
	glTranslatef(coords.x, coords.y, coords.z);
		
		glBegin(GL_QUADS);
			GLfloat emission[] = { 0.0f, 0.0f, 0.0f, 1.0f };		// example: glowing clock hand (Uhrzeiger) of an alarm clock at night -> we dont need this here
			glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, emission);
			
			GLfloat ambient[] = { 0.0f, 0.0f, 0.0f, 0.5f };			// example: this light scattered so often, that it comes from no particular direction but 
																	//          is uniformly distributed in the environment. If you specify no lighting in OpenGL,
																	//          the result is the same as if you define only ambient light.
			halfWidth = 0.95f * halfWidth_t;
			halfHeight = 1.05f * halfHeight_t;

			switch (style) {
				case NORMAL:
					halfWidth = halfWidth_t;
					halfHeight = halfHeight_t;

					if (!odd) {
						ambient[0] = 1.0f;
						ambient[1] = 1.0f;
						ambient[2] = 1.0f;
					} else {
						ambient[0] = 0.14f;
						ambient[1] = 0.07f;
						ambient[2] = 0.0f;
					}
					break;
				case CURSOR:
					ambient[0] = 0.8f;
					ambient[1] = 0.0f;
					ambient[2] = 0.0f;
					break;
				case MOVE:
					ambient[0] = 0.8f;
					ambient[1] = 0.6f;
					ambient[2] = 0.0f;
					break;
				case CASTLE:
					ambient[0] = 0.2f;
					ambient[1] = 0.6f;
					ambient[2] = 1.0f;
					break;
				default:
					break;
			}

			glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambient);

			GLfloat diffuse[] = { 0.2f, 0.2f, 0.2f, 1.0f };			// example: this light comes from a certain direction but is reflected homogenously from each point of the surface.
			glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffuse);

			GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };		// example: highlight point
			glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);

			GLfloat shininess[] = { 100 };
			glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, shininess);

			// front face
			glNormal3f(0.0f, 0.0f, -1.0f);
			glVertex3f(halfWidth, halfHeight, halfWidth);	// top right
			glVertex3f(-halfWidth, halfHeight, halfWidth);	// top left
			glVertex3f(-halfWidth, -halfHeight, halfWidth);	// bottom left
			glVertex3f(halfWidth, -halfHeight, halfWidth);	// bottom right

			// left face
			glNormal3f(-1.0f, 0.0f, 0.0f);
			glVertex3f(-halfWidth, halfHeight, halfWidth);
			glVertex3f(-halfWidth, halfHeight, -halfWidth);
			glVertex3f(-halfWidth, -halfHeight, -halfWidth);
			glVertex3f(-halfWidth, -halfHeight, halfWidth);

			// back face
			glNormal3f(0.0f, 0.0f, 1.0f);
			glVertex3f(halfWidth, halfHeight, -halfWidth);
			glVertex3f(-halfWidth, halfHeight, -halfWidth);
			glVertex3f(-halfWidth, -halfHeight, -halfWidth);
			glVertex3f(halfWidth, -halfHeight, -halfWidth);

			// right face
			glNormal3f(1.0f, 0.0f, 0.0f);
			glVertex3f(halfWidth, halfHeight, -halfWidth);
			glVertex3f(halfWidth, halfHeight, halfWidth);
			glVertex3f(halfWidth, -halfHeight, halfWidth);
			glVertex3f(halfWidth, -halfHeight, -halfWidth);

			// bottom face
			glNormal3f(0.0f, -1.0f, 0.0f);
			glVertex3f(halfWidth, -halfHeight, halfWidth);
			glVertex3f(-halfWidth, -halfHeight, halfWidth);
			glVertex3f(-halfWidth, -halfHeight, -halfWidth);
			glVertex3f(halfWidth, -halfHeight, -halfWidth);

			// top face
			glNormal3f(0.0f, 1.0f, 0.0f);
			glVertex3f(halfWidth, halfHeight, halfWidth);
			glVertex3f(-halfWidth, halfHeight, halfWidth);
			glVertex3f(-halfWidth, halfHeight, -halfWidth);
			glVertex3f(halfWidth, halfHeight, -halfWidth);
		glEnd();
	glPopMatrix();
}