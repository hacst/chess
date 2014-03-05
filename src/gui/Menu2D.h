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

/**
 * @brief 2D Menu to create a simple mouse-interacted menu.
 */
class Menu2D {
public:
    /**
     * @brief Creates a new 2D menu.
     * @param windowWidth The width of the window.
     * @param windowHeight The height of the window.
     */
    Menu2D(int windowWidth, int windowHeight);

    virtual ~Menu2D();

    /**
     * @brief Add a new button to the Menu.
     * @param filename The name of the button without the prefixes:
     *  - bt_a (active button state)
     *  - bt_h (hover button state) 
     *  - bt_n (normal button state)
     * @code
     *   m_pauseMenuMain = make_shared<Menu2D>(m_fsm.window->getWidth(), m_fsm.window->getHeight());
     *   m_pauseMenuMain->addButton("ResumeGame.png")->onClick(boost::bind(&GamePlay::onResumeGame, this));
     * @encdode
     */
    Menu2DItemPtr addButton(std::string filename);

    /**
     * @brief Draws the menu.
     */
    void draw();

    /**
     * @brief Tells the menu where the mouse is moved.
     * @note Call this every time the mouse coordinates changed.
     * @param x The x coordinate in the viewport.
     * @param y The y coordinate in the viewport.
     */
    void mouseMoved(const int x, const int y);

    /**
     * @brief Tells the menu that the first mouse key is pressed.
     * @note Call this every time the mouse is pressed. Make sure that there
     * was at least one mouseMoved(x, y) call to tell the menu the location
     * of where the mouse key is pressed.
     */
    void mousePressed();

    /**
     * @brief Tells the menu that the mouse key is relased.
     * @note Call this every time the mouse is released. Make sure that there
     * was at least one mouseMoved(x, y) call to tell the menu the location
     * of where the mouse key is released.
     */
    void mouseReleased();

    /**
     * @brief If the window is resized, the menu must adjust the position again.
     * @param newWith The new width of the window.
     * @param newHeight The new height of the window.
     */
    void windowResized(int newWidth, int newHeight);

    /**
     * @brief Resets the background-fading-animation.
     * @note Call this every time this menu is left for another
     * game context.
     */
    void resetAnimation();

private:
    //! All items/buttons of this menu.
    std::vector<Menu2DItemPtr> items;

    //! The width of the button
    const int m_buttonWidth = 200;
    //! The height of the button
    const int m_buttonHeight = 50;
    //! The margin (outer space) of the button
    const int m_buttonMargin = 10;
    //! The background-fading-animation duration
    const int m_animationDuration = 1500;

    //! The window width
    int m_windowWidth;
    //! The window height
    int m_windowHeight;
    //! The button count
    int m_btCount;
    //! The total height of the menu including all buttons and margin
    int m_height;
    //! The viewport distance to the left edge
    int m_marginLeft;
    //! The viewport distance to the top edge
    int m_marginTop;

    //! Internal mouse state
    struct mouseState {
        int x;
        int y;
        bool pressed;
    } mouseState;

    //! Active button of the menu
    Menu2DItemPtr activeButton;
    //! AnimationHelper for the background-fading.
    AnimationHelperPtr animationHelper;

    /**
     * @brief Updates the absolute position within the viewport after
     * the window changes or new buttons are added.
     */
    void updateAbsolutePosition();
};

using Menu2DPtr = std::shared_ptr<Menu2D>;

#endif // MENU2D_H
