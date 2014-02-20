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

#include "gui/AnimationHelper.h"
#include <iostream>
#include <math.h>

using namespace std;

AnimationHelper::AnimationHelper(int duration)
    : m_duration(duration)
    , m_startTime(0) {
}

void AnimationHelper::setStartNowOrKeepIt() {
    if (m_startTime == 0) {
        m_startTime = SDL_GetTicks();
    }
}

void AnimationHelper::reset() {
    m_startTime = SDL_GetTicks();
}

bool AnimationHelper::hasStopped() {
    bool hasStarted = (m_startTime > 0);
    bool hasEnded = getElapsedTime() > m_duration;

    return hasStarted && hasEnded;
}

unsigned int AnimationHelper::getElapsedTime() {
    return SDL_GetTicks() - m_startTime;
}

float AnimationHelper::ease(FunctionType type, const float lowerBound, const float upperBound) {
    // check if this function is called after the maximum animation duration is exceeded
    unsigned int elapsed_time = getElapsedTime();
    if (elapsed_time > m_duration) {
        return upperBound;
    }

    switch (type) {
    case EASE_OUTSINE:
        m_completeness = sinf(elapsed_time / static_cast<float>(m_duration)* (static_cast<float>(M_PI) / 2.f));
        break;
    case EASE_LINEAR:
    default:
        m_completeness = (elapsed_time / static_cast<float>(m_duration));
    }

    m_easingResult = lowerBound + ((upperBound - lowerBound) * m_completeness);

    return m_easingResult;
}
