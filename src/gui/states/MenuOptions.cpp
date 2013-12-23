#include <iostream>
#include <SDL.h>

#include "gui/states/MenuOptions.h"
#include "gui/Menu2DItem.h"
#include "gui/StateMachine.h"
#include "gui/GuiWindow.h"

using namespace std;

MenuOptions::MenuOptions() : fsm(StateMachine::getInstance()) {
	m_nextState = States::KEEP_CURRENT;
}

void MenuOptions::enter() {
	// switch to 2D mode
	fsm.window->set2DMode();

	// set background color
	glClearColor(0.6f, 0.21f, 0, 0);

	menu = make_shared<Menu2D>(fsm.window->getWidth(), fsm.window->getHeight());
	menu->addButton("OptVideoMode.png")->onClick(boost::bind(&MenuOptions::onResolutionChange, this));
	menu->addButton("LoadGameBack.png")->onClick(boost::bind(&MenuOptions::onMenuBack, this));
}

AbstractState* MenuOptions::run() {
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

void MenuOptions::draw() {
	menu->draw();

	fsm.window->printHeadline("3D Schach");
	fsm.window->printSubHeadline(".:: Optionen");
}

void MenuOptions::onResolutionChange() {
	if (!fsm.window->isFullscreen()) {
		fsm.window->switchWindowMode(GuiWindow::WindowMode::FULLSCREEN);
	} else {
		fsm.window->switchWindowMode(GuiWindow::WindowMode::WINDOW);
	}

	// resolution has changed -> propagate to menu (only here, other menus get window size on init)
	menu->windowResized(fsm.window->getWidth(), fsm.window->getHeight());

	m_nextState = States::MENU_MAIN;
}

void MenuOptions::onMenuBack() {
	m_nextState = States::MENU_MAIN;
}

void MenuOptions::exit() {
	std::cout << "left MenuOptions menu!" << std::endl;
	menu.reset();
}