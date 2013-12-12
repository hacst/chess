#ifndef MENUMAIN_H
#define MENUMAIN_H

#include "gui/interface/AbstractState.h"
#include "gui/Menu2D.h"

class StateMachine;

class MenuMain : public AbstractState {
public:	
	MenuMain();

	// interface methods
	void enter() override;
	AbstractState* run() override;
	void exit() override;
	
	// draw method
	void draw();

	// state methods
	void onNewGame();
	void onLoadGame();
	void onOptions();
	void onExitGame();

private:
	StateMachine& fsm;

	enum States {
		KEEP_CURRENT,

		NEW_GAME,
		LOAD_GAME,
		OPTIONS,
		EXIT_GAME
	};
	States m_nextState;

	// menu smart pointer handle
	Menu2DPtr menu;
};

#endif // MENUMAIN_H