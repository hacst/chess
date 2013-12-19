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
			m_completeness = sinf(elapsed_time / static_cast<float>(m_duration)* (M_PI / 2));
			break;
		case EASE_LINEAR:
		default:
			m_completeness = (elapsed_time / static_cast<float>(m_duration));
	}

	m_easingResult = lowerBound + ((upperBound - lowerBound) * m_completeness);

	return m_easingResult;
}
