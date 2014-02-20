/*
    Copyright (c) 2013-2014, Patrick Hillert <silent@gmx.biz>

    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

    3. Neither the name of the copyright holder nor the names of its
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.
*/

#include "gui/states/MenuGameMode.h"
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
        nextState = new MenuGameMode();
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
}