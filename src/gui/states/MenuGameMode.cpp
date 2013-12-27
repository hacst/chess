#include <iostream>
#include <SDL.h>

#include "gui/states/MenuGameMode.h"
#include "gui/states/MenuPlayerColor.h"
#include "gui/states/GamePlay.h"
#include "gui/Menu2DItem.h"
#include "gui/StateMachine.h"
#include "gui/GuiWindow.h"

using namespace std;

MenuGameMode::MenuGameMode() : fsm(StateMachine::getInstance()) {
	m_nextState = States::KEEP_CURRENT;
}

void MenuGameMode::enter() {
	// switch to 2D mode
	fsm.window->set2DMode();

	// set background color
	glClearColor(0.6f, 0.21f, 0, 0);

	menu = make_shared<Menu2D>(fsm.window->getWidth(), fsm.window->getHeight());
	menu->addButton("ModeKIVsKI.png")->onClick(boost::bind(&MenuGameMode::onModeAIVsAI, this));
	menu->addButton("ModePlayerVsKI.png")->onClick(boost::bind(&MenuGameMode::onModePlayerVsAI, this));
	menu->addButton("Back.png")->onClick(boost::bind(&MenuGameMode::onMenuBack, this));
}

AbstractState* MenuGameMode::run() {
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
		nextState = new GamePlay(GamePlay::GameMode::AI_VS_AI, PlayerColor::White);
		break;
	case States::MENU_PLAYER_COLOR:
		nextState = new MenuPlayerColor();
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

void MenuGameMode::draw() {
	menu->draw();

	fsm.window->printHeadline("3D Schach");
	fsm.window->printSubHeadline(".:: Welcher Spielmodus?");
}

void MenuGameMode::onMenuBack() {
	m_nextState = States::MENU_MAIN;
}

void MenuGameMode::onModeAIVsAI() {
	m_nextState = States::GAME_PLAY;
}

void MenuGameMode::onModePlayerVsAI() {
	m_nextState = States::MENU_PLAYER_COLOR;
}

void MenuGameMode::exit() {
}