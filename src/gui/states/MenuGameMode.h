#ifndef MENUGAMEMODE_H
#define MENUGAMEMODE_H

#include "gui/interface/AbstractState.h"
#include "gui/states/MenuGameMode.h"
#include "gui/Menu2D.h"

class StateMachine;

/**
 * @brief Class which holds the state GameMode. This state let the user choose one of two modes.
 * The _AI vs. AI_ mode which shows a chess match between two artificial computer players where
 * the user can only watch the game. In the _Player vs. AI_ mode, the user can play against an
 * artificial computer player.
 *
 * @note To run() a state, first enter() the state.
 */
class MenuGameMode : public AbstractState {
public:
	/**
	 * @brief Creates a new menu GameMode State object.
	 */
	MenuGameMode();

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
	 * @brief This method is called if the user chose the _AI vs. AI_ mode.
	 */
	void onModeAIVsAI();

	/**
	 * @brief This method is called if the user chose the _Player vs. AI_ mode.
	 */
	void onModePlayerVsAI();

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

		GAME_PLAY,
		MENU_PLAYER_COLOR,
		MENU_MAIN
	};

	//! The next State for the StateMachine to enter.
	States m_nextState;

	//! Shared pointer for better garbage handling.
	Menu2DPtr menu;
};

#endif // MENUGAMEMODE_H