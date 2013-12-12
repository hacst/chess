#ifndef MENUOPTIONS_H
#define MENUOPTIONS_H

#include "gui/interface/AbstractState.h"
#include "gui/states/MenuOptions.h"
#include "gui/Menu2D.h"

class StateMachine;

class MenuOptions : public AbstractState {
public:
	MenuOptions();

	// interface methods
	void enter() override;
	AbstractState* run() override;
	void exit() override;

	// draw method
	void draw();

	// state methods
	void onResolutionChange();
	void onMenuBack();

private:
	StateMachine& fsm;

	enum States {
		KEEP_CURRENT,

		MENU_BACK
	};
	States m_nextState;

	// menu smart pointer handle
	Menu2DPtr menu;
};

#endif // MENUOPTIONS_H