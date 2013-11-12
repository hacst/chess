#include <SDL.h>

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
	int duration = 1000;			// duration in ms
	int startTime = 0;				// start time 
	unsigned int elapsedTime = 0;	// time in ms since helper is running
	float completeness = 0.0;		// percent / 100
	float easingResult = 0.0;		// the calculated result of the animation

	int getElapsedTime();
};

#endif // ANIMATIONHELPER_H