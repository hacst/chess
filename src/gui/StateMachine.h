#ifndef STATEMACHINE_H
#define STATEMACHINE_H

#include "gui/interface/AbstractState.h"

#include <vector>
#include <iostream>

using namespace std;

class StateMachine {
public:
	// singleton
	static StateMachine& getInstance() {
		static StateMachine instance;
		return instance;
	}

	void setStartState(AbstractState* startState);
	AbstractState* run();

private:
	StateMachine() {};								// The {} brackets are needed here.
	~StateMachine() {};

	StateMachine(const StateMachine&);				// Don't implement
	StateMachine& operator=(const StateMachine&);	// Don't implement

	AbstractState* m_endState;
	AbstractState* m_currentState;
};

#endif // STATEMACHINE_H