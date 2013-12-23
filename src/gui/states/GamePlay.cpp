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

#include "ai/AIPlayer.h"
#include "misc/DummyPlayer.h"

using namespace std;

GamePlay::GamePlay()
	: fsm(StateMachine::getInstance())
	, m_rotateFrom(0)
	, m_rotateTo(180)
	, m_nextState(States::KEEP_CURRENT) {
}

void GamePlay::initMessageBox() {
	m_messageBox.width = 600;
	m_messageBox.height = 40;
	m_messageBox.padding = 10;
	m_messageBox.text = "";
	m_messageBox.showDuration = 3000;	// this should be the same as timeBetweenTurnsInSeconds of the GameConfiguration

	// precalculate absolute position
	m_messageBox.windowPosX = (fsm.window->getWidth() / 2) - (m_messageBox.width / 2);
	m_messageBox.windowPosY = 10;

	// create rectangle OGL list for faster drawing
	fsm.window->set2DMode();

	m_messageBox.displayList = glGenLists(1);

	glNewList(m_messageBox.displayList, GL_COMPILE);
		glPushMatrix();
			glBegin(GL_QUADS);
				glColor3f(0.0f, 0.0f, 0.4f);
				glVertex3f(static_cast<float>(m_messageBox.windowPosX), static_cast<float>(m_messageBox.windowPosY), 0.0f); // top left
				glVertex3f(static_cast<float>(m_messageBox.windowPosX + m_messageBox.width), static_cast<float>(m_messageBox.windowPosY), 0.0f); // top right
			
				glColor3f(0.0f, 0.0f, 0.3f);
				glVertex3f(static_cast<float>(m_messageBox.windowPosX + m_messageBox.width), static_cast<float>(m_messageBox.windowPosY + m_messageBox.height), 0.0f); // bottom right
				glVertex3f(static_cast<float>(m_messageBox.windowPosX), static_cast<float>(m_messageBox.windowPosY + m_messageBox.height), 0.0f); // bottom left
			glEnd();
		glPopMatrix();
	glEndList();
}

void GamePlay::enter() {
	// set background color to black
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	m_internalState = NOT_PAUSED;

	initChessSet();
	initMenuPause();
	initAnimationHelpers();
	initLighting();
	initMessageBox();

	// connect gui with ai and logic
	initPlayers();
	initGameLogic();
}

void GamePlay::initMenuPause() {
	m_pauseMenu = make_shared<Menu2D>(fsm.window->getWidth(), fsm.window->getHeight());
	m_pauseMenu->addButton("ResumeGame.png")->onClick(boost::bind(&GamePlay::onResumeGame, this));
	m_pauseMenu->addButton("SaveGame.png")->onClick(boost::bind(&GamePlay::onSaveGame, this));
	m_pauseMenu->addButton("BackToMainMenu.png")->onClick(boost::bind(&GamePlay::onLeaveGame, this));
}

void GamePlay::initPlayers() {
	// AI vs. AI
	auto firstPlayer = make_shared<AIPlayer>();
	firstPlayer->start();
	m_firstPlayer = firstPlayer;

	auto secondPlayer = make_shared<DummyPlayer>();
	secondPlayer->start();
	m_secondPlayer = secondPlayer;
}

void GamePlay::initGameLogic() {
	m_observer = make_shared<GuiObserver>(m_chessSet, *this);

	GameConfigurationPtr config = make_shared<GameConfiguration>();
	config->timeBetweenTurnsInSeconds = 3;

	m_gameLogic = make_shared<GameLogic>(m_firstPlayer, m_secondPlayer, config);
	m_gameLogic->addObserver(m_observer);

	m_observerProxy = make_shared<ObserverDispatcherProxy>(m_observer);
	m_gameLogic->addObserver(m_observerProxy);

	m_gameLogic->start();
}

// create a whole new ChessSet (2x6 models + 1 board)
void GamePlay::initChessSet() {
	m_chessSet = make_shared<ChessSet>();

	m_resourcesLoaded = 0;
	m_resourcesTotal = m_chessSet->getResourcesCount();

	m_chessSet->registerLoadCallback(boost::bind(&GamePlay::onBeforeLoadNextResource, this, _1));
	m_chessSet->loadResources();
}

// creates new animation helpers for camera movement and background fading
void GamePlay::initAnimationHelpers() {
	m_animationHelperCamera = make_shared<AnimationHelper>(1000);
	m_animationHelperBackground = make_shared<AnimationHelper>(1000);
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

	fsm.window->set3DMode();
}

void GamePlay::onResumeGame() {
	m_internalState = NOT_PAUSED;
	m_pauseMenu->resetAnimation();
}

void GamePlay::onSaveGame() {
	m_internalState = SAVE_GAME;


}

void GamePlay::onLeaveGame() {
	m_nextState = BACK_TO_MENU;
}

AbstractState* GamePlay::run() {
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
void GamePlay::initLighting() {
	m_lightPos0[0] = 0.0f;
	m_lightPos0[1] = 65.0f;
	m_lightPos0[2] = -50.0f;

	m_lightPos1[0] = 0.0f;
	m_lightPos1[1] = 65.0f;
	m_lightPos1[2] = 50.0f;

	fsm.window->set3DMode();

	glShadeModel(GL_SMOOTH);
	glEnable(GL_NORMALIZE);

	// === global lighting model configuration ===
	GLfloat globalAmbientLight[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, globalAmbientLight);

	GLfloat mode = GLfloat(GL_TRUE);
	glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, &mode);
	
	// === local ligthing source configuration ===
	GLfloat ambient[] = { 1.0f, 0.94f, 0.68f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT2, GL_AMBIENT, ambient);

	GLfloat diffuse[] = { 0.5f, 0.5f, 0.5f, 1.0f };
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, diffuse);

	GLfloat specular[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT2, GL_SPECULAR, specular);

	glLightfv(GL_LIGHT0, GL_POSITION, m_lightPos0);
	glLightfv(GL_LIGHT1, GL_POSITION, m_lightPos1);

	GLfloat direction0[] = { 0.0, 0.0, -1.0 };
	glLightfv(GL_LIGHT0, GL_SPOT_DIRECTION, direction0);
	GLfloat direction1[] = { 0.0, 0.0, 1.0 };
	glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, direction1);

	GLfloat angle[] = { 180.0 };
	glLightfv(GL_LIGHT0, GL_SPOT_CUTOFF, angle);
	glLightfv(GL_LIGHT1, GL_SPOT_CUTOFF, angle);

	GLfloat exponent[] = { 1.0 };
	glLightfv(GL_LIGHT0, GL_SPOT_EXPONENT, exponent);
	glLightfv(GL_LIGHT1, GL_SPOT_EXPONENT, exponent);
}

void GamePlay::enableLighting() {
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
}

void GamePlay::disableLighting() {
	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHT1);
}

void GamePlay::draw() {
	enableLighting();

	fadeBackgroundForOneTime();

	// 3D
	fsm.window->set3DMode();
	m_chessSet->draw();	// chessboard and models
	rotateCamera();		// trigger camera rotation

	// 2D
	fsm.window->set2DMode();
	disableLighting();

	// draw menu if game is paused
	if (m_internalState == PAUSED) {
		drawPauseMenu();
	} else {
		// do not draw any text -> looks crappy
		drawMessageBox();
		drawLastTurns();
	}
}

void GamePlay::handleEvents() {
	if (fsm.eventmap.mouseMoved) {
		m_pauseMenu->mouseMoved(fsm.eventmap.mouseX, fsm.eventmap.mouseY);
	}

	if (fsm.eventmap.mouseDown) {
		m_pauseMenu->mousePressed();
	}

	if (fsm.eventmap.mouseUp) {
		m_pauseMenu->mouseReleased();
	}
}

// must be done in 2D mode
void GamePlay::drawMessageBox() {
	if (m_messageBox.text == "")
		return;
	
	// show only if the show time is not over
	if (m_messageBox.shownSince < (SDL_GetTicks() - m_messageBox.showDuration)) {
		m_messageBox.text = "";
		return;
	}

	glCallList(m_messageBox.displayList);

	fsm.window->printText(
		m_messageBox.windowPosX + m_messageBox.padding,
		m_messageBox.windowPosY + m_messageBox.padding,
		1.0f, 1.0f, 1.0f, 
		m_messageBox.text
	);
}

// must be done in 2D mode
void GamePlay::drawLastTurns() {
	// config
	int fontSize = fsm.window->fontSize::TEXT_SMALL;
	int numberOfTurnsToDraw = 5;
	
	// precalculations
	int lineHeight = fontSize;
	int totalLineHeight = numberOfTurnsToDraw * lineHeight;
	int offsetY = fsm.window->getHeight() - totalLineHeight;

	int step = 0;
	for (auto& turn : m_chessSet->getTurns()) {
		if (step == numberOfTurnsToDraw)
			return;

		int relativeOffsetY = step * lineHeight;
		fsm.window->printTextSmall(10, offsetY + relativeOffsetY, 1.0f, 1.0f, 1.0f, turn.toString());

		++step;
	}
}

void GamePlay::drawPauseMenu() {
	handleEvents();
	
	// modal dialog with transparent background
	glEnable(GL_COLOR);
	glEnable(GL_BLEND);
	glPushMatrix();
		glBegin(GL_QUADS);
			glColor4f(0.0f, 0.0f, 0.0f, 0.25f);

			glVertex3f(0.0f, 0.0f, 0.0f);
			glVertex3f(static_cast<float>(fsm.window->getWidth()), 0.0f, 0.0f);
			glVertex3f(static_cast<float>(fsm.window->getWidth()), static_cast<float>(fsm.window->getHeight()), 0.0f);
			glVertex3f(0.0f, static_cast<float>(fsm.window->getHeight()), 0.0f);
		glEnd();
	glPopMatrix();
	glDisable(GL_BLEND);
	glDisable(GL_COLOR);

	fsm.window->printHeadline("P A U S E");
	m_pauseMenu->draw();
}

void GamePlay::drawCoordinateSystem() {
	glPushMatrix();
		
		glLineWidth(5.0f);
		glBegin(GL_LINES);
			// x
			glColor3f(1.0f, 0.0f, 0.0f);
			glVertex3f(-120.0f, 0.0f, 0.0f);
			glVertex3f(120.0f, 0.0f, 0.0f);

			// y
			glColor3f(0.0f, 1.0f, 0.0f);
			glVertex3f(0.0f, -120.0f, 0.0f);
			glVertex3f(0.0f, 120.0f, 0.0f);

			// z
			glColor3f(0.0f, 0.0f, 1.0f);
			glVertex3f(0.0f, 0.0f, -120.0f);
			glVertex3f(0.0f, 0.0f, 120.0f);
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

void GamePlay::startShowText(std::string text) {
	m_messageBox.text = text;
	m_messageBox.shownSince = SDL_GetTicks();
}

void GamePlay::onBackToMenu() {
	std::cout << "go back to menu" << std::endl;

	m_nextState = BACK_TO_MENU;
}

void GamePlay::exit() {
	std::cout << "left GamePlay!" << std::endl;
}
