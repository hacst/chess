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
    menu->addButton("Back.png")->onClick(boost::bind(&MenuOptions::onMenuBack, this));
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
    }
    else {
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