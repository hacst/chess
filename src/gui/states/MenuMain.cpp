#include "gui/states/GamePlay.h"
#include "gui/states/MenuMain.h"

#include <iostream>
#include <SDL.h>

#include "gui/Menu2DItem.h"
#include "gui/StateMachine.h"
#include "gui/GuiWindow.h"

#include "gui/states/MenuLoadGame.h"
#include "gui/states/MenuOptions.h"

using namespace std;

MenuMain::MenuMain() : fsm(StateMachine::getInstance()) {
	m_nextState = States::KEEP_CURRENT;
}

void MenuMain::enter() {
	// switch to 2D mode
	fsm.window->set2DMode();

	// set background color to orange
	glClearColor(0.6f, 0.21f, 0, 0);

	// create the menu
	menu = make_shared<Menu2D>(fsm.window->getWidth(), fsm.window->getHeight());
	menu->addButton("NewGame.png")->onClick(boost::bind(&MenuMain::onNewGame, this));
	menu->addButton("LoadGame.png")->onClick(boost::bind(&MenuMain::onLoadGame, this));
	menu->addButton("Options.png")->onClick(boost::bind(&MenuMain::onOptions, this));
	menu->addButton("ExitGame.png")->onClick(boost::bind(&MenuMain::onExitGame, this));

	// init OpenGL
	glEnable(GL_COLOR_MATERIAL);
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
	m_nextState = States::NEW_GAME;
}

void MenuMain::onLoadGame() {
	m_nextState = States::LOAD_GAME;
}

void MenuMain::onOptions() {
	m_nextState = States::OPTIONS;
}

void MenuMain::onExitGame() {
	m_nextState = States::EXIT_GAME;
}

void MenuMain::exit() {
	glDisable(GL_COLOR_MATERIAL);
}