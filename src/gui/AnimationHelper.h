#ifndef ANIMATIONHELPER_H
#define ANIMATIONHELPER_H

#include <SDL.h>
#include <memory>

using namespace std;

/**
 * The class helps to create animations by providing time dependent methods.
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
