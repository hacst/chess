#include "gui/AnimationHelper.h"
#include <iostream>
#include <math.h>

using namespace std;

AnimationHelper::AnimationHelper(int duration) {
	m_duration = duration;
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
	bool hasStarted = m_startTime;
	bool hasEnded = getElapsedTime() > m_duration;
	
	return hasStarted && hasEnded;
}

int AnimationHelper::getElapsedTime() {
	return SDL_GetTicks() - m_startTime;
}

float AnimationHelper::easeLinear(const float lowerBound, const float upperBound) {
	// check if this function is called after the maximum animation duration is exceeded
    int elapsed_time = getElapsedTime();
	if (elapsed_time > m_duration) {
		return m_easingResult;
	}

	m_completeness = (elapsed_time / static_cast<float>(m_duration));
	m_easingResult = lowerBound + ((upperBound - lowerBound) * m_completeness);

	return m_easingResult;
}

float AnimationHelper::easeOutSine(const float lowerBound, const float upperBound) {
	// check if this function is called after the maximum animation duration is exceeded
    int elapsed_time = getElapsedTime();
	if (elapsed_time > m_duration) {
		return m_easingResult;
	}

	m_completeness = sin(elapsed_time / static_cast<float>(m_duration)* (M_PI / 2));
	m_easingResult = lowerBound + ((upperBound - lowerBound) * m_completeness);

	return m_easingResult;
}
