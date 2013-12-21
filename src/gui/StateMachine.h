#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include "gui/interface/AbstractState.h"

#include <vector>
#include <iostream>

using namespace std;

class GuiWindow;

class StateMachine {
public:
	// singleton
	static StateMachine& getInstance() {
		static StateMachine instance;
		return instance;
	}

	struct EventMap {
		// mouse
		bool mouseMoved = false;
		int mouseX = 0;
		int mouseY = 0;
		bool mouseDown = false;
		bool mouseUp = false;

		// keyboard
		bool keyLeft = false;
		bool keyRight = false;
		bool keyDown = false;
		bool keyUp = false;
		bool keyEscape = false;
		bool key0 = false;
		bool key1 = false;
		bool keyA = false;
		bool keyY = false;
	} eventmap;

	GuiWindow* window;

	void setStartState(AbstractState* startState);
	AbstractState* run();
	void setNextState(AbstractState* state);

private:
	StateMachine() {};								// The {} brackets are needed here.
	~StateMachine() {};

	StateMachine(const StateMachine&);				// Don't implement
	//StateMachine& operator=(const StateMachine&);	// Don't implement

	AbstractState* m_endState;
	AbstractState* m_currentState;
};

#endif // STATEMACHINE_H