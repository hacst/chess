#include <iostream>
#include <SDL.h>

// ObsDisProx include Must be first to be before all windows.h
// includes in other headers to satisfy the angry gods
#include "logic/threading/ObserverDispatcherProxy.h" 

#include "gui/states/GamePlay.h"
#include "gui/GuiObserver.h"
#include "gui/Menu2DItem.h"
#include "gui/StateMachine.h"
#include "gui/AssimpHelper.h"
#include "gui/AnimationHelper.h"
#include "gui/ObjectHelper.h"
#include "gui/GuiWindow.h"

using namespace std;

GamePlay::GamePlay() : fsm(StateMachine::getInstance()) {
	m_nextState = States::KEEP_CURRENT;
}

void GamePlay::enter() {
	// set background color to black
	glClearColor(0.0, 0.0, 0, 1);

	// create a whole new ChessSet (2x 6 models + board)
	createChessSet();

	// @TODO: Menu must register at window, so that window can call menu if window size has changed
	m_pauseMenu = make_shared<Menu2D>(fsm.window->getWidth(), fsm.window->getHeight());
	m_pauseMenu->addButton("ResumeGame.png")->onClick(boost::bind(&GamePlay::onResumeGame, this));
	m_pauseMenu->addButton("SaveGame.png")->onClick(boost::bind(&GamePlay::onSaveGame, this));
	m_pauseMenu->addButton("BackToMainMenu.png")->onClick(boost::bind(&GamePlay::onLeaveGame, this));

	// create a new AnimationHelper for camera movement
	m_animationHelperCamera = make_shared<AnimationHelper>(1000);
	m_animationHelperBackground = make_shared<AnimationHelper>(1000);

	// light positions
	m_lightpos0[0] = 0.0f;		// right back
	m_lightpos0[1] = 300.0f;
	m_lightpos0[2] = 0.0f;
	m_lightpos0[3] = 1.0f;		// 0.0: directional light (light the sun), (x, y, z) specify the the direction of the light
								// 1.0: positional light (like a desk lamp), (x, y, z) specify the location of the light

	m_rotateFrom = 0;
	m_rotateTo = 180;

	// connection gui with ai and logic
	m_firstPlayer = make_shared<AIPlayer>();
	m_firstPlayer->start();
	m_secondPlayer = make_shared<AIPlayer>();
	m_secondPlayer->start();

	m_observer = make_shared<GuiObserver>(m_chessSet, *this);

	GameConfigurationPtr config = make_shared<GameConfiguration>();
	config->timeBetweenTurnsInSeconds = 3;
	m_gameLogic = make_shared<GameLogic>(m_firstPlayer, m_secondPlayer, config);
	m_gameLogic->addObserver(m_observer);
	m_observerProxy = make_shared<ObserverDispatcherProxy>(m_observer);
	m_gameLogic->addObserver(m_observerProxy);

	m_gameLogic->start();
	
	m_internalState = NOT_PAUSED;

	// debug
	angle[0] = 45.0;
	exponent[0] = 0;
}

void GamePlay::createChessSet() {
	m_chessSet = make_shared<ChessSet>();

	m_resourcesLoaded = 0;
	m_resourcesTotal = m_chessSet->getResourcesCount();

	m_chessSet->registerLoadCallback(boost::bind(&GamePlay::onBeforeLoadNextResource, this, _1));
	m_chessSet->loadResources();
}

void GamePlay::onBeforeLoadNextResource(string resourceName) {
	// swap the frame buffer for the first time to
	if (m_resourcesLoaded == 0) {
		fsm.window->swapFrameBufferNow();
	}

	// print what is loaded (resource name + progress bar)
	++m_resourcesLoaded;

	int windowWidth = fsm.window->getWidth();
	float percentLoaded = m_resourcesLoaded / static_cast<float>(m_resourcesTotal);

	array<float, 2> topLeftVertex		= { 0.0f, 0.0f	};
	array<float, 2> bottomLeftVertex	= { 0.0f, 10.0f };
	array<float, 2> bottomRightVertex	= { windowWidth * percentLoaded, 10.0f };
	array<float, 2> topRightVertex		= { windowWidth * percentLoaded, 0.0f };

	fsm.window->set2DMode();

	glPushMatrix();
		glBegin(GL_QUADS);
			// left side is grey
			glColor3f(0.6f, 0.6f, 0.6f);
			glVertex3f(topLeftVertex[0], topLeftVertex[1], -0.1f);
			glVertex3f(bottomLeftVertex[0], bottomLeftVertex[1], -0.1f);

			// right side is white
            glColor3f(1.0f, 1.0f, 1.0f);
			glVertex3f(bottomRightVertex[0], bottomRightVertex[1], -0.1f);
			glVertex3f(topRightVertex[0], topRightVertex[1], -0.1f);
		glEnd();
	glPopMatrix();

	fsm.window->printText(10, fsm.window->getHeight() - 30, 1.0, 1.0, 1.0, resourceName + " (" + to_string(m_resourcesLoaded) + " of " + to_string(m_resourcesTotal) + ")");

	// we must now swap the frame buffer
	fsm.window->swapFrameBufferNow();
}

void GamePlay::onResumeGame() {
	m_internalState = NOT_PAUSED;
}

void GamePlay::onSaveGame() {

}

void GamePlay::onLeaveGame() {

}

AbstractState* GamePlay::run() {
	// on demand event handling
	if (fsm.eventmap.keyUp) {
	}

	if (fsm.eventmap.keyRight) {
	}

	if (fsm.eventmap.keyDown) {
	}

	if (fsm.eventmap.keyLeft) {
	}

	if (fsm.eventmap.keyEscape) {
		m_internalState = PAUSED;
	}
	
	// Execute all pending calls from the observer
	m_observerProxy->poll();

	this->draw();

	AbstractState* nextState;
	switch (m_nextState) {
	case BACK_TO_MENU:
		nextState = new MenuMain();
		break;
	case KEEP_CURRENT:
		nextState = this;
		break;
	default:
		nextState = this;
		break;
	}

	return nextState;
}

// this light source has an effect like a desk lamp and is in the middle of the chessboard, the lighting direction is downwards
void GamePlay::setLights() {
	// init OpenGL lighting (after creating ChessSet) to not destroy the lighting used there
	glClearDepth(1.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_COLOR_MATERIAL);

	// config the single lights
	glEnable(GL_LIGHTING);

	// LIGHT 0 (ambient + diffuse + specular = phong)
	ambientLight[0] = 0.1f;
	ambientLight[1] = 0.1f;
	ambientLight[2] = 0.1f;
	ambientLight[3] = 0.1f;

	// a warm lighting source
	diffuseLight[0] = 0.2f;
	diffuseLight[1] = 0.2f;
	diffuseLight[2] = 0.2f;
	diffuseLight[3] = 1.0f;

	specularLight[0] = 1.0f;
	specularLight[1] = 1.0f;
	specularLight[2] = 1.0f;
	specularLight[3] = 1.0f;

	// LIGHTS
	GLfloat lightDir0[] = { 0.0, -1.0, 0.0 };
	GLfloat angle[] = { 45.0 };	// looks nice with 39.0
	GLfloat exponent[] = { 5.0 };	// looks nice with 9.0

	glLightfv(GL_LIGHT0, GL_POSITION, m_lightpos0);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lightDir0);
	glLightfv(GL_LIGHT0, GL_SPOT_EXPONENT, exponent);
	glLightfv(GL_LIGHT0, GL_SPOT_CUTOFF, angle);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);

	glEnable(GL_LIGHT0);

	// smoothing the light
	glShadeModel(GL_SMOOTH);
}

void GamePlay::draw() {
	fadeBackgroundForOneTime();

	// 3D
	fsm.window->set3DMode();

	setLights();	// this MUST be in 3D Mode -> The light's position is transformed by the modelview matrix that was active at the moment the light was defined.

	// chessboard and models
	m_chessSet->draw();

	// trigger camera rotation
	rotateCamera();

	// draw menu if game is paused
	if (m_internalState == PAUSED) {
		glEnable(GL_COLOR_MATERIAL);

		// 2D
		fsm.window->set2DMode();

		fsm.window->printHeadline("II P A U S E");
		m_pauseMenu->draw();

		glDisable(GL_COLOR_MATERIAL);
	}
}

void GamePlay::drawCoordinateSystem() {
	glPushMatrix();
		glLineWidth(5.0f);
		glBegin(GL_LINES);
			// x
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3f(-30.0f, 0.0f, 0.0f);
			glVertex3f(30.0f, 0.0f, 0.0f);

			// y
			glColor3f(0.0f, 1.0f, 0.0f);
			glVertex3f(0.0f, -30.0f, 0.0f);
			glVertex3f(0.0f, 30.0f, 0.0f);

			// z
			glColor3f(0.0f, 0.0f, 1.0f);
			glVertex3f(0.0f, 0.0f, -30.0f);
			glVertex3f(0.0f, 0.0f, 30.0f);
		glEnd();

		glTranslatef(0, 5, 0);
	glPopMatrix();
}

void GamePlay::fadeBackgroundForOneTime() {
	m_animationHelperBackground->setStartNowOrKeepIt();

	// set background color to cool lime and fade in
	glClearColor(
		m_animationHelperBackground->ease(AnimationHelper::EASE_LINEAR, 0.0f, 0.47f),
		m_animationHelperBackground->ease(AnimationHelper::EASE_LINEAR, 0.0f, 0.64f),
		0.0,
		1.0
	);
}

void GamePlay::rotateCamera() {
	m_animationHelperCamera->setStartNowOrKeepIt();

	if (m_animationHelperCamera->hasStopped()) {
		return;
	}

	float angleDegree = m_animationHelperCamera->ease(AnimationHelper::EASE_OUTSINE, static_cast<float>(m_rotateFrom), static_cast<float>(m_rotateTo));
	float angleRadian = angleDegree * ((float)M_PI / 180.0f);

	float newCameraX = sinf(angleRadian) * fsm.window->getCameraDistanceToOrigin();
	float newCameraZ = cosf(angleRadian) * fsm.window->getCameraDistanceToOrigin();
	float rotationY = (atan2f(newCameraX, -newCameraZ) * 180.0f / (float)M_PI) - 180.0f;

	fsm.window->m_cameraAngleY = rotationY;
	fsm.window->m_cX = newCameraX;
	fsm.window->m_cZ = newCameraZ;
}

void GamePlay::startCameraRotation() {
	m_animationHelperCamera->reset();

	// also set the coordinates to the opposite
	m_rotateFrom = (m_rotateFrom + 180) % 360;
	m_rotateTo = (m_rotateFrom + 180) % 360;
}

void GamePlay::onBackToMenu() {
	std::cout << "go back to menu" << std::endl;

	m_nextState = BACK_TO_MENU;
}

void GamePlay::exit() {
	std::cout << "left GamePlay!" << std::endl;
}
