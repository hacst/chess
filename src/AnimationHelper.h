#include <SDL.h>
#include <memory>

using namespace std;

#ifndef ANIMATIONHELPER_H
#define ANIMATIONHELPER_H

class AnimationHelper {
public:
	AnimationHelper(const int duration);
	virtual ~AnimationHelper() { /* Nothing */ }

	// config methods
	void setStartNowOrKeepIt();

	// ease functions
	float easeLinear(const float lowerBound, const float upperBound);
	float easeOutSine(const float lowerBound, const float upperBound);

private:
	int m_duration = 1000;			// duration in ms

	unsigned int m_startTime = 0;	// start time 
	unsigned int m_elapsedTime = 0;	// time in ms since helper is running
	float m_completeness = 0.0;		// percent / 100
	float m_easingResult = 0.0;		// the calculated result of the animation

	int getElapsedTime();
};

using AnimationHelperPtr = std::shared_ptr<AnimationHelper>;

#endif // ANIMATIONHELPER_H