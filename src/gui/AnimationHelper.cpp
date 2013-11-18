#include "gui/AnimationHelper.h"

using namespace std;

AnimationHelper::AnimationHelper(int duration) {
	m_duration = duration;
}

void AnimationHelper::setStartNowOrKeepIt() {
	if (m_startTime == 0) {
		m_startTime = SDL_GetTicks();
	}
}

int AnimationHelper::getElapsedTime() {
	return SDL_GetTicks() - m_startTime;
}

float AnimationHelper::easeLinear(const float lowerBound, const float upperBound) {
	// check if this function is called after the maximum animation duration is exceeded
	if (getElapsedTime() > m_duration) {
		return m_easingResult;
	}

	m_completeness = (getElapsedTime() / static_cast<float>(m_duration));
	m_easingResult = lowerBound + ((upperBound - lowerBound) * m_completeness);

	return m_easingResult;
}

float AnimationHelper::easeOutSine(const float lowerBound, const float upperBound) {
	return 0.0;
}