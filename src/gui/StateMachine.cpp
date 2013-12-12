#include "gui/StateMachine.h"

void StateMachine::setStartState(AbstractState* startState) {
	m_currentState = startState;
	m_currentState->enter();
}

AbstractState* StateMachine::run() {
	AbstractState *nextState = m_currentState->run();
	
	if (nextState != m_currentState) {
		m_currentState->exit();
		delete m_currentState;	// free memory

		if (nextState != nullptr) {
			nextState->enter();
			m_currentState = nextState;
		}
	}

	return nextState;
}