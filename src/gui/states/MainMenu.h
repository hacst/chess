#ifndef STATEMAINMENU_H
#define STATEMAINMENU_H

#include "gui/interface/AbstractState.h"

class MainMenu : public AbstractState {
public:
	void enter() override;
	AbstractState* run() override;
	void exit() override;
};

#endif // STATEMENU_H