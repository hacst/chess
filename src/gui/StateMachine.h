#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include "gui/interface/AbstractState.h"

#include <vector>
#include <iostream>

// Forward declaration of GuiWindow.
class GuiWindow;

using namespace std;

/**
* @brief Class which manages the states.
* @note This is a singleton, you can get only one instance of the StateMachine.
* Don't forget to update the events if they occur.
*/
class StateMachine {
public:
	/**
	* @brief Gets an instance of the StateMachine.
	* @note This is a singleton. So you can only get one instance.
	* @return StateMachine& A reference to the StateMachine.
	*/
	static StateMachine& getInstance() {
		static StateMachine instance;
		return instance;
	}

	//! Structure for holding user events.
	struct EventMap {
		// mouse events
		bool mouseMoved = false;
		int mouseX = 0;
		int mouseY = 0;
		bool mouseDown = false;
		bool mouseUp = false;

		// keyboard events
		bool keyLeft = false;
		bool keyRight = false;
		bool keyDown = false;
		bool keyUp = false;
		bool keyEscape = false;
		bool keyReturn = false;
		bool key0 = false;
		bool key1 = false;
		bool keyA = false;
		bool keyY = false;
	} eventmap;

	//! Holds the pointer to the GuiWindow object, to access gui related methods.
	GuiWindow* window;

	/**
	* @brief Sets the start state and setup the state.
	* @param startState The start state.
	*/
	void setStartState(AbstractState* startState);

	/**
	* @brief Runs the current state.
	* @return The AbstractState pointer to the state which must be run() the next time.
	*/
	AbstractState* run();

	/**
	* @brief Sets the next state which should be run.
	* @param state The next state.
	*/
	void setNextState(AbstractState* state);

private:
	//! For singleton reasons no public constructor.
	StateMachine() {};								// The {} brackets are needed here.
	StateMachine(const StateMachine&);				// Don't implement
	StateMachine& operator=(const StateMachine&);	// Don't implement

	~StateMachine() {};

	//! The current state.
	AbstractState* m_currentState;
};

#endif // STATEMACHINE_H