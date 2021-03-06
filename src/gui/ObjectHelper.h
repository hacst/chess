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

#ifndef OBJECTHELPER_H
#define OBJECTHELPER_H

#ifdef _WIN32
#include <windows.h> // Needed for gl.h
#endif

#include <GL/gl.h>
#include <memory>

/**
 * @brief Helper class for creating static OpenGL display lists to boost the
 * drawing of OpenGL objects.
 * @note See http://www.opengl.org/documentation/specs/version1.1/glspec1.1/node123.html for more
 * details about display lists.
 */
class ObjectHelper {
public:
    /**
     * @brief Creates a new OpenGL display list for a cube.
     * @param size The size of an edge of the cube.
     * @param x The position of the cube in x world coordinate.
     * @param y The position of the cube in y world coordinate.
     * @param z The position of the cube in z world coordinate.
     * @return GLuint A display list index which holds the compiled cube.
     */
    static GLuint createCubeList(float size, float x, float y, float z);

    /**
     * @brief Creates a new OpenGL display list for a 2D rectangle box.
     * @param width The width of the rectangle.
     * @param height The height of the rectangle.
     * @param viewportX The viewport x coordinate from the left top corner.
     * @param viewportY The viewport y coordinate from the left top corner.
     * @param colorR The red color value between 0.0 and 1.0.
     * @param colorG The green color value between 0.0 and 1.0.
     * @param colorB The blue color value between 0.0 and 1.0.
     * @return GLuint A display list index which holds the compiled rectangle.
     */
    static GLuint create2DRectList(float width, float height, float viewportX, float viewportY, float colorR, float colorG, float colorB);

    /**
     * @brief Creates a new OpenGL display list for a 2D rectangle box with gradient color from top to bottom.
     * @param width The width of the rectangle.
     * @param height The height of the rectangle.
     * @param viewportX The viewport x coordinate from the left top corner.
     * @param viewportY The viewport y coordinate from the left top corner.
     * @param fromColorR The red color value between 0.0 and 1.0 at the top edge of the rectangle.
     * @param fromColorG The green color value between 0.0 and 1.0 at the top edge of the rectangle.
     * @param fromColorB The blue color value between 0.0 and 1.0 at the top edge of the rectangle.
     * @param toColorR The red color value between 0.0 and 1.0 at the bottom edge of the rectangle.
     * @param toColorG The green color value between 0.0 and 1.0 at the bottom edge of the rectangle.
     * @param toColorB The blue color value between 0.0 and 1.0 at the bottom edge of the rectangle.
     * @return GLuint A display list index which holds the compiled rectangle.
     */
    static GLuint create2DGradientRectList(float width, float height, float viewportX, float viewportY, float fromColorR, float fromColorG, float fromColorB, float toColorR, float toColorG, float toColorB);

private:
    //! No constructor needed.
    ObjectHelper() {};
};

#endif // OBJECTHELPER_H