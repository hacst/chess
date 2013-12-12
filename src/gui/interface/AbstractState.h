#ifndef ABSTRACTSTATE_H
#define ABSTRACTSTATE_H

//class GuiWindow; // do not include header file of GuiWindow, just pointer dealing here!

class AbstractState {
public:
	virtual void enter() = 0;
	virtual AbstractState* run() = 0;
	virtual void exit() = 0;
	virtual void draw() = 0;
};

#endif // ABSTRACTSTATE_H