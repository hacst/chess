#include "AnimationHelper.h"

using namespace std;

AnimationHelper::AnimationHelper(const int duration) {
	this->duration = duration;
}

void AnimationHelper::setStartNowOrKeepIt() {
	if (startTime == 0) {
		startTime = SDL_GetTicks();
	}
}

int AnimationHelper::getElapsedTime() {
	return SDL_GetTicks() - startTime;
}

float AnimationHelper::easeLinear(const float lowerBound, const float upperBound) {
	// check if this function is called after the maximum animation duration is exceeded
	if (getElapsedTime() > duration) {
		return easingResult;
	}

	completeness = (getElapsedTime() / static_cast<float>(duration));
	easingResult = lowerBound + ((upperBound - lowerBound) * completeness);

	return easingResult;
}

float AnimationHelper::easeOutSine(const float lowerBound, const float upperBound) {
	return 0.0;
}