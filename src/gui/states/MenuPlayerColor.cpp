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

#include <iostream>

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
    
}