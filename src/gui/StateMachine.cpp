#include "gui/StateMachine.h"

void StateMachine::setStartState(AbstractState* startState) {
	m_currentState = startState;
	m_currentState->enter();
}

AbstractState* StateMachine::run() {
	AbstractState *nextState = m_currentState->run();

	if (nextState != m_currentState) {
		m_currentState->exit();
		nextState->enter();
	}

	return nextState;
}