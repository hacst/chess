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

/**
 * @brief The class helps to handle the keyboard navigation with the arrow keys.
 */
class ArrowNavigationHandler {
public:
    /**
     * @brief Creates a new ArrowNavigationHandler object.
     * @param inverseNavigation If true left/right and up/down are changed to the opposite.
     */
    ArrowNavigationHandler(bool inverseNavigation);

    //! Enumeration for the arrow keys directions.
    enum ArrowKey {
        UP,
        RIGHT,
        DOWN,
        LEFT
    };

    /**
     * @brief This method must be called if an arrow key is pressed.
     * @param direction the ArrowKey direction (see above).
     */
    void onKey(ArrowKey direction);

    /**
     * @brief Returns the current cursor position.
     * @return The cursor position as Field.
     */
    Field getCursorPosition();

private:
    //! The Configuration
    struct Config {
        unsigned int throttleMilliseconds;
        bool inverseNavigation;
    } m_config;

    //! The current cursor position.
    unsigned int m_tileCursor;

    //! The last stroke-time for the direction keys.
    std::chrono::time_point<std::chrono::system_clock> m_timeStart[4], m_timeNow[4];

    //! Flag, if a direction key must be throttled.
    bool m_throttling[4];

    /**
     * @brief Checks the time between the current and last keystroke and sets the
     * throttle flag.
     * @param direction The arrow key direction to check the time.
     */
    void checkTimeBetweenKeyStrokes(ArrowKey direction);

    /**
     * @brief Calculates the new vertical position depending on the given step-width.
     * @param steps The step-count (8 or -8) for the vertical (up/down) move.
     */
    void moveCursorVertical(int steps);

    /**
     * @brief Calculates the new horizontal position depending on the given step-width.
     * @param steps The step-count (1 or -1) for the horizontal (left/right) move.
     */
    void moveCursorHorizontal(int steps);

    /**
     * @brief Moves the cursor up with respect to the configured inversion of the keys.
     */
    void moveCursorUp();

    /**
     * @brief Moves the cursor right with respect to the configured inversion of the keys.
     */
    void moveCursorRight();
    
    /**
     * @brief Moves the cursor down with respect to the configured inversion of the keys.
     */
    void moveCursorDown();

    /**
     * @brief Moves the cursor left with respect to the configured inversion of the keys.
     */
    void moveCursorLeft();
};

using ArrowNavigationHandlerPtr = std::shared_ptr<ArrowNavigationHandler>;

#endif // ARROWNAVIGATIONHANDLER_H