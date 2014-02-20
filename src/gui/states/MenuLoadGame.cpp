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
#include <SDL.h>

#include "gui/states/MenuLoadGame.h"
#include "gui/Menu2DItem.h"
#include "gui/StateMachine.h"
#include "gui/GuiWindow.h"
#include "gui/SaveGame.h"

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
    menu->addButton("LoadGameSlot1.png")->onClick([&]() {
        m_nextState = States::LOAD_SLOT_A;
    });
    menu->addButton("LoadGameSlot2.png")->onClick([&]() {
        m_nextState = States::LOAD_SLOT_B;
    });
    menu->addButton("LoadGameSlot3.png")->onClick([&]() {
        m_nextState = States::LOAD_SLOT_C;
    });
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
    case States::LOAD_SLOT_B:
    case States::LOAD_SLOT_C: {
        int slot = m_nextState - States::LOAD_SLOT_A;
        auto saveGame = SaveGame::loadFromSlot(slot);
        if (!saveGame) {
            // Failed to load
            nextState = this;
            break;
        }
        
        nextState = new GamePlay(
            saveGame->gameMode,
            saveGame->humanPlayerColor,
            saveGame->fen);

        break;
    }
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