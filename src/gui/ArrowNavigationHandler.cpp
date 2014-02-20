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

#include "gui/ArrowNavigationHandler.h"

ArrowNavigationHandler::ArrowNavigationHandler(bool inverseNavigation) {

    m_config.inverseNavigation = inverseNavigation;
    m_config.throttleMilliseconds = 200;

    if (m_config.inverseNavigation) {
        m_tileCursor = 59;
    }
    else {
        m_tileCursor = 4;
    }

    m_throttling[ArrowKey::UP] = false;
    m_throttling[ArrowKey::RIGHT] = false;
    m_throttling[ArrowKey::DOWN] = false;
    m_throttling[ArrowKey::LEFT] = false;

    m_timeStart[ArrowKey::UP] = std::chrono::system_clock::now();
    m_timeStart[ArrowKey::RIGHT] = std::chrono::system_clock::now();
    m_timeStart[ArrowKey::DOWN] = std::chrono::system_clock::now();
    m_timeStart[ArrowKey::LEFT] = std::chrono::system_clock::now();
}

void ArrowNavigationHandler::checkTimeBetweenKeyStrokes(ArrowKey direction) {
    m_throttling[direction] = false;

    m_timeNow[direction] = std::chrono::system_clock::now();
    if (std::chrono::duration_cast<std::chrono::milliseconds>(m_timeNow[direction] - m_timeStart[direction]).count() <= m_config.throttleMilliseconds) {
        m_throttling[direction] = true;
    }
}

void ArrowNavigationHandler::onKey(ArrowKey direction) {
    checkTimeBetweenKeyStrokes(direction);
    if (m_throttling[direction])
        return;

    m_timeStart[direction] = std::chrono::system_clock::now();

    switch (direction) {
    case ArrowKey::UP:
        moveCursorUp();
        break;
    case ArrowKey::RIGHT:
        moveCursorRight();
        break;
    case ArrowKey::DOWN:
        moveCursorDown();
        break;
    case ArrowKey::LEFT:
        moveCursorLeft();
        break;
    default: break;
    }
}

void ArrowNavigationHandler::moveCursorVertical(int steps) {
    int newTileCursor = m_tileCursor + steps;

    // check boundary before moving the cursor
    if (newTileCursor >= 0 && newTileCursor <= 63) {
        m_tileCursor = newTileCursor;
    }
}

void ArrowNavigationHandler::moveCursorHorizontal(int steps) {
    int newTileCursor = m_tileCursor + steps;

    // check boundary before moving the cursor
    if (steps == -1) {
        // left key pressed
        switch (newTileCursor) {
            // all right side fields
        case 63:
        case 55:
        case 47:
        case 39:
        case 31:
        case 23:
        case 15:
        case 7:
        case -1:
            /* border corssed -> do nothing - keep old value */
            break;
        default:
            m_tileCursor = newTileCursor;
            break;
        }
    }
    else {
        // right key pressed
        switch (newTileCursor) {
            // all left side fields
        case 64:
        case 56:
        case 48:
        case 40:
        case 32:
        case 24:
        case 16:
        case 8:
            /* border corssed -> do nothing - keep old value */
            break;
        default:
            m_tileCursor = newTileCursor;
            break;
        }
    }
}

void ArrowNavigationHandler::moveCursorUp() {
    if (m_config.inverseNavigation) {
        moveCursorVertical(-8);
    }
    else {
        moveCursorVertical(8);
    }
}

void ArrowNavigationHandler::moveCursorRight() {
    if (m_config.inverseNavigation) {
        moveCursorHorizontal(-1);
    }
    else {
        moveCursorHorizontal(1);
    }
}

void ArrowNavigationHandler::moveCursorDown() {
    if (m_config.inverseNavigation) {
        moveCursorVertical(8);
    }
    else {
        moveCursorVertical(-8);
    }
}

void ArrowNavigationHandler::moveCursorLeft() {
    if (m_config.inverseNavigation) {
        moveCursorHorizontal(1);
    }
    else {
        moveCursorHorizontal(-1);
    }
}

Field ArrowNavigationHandler::getCursorPosition() {
    return static_cast<Field>(m_tileCursor);
}