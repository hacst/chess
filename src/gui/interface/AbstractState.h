#ifndef STATE_H
#define STATE_H

class AbstractState {
public:
	virtual void enter() = 0;
	virtual AbstractState* run() = 0;
	virtual void exit() = 0;
};

#endif // STATE_H