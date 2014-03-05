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

#include <iostream>

#include "gui/Model.h"

using namespace std;

Model::Model(string file) : m_file(file) {
    loadScene();

    m_correctPosition = { 0, 0, 0 };
    m_correctScaling = 1.0f;
    m_correctRotation = { 0, 0, 0 };

    m_position = { 0, 0, 0 };
}

void Model::setCorrectionValues(int localX, int localY, int localZ,
    float scaleFactor,
    int rotateX, int rotateY, int rotateZ) {
    m_correctPosition = { localX, localY, localZ };
    m_correctScaling = scaleFactor;
    m_correctRotation = { rotateX, rotateY, rotateZ };
}

void Model::setColor(Color color) {
    m_color = color;
}

void Model::setPosition(int globalX, int globalY, int globalZ) {
    m_position = { globalX, globalY, globalZ };
}

void Model::loadScene() {
    model = make_shared<AssimpHelper>();
    model->importScene(m_file);
}

void Model::draw() {
    glPushMatrix();
        bool flipModelDirection = true;
        GLfloat emission[] = { 0.2f, 0.2f, 0.2f, 1.0f };

        GLfloat ambient[] = { 0.05f, 0.05f, 0.05f, 1.0f };
        glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);

        GLfloat diffuse[] = { 0.97f, 0.92f, 0.79f, 1.0f };

        if (m_color == Color::BLACK) {
            flipModelDirection = false;

            emission[0] = 0.05f;
            emission[1] = 0.05f;
            emission[2] = 0.05f;

            diffuse[0] = 0.1f;
            diffuse[1] = 0.1f;
            diffuse[2] = 0.1f;
        }
        glMaterialfv(GL_FRONT, GL_EMISSION, emission);
        glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);

        GLfloat specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        glMaterialfv(GL_FRONT, GL_SPECULAR, specular);

        GLfloat shininess[] = { 128 };
        glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

        // note: transformation matrices are multiplied from right -> last matrix first!
        glTranslatef((float)m_position.x, (float)m_position.y, (float)m_position.z);							// 5) translate to world space coordinates
        glScalef(m_correctScaling, m_correctScaling, m_correctScaling);											// 4) scale
        glRotatef(flipModelDirection ? 180.0f : 0.0f, 0.0f, 1.0f, 0.0f);										// 3) correct rotation y (assertion: we are now in (0,0,0) local space because of step 2)
        glTranslatef((float)m_correctPosition.x, (float)m_correctPosition.y, (float)m_correctPosition.z);	    // 2) correct local origin
        glRotatef(-90.0, 1.0f, 0.0f, 0.0f);																		// 1) correct rotation around x-axis to "stand-up" the model

        model->drawScene();																						// 6) draw the scene
    glPopMatrix();
}
