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

#ifndef ANIMATIONHELPER_H
#define ANIMATIONHELPER_H

#include <SDL.h>
#include <memory>

using namespace std;

/**
 * @brief The class helps to create animations by providing time dependent methods.
 */
class AnimationHelper {
public:
    /**
     * @brief Creates a new AnimationHelper object.
     * @param duration The period how long the animation should took.
     */
    AnimationHelper(const int duration);
    virtual ~AnimationHelper() { /* Nothing */ }

    //! The possible time function types.
    enum FunctionType {
        //! Linear
        EASE_LINEAR,

        //! Sinus like curve
        EASE_OUTSINE
    };

    /**
     * @brief Sets the current time as start point for the animation.
     * If this method is called multiple times, only the first call
     * will take effect.
     */
    void setStartNowOrKeepIt();

    /**
     * @brief Resets the start time stamp to the current time.
     */
    void reset();

    /**
     * @brief The percentage of the range between the lowerBound and upperBound.
     * @note The lowerBound must be less than upperBound.
     * @param type One of the FunctionType as defined above.
     * @param lowerBound The lower bound of the range.
     * @param upperBound The upper bound of the range.
     * @return The numeric value in percent. This will show the completeness
     * of the animation in percent between 0.0 and 1.0;
     */
    float ease(FunctionType type, const float lowerBound, const float upperBound);

    /**
     * @brief Gets the status of the animation.
     * @return True if the animation was started and has already finished. False if not.
     */
    bool hasStopped();

private:
    //! The duration of the animation in miliseconds.
    unsigned int m_duration;

    //! The start time of the animation.
    unsigned int m_startTime;

    //! The completeness of the animation (1/100 percent, 0.0 - 1.0).
    float m_completeness;

    //! The calculated result of the animation.
    float m_easingResult;

    /**
     * @brief Gets the elapsed time since the animation was started.
     * @return The number of miliseconds since the animation was started.
     */
    unsigned int getElapsedTime();
};

using AnimationHelperPtr = std::shared_ptr<AnimationHelper>; // Shared pointer for better garbage handling.

#endif // ANIMATIONHELPER_H
