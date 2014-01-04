#ifndef ABSTRACTSTATE_H
#define ABSTRACTSTATE_H

/**
 * @brief Interface for modelling a game state.
 * @note To run() a state, first enter() the state.
 */
class AbstractState {
public:
	virtual ~AbstractState() {}

	/**
	 * @brief Enters the state for the first time. This will setup all the state related stuff.
	 * @note To run() the current state, first enter() it.
	 */
	virtual void enter() = 0;

	/**
	 * @brief Runs the current state and does all the work.
	 * @return AbstractState* the state which should be run after this state.
	 * A nullptr if the game should be exited.
	 */
	virtual AbstractState* run() = 0;

	/**
	 * @brief Exits the current state and cleans up all allocated resources.
	 * @note This is the last method to call, before the object is deleted.
	 */
	virtual void exit() = 0;

	/**
	 * @brief Draws something state related stuff on the screen.
	 */
	virtual void draw() = 0;
};

#endif // ABSTRACTSTATE_H
