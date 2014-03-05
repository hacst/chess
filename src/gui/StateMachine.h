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

#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include "gui/interface/AbstractState.h"

#include <vector>
#include <iostream>

// Forward declaration of GuiWindow.
class GuiWindow;

using namespace std;

/**
 * @brief Class which manages the states.
 * @note This is a singleton, you can get only one instance of the StateMachine.
 * Don't forget to update the events if they occur.
 */
class StateMachine {
public:
    /**
     * @brief Gets an instance of the StateMachine.
     * @note This is a singleton. So you can only get one instance.
     * @return StateMachine& A reference to the StateMachine.
     */
    static StateMachine& getInstance() {
        static StateMachine instance;
        return instance;
    }

    //! Structure for holding user events.
    struct EventMap {
        // mouse events
        bool mouseMoved = false;
        int mouseX = 0;
        int mouseY = 0;
        bool mouseDown = false;
        bool mouseUp = false;

        // keyboard events
        bool keyLeft = false;
        bool keyRight = false;
        bool keyDown = false;
        bool keyUp = false;
        bool keyEscape = false;
        bool keyReturn = false;
        bool key0 = false;
        bool key1 = false;
        bool key2 = false;
        bool key3 = false;
        bool key4 = false;
        bool keyA = false;
        bool keyY = false;
        bool keyR = false;
    } eventmap;

    //! Holds the pointer to the GuiWindow object, to access gui related methods.
    GuiWindow* window;

    /**
     * @brief Sets the start state and setup the state.
     * @param startState The start state.
     */
    void setStartState(AbstractState* startState);

    /**
     * @brief Runs the current state.
     * @return The AbstractState pointer to the state which must be run() the next time.
     */
    AbstractState* run();

    /**
     * @brief Sets the next state which should be run.
     * @param state The next state.
     */
    void setNextState(AbstractState* state);

private:
    //! For singleton reasons no public constructor.
    StateMachine() {};								// The {} brackets are needed here.
    StateMachine(const StateMachine&);				// Don't implement
    StateMachine& operator=(const StateMachine&);	// Don't implement

    ~StateMachine() {};

    //! The current state.
    AbstractState* m_currentState;
};

#endif // STATEMACHINE_H