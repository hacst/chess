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

#include "gui/ResourceInitializer.h"

using namespace std;

ResourceInitializer::ResourceInitializer()
    : m_fsm(StateMachine::getInstance()) {
}

ChessSetPtr ResourceInitializer::load() {
    m_chessSet = make_shared<ChessSet>();

    m_resourcesLoaded = 0;
    m_resourcesTotal = m_chessSet->getResourcesCount();

    m_chessSet->registerLoadCallback(boost::bind(&ResourceInitializer::onBeforeLoadNextResource, this, _1));
    m_chessSet->loadResources();

    return m_chessSet;
}

void ResourceInitializer::onBeforeLoadNextResource(string resourceName) {
    // swap the frame buffer for the first time to
    if (m_resourcesLoaded == 0) {
        m_fsm.window->swapFrameBufferNow();
    }

    // print what is loaded (resource name + progress bar)
    ++m_resourcesLoaded;

    int windowWidth = m_fsm.window->getWidth();
    float percentLoaded = m_resourcesLoaded / static_cast<float>(m_resourcesTotal);

    array<float, 2> topLeftVertex = { 0.0f, 0.0f };
    array<float, 2> bottomLeftVertex = { 0.0f, 10.0f };
    array<float, 2> bottomRightVertex = { windowWidth * percentLoaded, 10.0f };
    array<float, 2> topRightVertex = { windowWidth * percentLoaded, 0.0f };

    m_fsm.window->set2DMode();

    glPushMatrix();
        glBegin(GL_QUADS);
            // left side is grey
            glColor3f(0.6f, 0.6f, 0.6f);
            glVertex3f(topLeftVertex[0], topLeftVertex[1], -0.1f);
            glVertex3f(bottomLeftVertex[0], bottomLeftVertex[1], -0.1f);

            // right side is white
            glColor3f(1.0f, 1.0f, 1.0f);
            glVertex3f(bottomRightVertex[0], bottomRightVertex[1], -0.1f);
            glVertex3f(topRightVertex[0], topRightVertex[1], -0.1f);
        glEnd();
    glPopMatrix();

    m_fsm.window->printText(10, m_fsm.window->getHeight() - 30, 1.0, 1.0, 1.0, resourceName + " (" + to_string(m_resourcesLoaded) + " of " + to_string(m_resourcesTotal) + ")");

    // we must now swap the frame buffer
    m_fsm.window->swapFrameBufferNow();

    m_fsm.window->set3DMode();
}