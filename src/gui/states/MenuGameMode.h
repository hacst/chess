#ifndef MENUGAMEMODE_H
#define MENUGAMEMODE_H

#include "gui/interface/AbstractState.h"
#include "gui/states/MenuGameMode.h"
#include "gui/Menu2D.h"

class StateMachine;

class MenuGameMode : public AbstractState {
public:
	MenuGameMode();

	// interface methods
	void enter() override;
	AbstractState* run() override;
	void exit() override;

	// draw method
	void draw();

	// state methods
	void onModeKIVsKI();
	void onModePlayerVsKI();
	void onMenuBack();

private:
	StateMachine& fsm;

	enum States {
		KEEP_CURRENT,

		GAME_PLAY,
		MENU_PLAYER_COLOR,
		MENU_MAIN
	};
	States m_nextState;

	// menu smart pointer handle
	Menu2DPtr menu;
};

#endif // MENUGAMEMODE_H