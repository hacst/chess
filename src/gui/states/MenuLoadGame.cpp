#include <iostream>
#include <SDL.h>

#include "gui/states/MenuLoadGame.h"
#include "gui/Menu2DItem.h"
#include "gui/StateMachine.h"
#include "gui/GuiWindow.h"

using namespace std;

MenuLoadGame::MenuLoadGame() : fsm(StateMachine::getInstance()) {
	m_nextState = States::KEEP_CURRENT;
}

void MenuLoadGame::enter() {
	// switch to 2D mode
	fsm.window->set2DMode();

	// set background color
	glClearColor(0.6f, 0.21f, 0, 0);

	menu = make_shared<Menu2D>(fsm.window->getWidth(), fsm.window->getHeight());
	menu->addButton("LoadGameSlot1.png");
	menu->addButton("LoadGameSlot2.png");
	menu->addButton("LoadGameSlot3.png");
	menu->addButton("Back.png")->onClick(boost::bind(&MenuLoadGame::onMenuBack, this));
}

AbstractState* MenuLoadGame::run() {
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
		case States::LOAD_SLOT_A:
			nextState = this;	// @todo
			break;
		case States::LOAD_SLOT_B:
			nextState = this;	// @todo
			break;
		case States::LOAD_SLOT_C:
			nextState = this;	// @todo
			break;
		case States::MENU_MAIN:
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

void MenuLoadGame::draw() {
	menu->draw();

	fsm.window->printHeadline("3D Schach");
	fsm.window->printSubHeadline(".:: Spiel laden");
}

void MenuLoadGame::onMenuBack() {
	m_nextState = States::MENU_MAIN;
}

void MenuLoadGame::exit() {
}