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

	// create a new AnimationHelper for camera movement
	animationHelper = make_shared<AnimationHelper>(1000);

	// init OpenGL lighting (after creating ChessSet) to not destroy the lighting used there
	glClearDepth(1.0f);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glEnable(GL_COLOR_MATERIAL);

	// config the single lights
	glEnable(GL_LIGHTING);

	// Create light components.
	ambientLight[0] = 0.0f;
	ambientLight[1] = 0.0f;
	ambientLight[2] = 0.0f;
	ambientLight[3] = 1.0f;

	diffuseLight[0] = 1.0f;
	diffuseLight[1] = 1.0f;
	diffuseLight[2] = 1.0f;
	diffuseLight[3] = 1.0f;

	specularLight[0] = 0.0f;
	specularLight[1] = 0.0f;
	specularLight[2] = 0.0f;
	specularLight[3] = 1.0f;

	glEnable(GL_LIGHT0);
	//glEnable(GL_LIGHT1);
	//glEnable(GL_LIGHT2);
	//glEnable(GL_LIGHT3);

	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);

	m_lightpos1[0] = 0.0f;
	m_lightpos1[1] = 120.0f;
	m_lightpos1[2] = 0.0f;
	m_lightpos1[3] = 1.0f;

	m_lightpos2[0] = 108.0f;
	m_lightpos2[1] = 100.0f;
	m_lightpos2[2] = 108.0f;
	m_lightpos2[3] = 1.0f;

	m_lightpos3[0] = -108.0f;
	m_lightpos3[1] = 100.0f;
	m_lightpos3[2] = -108.0f;
	m_lightpos3[3] = 1.0f;

	m_lightpos4[0] = -108.0f;
	m_lightpos4[1] = 100.0f;
	m_lightpos4[2] = 108.0f;
	m_lightpos4[3] = 1.0f;

	// moved light0 to draw

	glLightfv(GL_LIGHT1, GL_POSITION, m_lightpos2);
	glLightfv(GL_LIGHT2, GL_POSITION, m_lightpos3);
	glLightfv(GL_LIGHT3, GL_POSITION, m_lightpos4);

	// smoothing the light
	glShadeModel(GL_SMOOTH);

	// ===== create OGL lists =====
	m_cube1 = ObjectHelper::createCubeList(4, m_lightpos1[0], m_lightpos1[1], m_lightpos1[2]);
	m_cube2 = ObjectHelper::createCubeList(4, m_lightpos2[0], m_lightpos2[1], m_lightpos2[2]);
	m_cube3 = ObjectHelper::createCubeList(4, m_lightpos3[0], m_lightpos3[1], m_lightpos3[2]);
	m_cube4 = ObjectHelper::createCubeList(4, m_lightpos4[0], m_lightpos4[1], m_lightpos4[2]);

	m_rotateFrom = 0;
	m_rotateTo = 180;

	// debug
	debugText = "light 0 on";
	//cameraView = 0;

	// connection gui with ai and logic
	m_firstPlayer = make_shared<AIPlayer>();
	m_firstPlayer->start();
	m_secondPlayer = make_shared<AIPlayer>();
	m_secondPlayer->start();
	
	m_observer = make_shared<GuiObserver>(m_chessSet, *this);

	GameConfigurationPtr config = make_shared<GameConfiguration>();
	config->minimumTurnTimeInSeconds = 3;
    m_gameLogic = make_shared<GameLogic>(m_firstPlayer, m_secondPlayer, config);
	m_gameLogic->addObserver(m_observer);
	m_observerProxy = make_shared<ObserverDispatcherProxy>(m_observer);
	m_gameLogic->addObserver(m_observerProxy);

	m_gameLogic->start();
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

AbstractState* GamePlay::run() {
	// on demand event handling
	if (fsm.eventmap.keyUp) {
		glDisable(GL_LIGHT0);
	}

	if (fsm.eventmap.keyRight) {

	}

	if (fsm.eventmap.keyDown) {
		glEnable(GL_LIGHT0);
	}

	if (fsm.eventmap.keyLeft) {
	}

	if (fsm.eventmap.mouseMoved) {

	}

	if (fsm.eventmap.mouseDown) {

	}

	if (fsm.eventmap.mouseUp) {
		if (debugText == "light 0 on") {
			glDisable(GL_LIGHT0);
			debugText = "light 0 off";
		}
		else {
			glEnable(GL_LIGHT0);
			debugText = "light 0 on";
		}
	}

	if (fsm.eventmap.keyEscape) {
		onBackToMenu();
	}

	// Execute all pending calls from the observer
	m_observerProxy->poll();

	this->draw();

	AbstractState* nextState;
	switch (m_nextState) {
	case States::BACK_TO_MENU:
		nextState = new MenuMain();
		break;
	case States::KEEP_CURRENT:
		nextState = this;
		break;
	default:
		nextState = this;
		break;
	}

	return nextState;
}

void GamePlay::draw() {
	// 2D
	fsm.window->set2DMode();

	// 3D
	fsm.window->set3DMode();

	// chessboard and models
	m_chessSet->draw();

	rotateCamera();

	// visualize light position
	glCallList(m_cube1);
	glCallList(m_cube2);
	glCallList(m_cube3);
	glCallList(m_cube4);

	// lights
	GLfloat lightDir0[] = { 0.0, -1.0, 0.0 };
	GLfloat exponent[] = { 0.0 };
	GLfloat angle[] = { 180.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, m_lightpos1);
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, lightDir0);
	glLightfv(GL_LIGHT0, GL_SPOT_EXPONENT, exponent);
	glLightfv(GL_LIGHT0, GL_SPOT_CUTOFF, angle);

	// debug info
	fsm.window->printText(100, 100, 1, 0, 0, debugText);
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
	glPopMatrix();
}

void GamePlay::rotateCamera() {
	animationHelper->setStartNowOrKeepIt();

	if (animationHelper->hasStopped()) {
		return;
	}

	float angleDegree = animationHelper->easeOutSine(m_rotateFrom, m_rotateTo);
	float angleRadian = angleDegree * (M_PI / 180.0);

	float newCameraX = sinf(angleRadian) * fsm.window->getCameraDistanceToOrigin();
	float newCameraZ = cosf(angleRadian) * fsm.window->getCameraDistanceToOrigin();
	float rotationY = (atan2(newCameraX, -newCameraZ) * 180 / M_PI) - 180;

	fsm.window->m_cameraAngleY = rotationY;
	fsm.window->m_cX = newCameraX;
	fsm.window->m_cZ = newCameraZ;
}

void GamePlay::startCameraRotation() {
	animationHelper->reset();

	// also set the coordinates to the opposite
	m_rotateFrom = (m_rotateFrom + 180) % 360;
	m_rotateTo = (m_rotateFrom + 180) % 360;
}

void GamePlay::onBackToMenu() {
	std::cout << "go back to menu" << std::endl;

	m_nextState = States::BACK_TO_MENU;
}

void GamePlay::exit() {
	std::cout << "left GamePlay!" << std::endl;
}
