#include <iostream>
#include <SDL.h>

#include "gui/states/MenuMain.h"
#include "gui/Menu2DItem.h"
#include "gui/StateMachine.h"
#include "gui/GuiWindow.h"

#include "gui/states/GamePlay.h"
#include "gui/states/MenuLoadGame.h"
#include "gui/states/MenuOptions.h"

using namespace std;

MenuMain::MenuMain() : fsm(StateMachine::getInstance()) {
	m_nextState = States::KEEP_CURRENT;
}

void MenuMain::enter() {
	std::cout << "entered MenuMain menu! " << std::endl;
	
	// switch to 2D mode
	fsm.window->set2DMode();

	// set background color
	glClearColor(0.6, 0.21, 0, 0.0);

	menu = make_shared<Menu2D>(fsm.window->getWidth(), fsm.window->getHeight());
	menu->addButton("NewGame.png")->onClick(boost::bind(&MenuMain::onNewGame, this));
	menu->addButton("LoadGame.png")->onClick(boost::bind(&MenuMain::onLoadGame, this));
	menu->addButton("Options.png")->onClick(boost::bind(&MenuMain::onOptions, this));
	menu->addButton("ExitGame.png")->onClick(boost::bind(&MenuMain::onExitGame, this));

	// ===== init OpenGL =====
	glClearDepth(1.0f);

	glEnable(GL_DEPTH_TEST);									// activate depth testing with Z-buffer
	glDepthFunc(GL_LEQUAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);			// specify implementation-specific hints
	glEnable(GL_COLOR_MATERIAL);

	// ===== config the lights =====
	GLfloat lightpos[] = { 0.0f, 0.0f, 0.0f, 1.0f };			// light position
	glLightfv(GL_LIGHT0, GL_POSITION, lightpos);				// using first light (GL_LIGHT0)

	// ambient + diffuse + specular = illumination
	GLfloat light_ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat light_diffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	GLfloat light_specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

	// activate the lightning
	glEnable(GL_LIGHTING);										// enable general lighting in this OpenGL context
	glEnable(GL_LIGHT0);										// enable previously configured light (GL_LIGHT0)

	// smoothing the light
	glShadeModel(GL_SMOOTH);									// Shading models: GL_SMOOTH, GL_FLAT

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);			// enable transparency (alpha)
	glEnable(GL_BLEND);											// enable color mixing (http://wiki.delphigl.com/index.php/glBlendFunc)
}

AbstractState* MenuMain::run() {
	// on demand event handling
	if (fsm.eventmap.mouseMoved) {
		menu->mouseMoved(fsm.eventmap.mouseX, fsm.eventmap.mouseY);
	}

	if (fsm.eventmap.mouseDown) {
		menu->mousePressed();
	}

	if (fsm.eventmap.mouseUp) {
		menu->mouseReleased();
	}

	this->draw();

	AbstractState* nextState;
	switch (m_nextState) {
		case States::NEW_GAME:
			nextState = new GamePlay();
			break;
		case States::LOAD_GAME:
			nextState = new MenuLoadGame();
			break;
		case States::OPTIONS:
			nextState = new MenuOptions();
			break;
		case States::EXIT_GAME:
			nextState = nullptr;
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

void MenuMain::draw() {
	menu->draw();

	fsm.window->printHeadline("3D Schach");
	fsm.window->printSubHeadline(".:: Hauptmenue");
}

void MenuMain::onNewGame() {
	std::cout << "start new game!" << std::endl;
	m_nextState = States::NEW_GAME;
}

void MenuMain::onLoadGame() {
	std::cout << "load a game!" << std::endl;
	m_nextState = States::LOAD_GAME;
}

void MenuMain::onOptions() {
	std::cout << "show options" << std::endl;
	m_nextState = States::OPTIONS;
}

void MenuMain::onExitGame() {
	m_nextState = States::EXIT_GAME;
}

void MenuMain::exit() {
	std::cout << "left MenuMain menu!" << std::endl;
}