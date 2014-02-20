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

#ifndef ARROWNAVIGATIONHANDLER_H
#define ARROWNAVIGATIONHANDLER_H

#include <chrono>
#include <memory>
#include "logic/ChessTypes.h"

class ArrowNavigationHandler {
public:
    ArrowNavigationHandler(bool inverseNavigation);

    enum ArrowKey {
        UP,
        RIGHT,
        DOWN,
        LEFT
    };

    void onKey(ArrowKey direction);
    Field getCursorPosition();

private:
    struct Config {
        unsigned int throttleMilliseconds;
        bool inverseNavigation;
    } m_config;

    unsigned int m_tileCursor;
    std::chrono::time_point<std::chrono::system_clock> m_timeStart[4], m_timeNow[4];
    bool m_throttling[4];

    void checkTimeBetweenKeyStrokes(ArrowKey direction);
    void moveCursorVertical(int steps);
    void moveCursorHorizontal(int steps);
    void moveCursorUp();
    void moveCursorRight();
    void moveCursorDown();
    void moveCursorLeft();
};

using ArrowNavigationHandlerPtr = std::shared_ptr<ArrowNavigationHandler>;

#endif // ARROWNAVIGATIONHANDLER_H