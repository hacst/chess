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

#ifndef MENU2DITEM_H
#define MENU2DITEM_H

#include <string>
#include <memory>
#include <iostream>
#include <boost/signals2.hpp>

#ifdef _WIN32
#include <windows.h> // Needed for gl.h
#endif

#include <GL/gl.h>

#include "SOIL.h"
#include "gui/Menu2DItem.h"

/**
 * @brief This class describes a button of a menu. The button is a rectangle
 * with textures depending on one of three states (normal, hover, active). The
 * buttons can be used with the mouse cursor.
 */
class Menu2DItem {
public:
    virtual ~Menu2DItem();

    /**
     * @brief Creates a new menu button item.
     * @param filename The filename relative to the executable located in resources/bt_n<filename>.
     * @param width The width of the button.
     * @param height The height of the button.
     *
     * @note You must provide a texture for each state in the /resources folder
     * relative to the executable.
     *
     * For example:
     * - bt_nBack.png for the normal (no action, simple button) state.
     * - bt_aBack.png for the active (pressed) state.
     * - bt_hBack.png for the hover (mouse above the button) state.
     *
     * Provide the filename without the prefix _bt_n_, _bt_a_ and _bt_h_,
     * this is automatically added.
     */
    Menu2DItem(std::string filename, int width, int height);

    /**
     * @brief Sets the buttons position in viewport coordinates.
     * @param x The x coordinate.
     * @param y The y coordinate.
     * @note The origin of the viewport is the upper left corner.
     */
    void setPosition(int x, int y);

    /**
     * @brief Draws the button on the previously set position. This method
     * will also consider the button state.
     * @note To provide the correct button state, you must update the mouse
     * position. See the methods below.
     */
    void draw();

    /**
     * @brief Updates the mouse's pointer/cursor position.
     * @param x The mouse's x position.
     * @param x The mouse's y position.
     *
     * @note You must use this method only if the mouse is moved but the
     * mouse button is neither pressed nor released.
     */
    void mouseMoved(int x, int y);

    /**
     * @brief Sets the coordinates, where the mouse clicked in the viewport.
     * @param x The mouse's x position.
     * @param x The mouse's y position.
     *
     * @note You must use this method only if the mouse was clicked but the
     * mouse button is neither moved nor released.
     */
    void mousePressed(int x, int y);

    /**
     * @brief Sets the coordinates, where the mouse click was released in the viewport.
     * @param x The mouse's x position.
     * @param x The mouse's y position.
     *
     * @note You must use this method only if the mouse was released but the
     * mouse button is neither pressed nor the mouse is moved.
     */
    void mouseReleased(int x, int y);

    /**
     * @brief Add a function or method which should be called via boost signals
     * when the button is clicked. So the given method can do something.
     * @param slot The function/method to call when the button is clicked.
     */
    void onClick(const boost::function<void()>& slot);

    /**
     * @brief Remove all click signals.
     */
    void unClick();
private:
    //! The width of the button.
    int m_width;

    //! The height of the button
    int m_height;

    //! The filename (without prefix) for the three textures.
    std::string m_filename;

    //! The button's x position.
    int m_positionX;

    //! The button's y position.
    int m_positionY;

    //! The current mouse's x position.
    int m_mousePosX = 0;

    //! The current mouse's x position.
    int m_mousePosY = 0;

    //! The button's state textures.
    GLuint m_texture[3];

    //! Flag, if the button is hovered at this moment.
    bool m_hovered;

    //! Flag, if the button is activated at this moment.
    bool m_activated = false;

    /**
     * @brief Checks whether the mouse is currently above the button.
     * @return True if the mouse is above the button, false otherwise.
     */
    bool inBoundingBox();

    //! Boost signals2 to provide callback functionality.
    using Signal = boost::signals2::signal<void()>;
    Signal m_clicked;
};

using Menu2DItemPtr = std::shared_ptr<Menu2DItem>; // provide shared pointer

#endif // MENU2DITEM_H
