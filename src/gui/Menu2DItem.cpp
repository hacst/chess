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

#include "gui/Menu2DItem.h"

#include <iostream>
#include <sstream>

using namespace std;

Menu2DItem::Menu2DItem(std::string filename, int width, int height)
    : m_width(width)
    , m_height(height)
    , m_filename(filename) {

    // load normal state texture
    m_texture[0] = SOIL_load_OGL_texture(
        ("resources/bt_n" + m_filename).c_str(),
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_INVERT_Y
        );

    // load hover state texture
    m_texture[1] = SOIL_load_OGL_texture(
        ("resources/bt_h" + m_filename).c_str(),
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_INVERT_Y
        );

    // load active state texture
    m_texture[2] = SOIL_load_OGL_texture(
        ("resources/bt_a" + m_filename).c_str(),
        SOIL_LOAD_AUTO,
        SOIL_CREATE_NEW_ID,
        SOIL_FLAG_INVERT_Y
        );
}

Menu2DItem::~Menu2DItem() {
    m_clicked.disconnect_all_slots();
}

void Menu2DItem::setPosition(int x, int y) {
    m_positionX = x;
    m_positionY = y;
}

bool Menu2DItem::inBoundingBox() {
    bool inX = m_mousePosX >= m_positionX && m_mousePosX <= m_positionX + m_width;
    bool inY = m_mousePosY >= m_positionY && m_mousePosY <= m_positionY + m_height;

    return inX && inY;
}

void Menu2DItem::draw() {
    // add texture
    int state_index;
    if (m_hovered) {
        state_index = 1;
    }
    else if (m_activated) {
        state_index = 2;
    }
    else {
        state_index = 0;
    }

    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, m_texture[state_index]);

    // we could abstact the openGL binding with an abstract graphic class which implements OpenGL, DirectX, ...
    glColor4f(1.f, 1.f, 1.f, 1.f);
    glBegin(GL_QUADS);
    glTexCoord2i(0, 1); glVertex2i(m_positionX, m_positionY);
    glTexCoord2i(1, 1); glVertex2i(m_positionX + m_width, m_positionY);
    glTexCoord2i(1, 0); glVertex2i(m_positionX + m_width, m_positionY + m_height);
    glTexCoord2i(0, 0); glVertex2i(m_positionX, m_positionY + m_height);
    glEnd();

    glDisable(GL_TEXTURE_2D);
}

void Menu2DItem::mouseMoved(int x, int y) {
    m_mousePosX = x;
    m_mousePosY = y;

    if (inBoundingBox()) {
        m_hovered = true;
        m_activated = false;
    }
    else {
        m_hovered = false;
        m_activated = false;
    }
}

void Menu2DItem::mousePressed(int x, int y) {
    m_mousePosX = x;
    m_mousePosY = y;

    if (inBoundingBox()) {
        m_hovered = false;
        m_activated = true;
    }
}

void Menu2DItem::mouseReleased(int x, int y) {
    m_mousePosX = x;
    m_mousePosY = y;

    if (inBoundingBox()) {
        m_hovered = true;
        m_activated = false;

        m_clicked();	// boost signal
    }
}

void Menu2DItem::onClick(const boost::function<void()>& slot) {
    m_clicked.connect(slot);
}

void Menu2DItem::unClick() {
    m_clicked.disconnect_all_slots();
}