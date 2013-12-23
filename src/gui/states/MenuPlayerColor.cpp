#include <iostream>
#include <SDL.h>

#include "gui/states/MenuPlayerColor.h"
#include "gui/states/GamePlay.h"
#include "gui/states/MenuGameMode.h"
#include "gui/Menu2DItem.h"
#include "gui/StateMachine.h"
#include "gui/GuiWindow.h"

using namespace std;

MenuPlayerColor::MenuPlayerColor() : fsm(StateMachine::getInstance()) {
	m_nextState = States::KEEP_CURRENT;
}

void MenuPlayerColor::enter() {
	// switch to 2D mode
	fsm.window->set2DMode();

	// set background color
	glClearColor(0.6f, 0.21f, 0, 0);

	menu = make_shared<Menu2D>(fsm.window->getWidth(), fsm.window->getHeight());
	menu->addButton("ColorWhite.png")->onClick(boost::bind(&MenuPlayerColor::onColorWhite, this));
	menu->addButton("ColorBlack.png")->onClick(boost::bind(&MenuPlayerColor::onColorBlack, this));
	menu->addButton("Back.png")->onClick(boost::bind(&MenuPlayerColor::onMenuBack, this));
}

AbstractState* MenuPlayerColor::run() {
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
	case States::GAME_PLAY:
		nextState = this; // @todo
		break;
	case States::MENU_GAME_MODE:
		nextState = new MenuGameMode();
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

void MenuPlayerColor::draw() {
	menu->draw();

	fsm.window->printHeadline("3D Schach");
	fsm.window->printSubHeadline(".:: Welche Spielerfarbe?");
}

void MenuPlayerColor::onMenuBack() {
	m_nextState = States::MENU_GAME_MODE;
}

void MenuPlayerColor::onColorWhite() {
	m_nextState = States::GAME_PLAY;
}

void MenuPlayerColor::onColorBlack() {
	m_nextState = States::GAME_PLAY;
}

void MenuPlayerColor::exit() {
	// @todo
}