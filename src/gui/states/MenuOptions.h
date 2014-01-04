#ifndef MENUOPTIONS_H
#define MENUOPTIONS_H

#include "gui/interface/AbstractState.h"
#include "gui/states/MenuOptions.h"
#include "gui/Menu2D.h"

class StateMachine;

/**
 * @brief Class which holds the state MenuOption. This state let the user toggle between
 * the fullscreen view or the windowed mode of the game.
 *
 * @note To run() a state, first enter() the state.
 */
class MenuOptions : public AbstractState {
public:
	/**
	 * @brief Creates a new menu Options State object.
	 */
	MenuOptions();

	/**
	 * @brief Enters the state for the first time. This will setup all the state related stuff.
	 * @note To run() the current state, first enter() it.
	 */
	void enter() override;

	/**
	 * @brief Runs the current state and does all the work.
	 * @return AbstractState* the state which should be run after this state.
	 * A nullptr if the game should be exited.
	 */
	AbstractState* run() override;

	/**
	 * @brief Exits the current state and cleans up all allocated resources.
	 * @note This is the last method to call, before the object is deleted.
	 */
	void exit() override;

	/**
	 * @brief Draws something state related stuff on the screen.
	 */
	void draw();

	/**
	 * @brief This method is called if the user chose to change the resolution.
	 */
	void onResolutionChange();

	/**
	 * @brief This method is called if the user chose to go back to the menu he was, before he get here.
	 */
	void onMenuBack();

private:
	//! Reference to the StateMachine.
	StateMachine& fsm;

	//! Internal GameMode states.
	enum States {
		KEEP_CURRENT,

		MENU_MAIN
	};

	//! The next State for the StateMachine to enter.
	States m_nextState;

	//! Shared pointer for better garbage handling.
	Menu2DPtr menu;
};

#endif // MENUOPTIONS_H