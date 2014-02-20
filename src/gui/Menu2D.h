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

#ifndef MENU2D_H
#define MENU2D_H

#include <iostream>
#include <array>
#include <string>

#include <vector>
#include <memory>
#include "boost/bind.hpp"

#include "gui/Menu2DItem.h"
#include "gui/AnimationHelper.h"

#ifdef _WIN32
#include <windows.h> // Needed for gl.h
#endif

#include <GL/gl.h>

class GuiWindow;

class Menu2D {
public:
    Menu2D(int windowWidth, int windowHeight);
    virtual ~Menu2D();

    Menu2DItemPtr addButton(std::string filename);

    void draw();
    void mouseMoved(const int x, const int y);
    void mousePressed();
    void mouseReleased();
    void windowResized(int newWidth, int newHeight);
    void resetAnimation();

private:

    std::vector<Menu2DItemPtr> items;

    // config
    const int m_buttonWidth = 200;
    const int m_buttonHeight = 50;
    const int m_buttonMargin = 10;
    const int m_animationDuration = 1500;

    int m_windowWidth;
    int m_windowHeight;
    int m_btCount;
    int m_height;						// menu height in total with margin
    int m_marginLeft;					// distance from left
    int m_marginTop;					// distance from top

    struct mouseState {
        int x;
        int y;
        bool pressed;
    } mouseState;

    Menu2DItemPtr activeButton;
    AnimationHelperPtr animationHelper;

    // methods
    void updateAbsolutePosition();
};

using Menu2DPtr = std::shared_ptr<Menu2D>;

#endif // MENU2D_H
