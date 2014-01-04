#ifndef MENULOADGAME_H
#define MENULOADGAME_H

#include "gui/interface/AbstractState.h"
#include "gui/states/MenuLoadGame.h"
#include "gui/Menu2D.h"

class StateMachine;

/**
 * @brief Class which holds the state LoadGame. The user can load a previously
 * saved game from one of three game slots.
 *
 * @note To run() a state, first enter() the state.
 */
class MenuLoadGame : public AbstractState {
public:
	/**
	 * @brief Creates a new menu LoadGame State object.
	 */
	MenuLoadGame();

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
	 * @brief This method is called if the user chose the back button.
	 */
	void onMenuBack();

private:
	//! Reference to the StateMachine.
	StateMachine& fsm;

	//! Internal GameMode states.
	enum States {
		KEEP_CURRENT,

		LOAD_SLOT_A,
		LOAD_SLOT_B,
		LOAD_SLOT_C,
		MENU_MAIN
	};

	//! The next State for the StateMachine to enter.
	States m_nextState;

	//! Shared pointer for better garbage handling.
	Menu2DPtr menu;
};

#endif // MENULOADGAME_H