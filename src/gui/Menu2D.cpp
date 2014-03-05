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

#include "gui/Menu2D.h"
#include "gui/GuiWindow.h"

using namespace std;

Menu2D::Menu2D(int windowWidth, int windowHeight)
    : m_windowWidth(windowWidth)
    , m_windowHeight(windowHeight)
    , m_btCount(0) {

    // create an animation object
    animationHelper = make_shared<AnimationHelper>(m_animationDuration);
}

Menu2D::~Menu2D() {
    for (auto& item : items) {
        item->unClick();
        item.reset();
    }

    items.clear();
}

Menu2DItemPtr Menu2D::addButton(std::string filename) {
    updateAbsolutePosition();	// update absolute menu position

    // we don't know how many buttons will be added in total, so we must update the position later (before drawing)
    Menu2DItemPtr item = make_shared<Menu2DItem>(filename, m_buttonWidth, m_buttonHeight);
    items.push_back(item);

    ++m_btCount;

    return item;
}

void Menu2D::updateAbsolutePosition() {
    m_height = (m_btCount + 1) * (m_buttonHeight + m_buttonMargin);	// total height of menu
    m_marginTop = (m_windowHeight / 2) - (m_height / 2);			// center in height
    m_marginLeft = (m_windowWidth / 2) - (m_buttonWidth / 2);		// center in width
}

void Menu2D::draw() {
    glEnable(GL_COLOR);
    glEnable(GL_BLEND);

    glPushMatrix();

        // now we know how many buttons are on our list, so we can position them right now
        int index = 0;
        for (auto& item : items) {
            int x = m_marginLeft;	// always the same distance to left
            int y = (index * m_buttonHeight) + ((index + 1) * m_buttonMargin) + m_marginTop - (m_buttonMargin / 2);

            item->setPosition(x, y);
            ++index;
        }

        // draw a transparent background for the menu with an animation
        animationHelper->setStartNowOrKeepIt();
        glColor4f(0.2f, 0.2f, 0.3f, animationHelper->ease(AnimationHelper::EASE_LINEAR, 0.1f, 0.9f));

        glBegin(GL_QUADS);
            glVertex2i(m_marginLeft - 20, m_marginTop - 20);
            glVertex2i(m_marginLeft + m_buttonWidth + 20, m_marginTop - 20);
            glVertex2i(m_marginLeft + m_buttonWidth + 20, m_marginTop + m_height + 20);
            glVertex2i(m_marginLeft - 20, m_marginTop + m_height + 20);
        glEnd();

        // iterate over all items and let them draw
        for (auto& item : items) {
            item->draw();
        }

    glPopMatrix();

    glDisable(GL_BLEND);
    glDisable(GL_COLOR);
}

void Menu2D::mouseMoved(const int x, const int y) {
    mouseState.x = x;
    mouseState.y = y;

    for (auto& item : items) {
        item->mouseMoved(mouseState.x, mouseState.y);
    }
}

void Menu2D::mousePressed() {
    // clicked at already known x/y position
    for (auto& item : items) {
        item->mousePressed(mouseState.x, mouseState.y);
    }
}

void Menu2D::mouseReleased() {
    // released at already known x/y position
    for (auto& item : items) {
        item->mouseReleased(mouseState.x, mouseState.y);
    }
}

void Menu2D::windowResized(int newWidth, int newHeight) {
    m_windowWidth = newWidth;
    m_windowHeight = newHeight;

    updateAbsolutePosition();
}

void Menu2D::resetAnimation() {
    animationHelper->reset();
}