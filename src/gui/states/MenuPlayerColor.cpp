#include <iostream>
#include <SDL.h>

#include "gui/states/MenuPlayerColor.h"
#include "gui/states/GamePlay.h"
#include "gui/states/MenuGameMode.h"
#include "gui/Menu2DItem.h"
#include "gui/StateMachine.h"
#include "gui/GuiWindow.h"

using namespace std;

MenuPlayerColor::MenuPlayerColor() : m_fsm(StateMachine::getInstance()) {
	m_nextState = States::KEEP_CURRENT;
}

void MenuPlayerColor::enter() {
	// switch to 2D mode
	m_fsm.window->set2DMode();

	// set background color
	glClearColor(0.6f, 0.21f, 0, 0);

	m_menu = make_shared<Menu2D>(m_fsm.window->getWidth(), m_fsm.window->getHeight());
	m_menu->addButton("ColorWhite.png")->onClick(boost::bind(&MenuPlayerColor::onColorWhite, this));
	m_menu->addButton("ColorBlack.png")->onClick(boost::bind(&MenuPlayerColor::onColorBlack, this));
	m_menu->addButton("Back.png")->onClick(boost::bind(&MenuPlayerColor::onMenuBack, this));
}

AbstractState* MenuPlayerColor::run() {
	// on demand event handling
	if (m_fsm.eventmap.mouseMoved) {
		m_menu->mouseMoved(m_fsm.eventmap.mouseX, m_fsm.eventmap.mouseY);
	}

	if (m_fsm.eventmap.mouseDown) {
		m_menu->mousePressed();
	}

	if (m_fsm.eventmap.mouseUp) {
		m_menu->mouseReleased();
	}

	this->draw();

	AbstractState* nextState;
	switch (m_nextState) {
	case States::GAME_PLAY:
		nextState = new GamePlay(GamePlay::GameMode::PLAYER_VS_AI, m_colorWhite ? PlayerColor::White : PlayerColor::Black);
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
	m_menu->draw();

	m_fsm.window->printHeadline("3D Schach");
	m_fsm.window->printSubHeadline(".:: Welche Spielerfarbe?");
}

void MenuPlayerColor::onMenuBack() {
	m_nextState = States::MENU_GAME_MODE;
}

void MenuPlayerColor::onColorWhite() {
	m_colorWhite = true;
	m_nextState = States::GAME_PLAY;
}

void MenuPlayerColor::onColorBlack() {
	m_colorWhite = false;
	m_nextState = States::GAME_PLAY;
}

void MenuPlayerColor::exit() {
	// @todo
}