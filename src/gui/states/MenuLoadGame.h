#ifndef MENULOADGAME_H
#define MENULOADGAME_H

#include "gui/interface/AbstractState.h"
#include "gui/states/MenuLoadGame.h"
#include "gui/Menu2D.h"

class StateMachine;

class MenuLoadGame : public AbstractState {
public:
	MenuLoadGame();

	// interface methods
	void enter() override;
	AbstractState* run() override;
	void exit() override;

	// draw method
	void draw();

	// state methods
	void onMenuBack();

private:
	StateMachine& fsm;

	enum States {
		KEEP_CURRENT,

		LOAD_SLOT_A,
		LOAD_SLOT_B,
		LOAD_SLOT_C,
		MENU_BACK
	};
	States m_nextState;

	// menu smart pointer handle
	Menu2DPtr menu;
};

#endif // MENULOADGAME_H