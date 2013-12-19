#ifndef ANIMATIONHELPER_H
#define ANIMATIONHELPER_H

#include <SDL.h>
#include <memory>

using namespace std;

class AnimationHelper {
public:
	AnimationHelper(const int duration);
	virtual ~AnimationHelper() { /* Nothing */ }

	enum FunctionType {
		EASE_LINEAR,
		EASE_OUTSINE
	};

	// config methods
	void setStartNowOrKeepIt();
	void reset();

	// ease functions
	float ease(FunctionType type, const float lowerBound, const float upperBound);

	// status methods
	bool hasStopped();

private:
	unsigned int m_duration;	// duration in ms

	unsigned int m_startTime;	// start time 
	float m_completeness;		// percent / 100
	float m_easingResult;		// the calculated result of the animation

	unsigned int getElapsedTime();
};

using AnimationHelperPtr = std::shared_ptr<AnimationHelper>;

#endif // ANIMATIONHELPER_H
