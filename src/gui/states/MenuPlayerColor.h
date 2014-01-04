#ifndef MENUPLAYERCOLOR_H
#define MENUPLAYERCOLOR_H

#include "gui/interface/AbstractState.h"
#include "gui/Menu2D.h"

class StateMachine;

/**
 * @brief Class which holds the state PlayerColor. This state let the user choose between
 * black or white for the chess model figures.
 *
 * @note To run() a state, first enter() the state.
 */
class MenuPlayerColor : public AbstractState {
public:
	/**
	 * @brief Creates a new menu PlayerColor State object.
	 */
	MenuPlayerColor();

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
	 * @brief This method is called if the user chose the white color as player color.
	 */
	void onColorWhite();

	/**
	 * @brief This method is called if the user chose the black color as player color.
	 */
	void onColorBlack();

	/**
	 * @brief This method is called if the user chose to go back to the menu he was, before he get here.
	 */
	void onMenuBack();

private:
	//! Reference to the StateMachine.
	StateMachine& m_fsm;

	//! Workaround to start game with white or black color.
	bool m_colorWhite;

	//! Internal GameMode states.
	enum States {
		KEEP_CURRENT,

		GAME_PLAY,
		MENU_GAME_MODE
	};

	//! The next State for the StateMachine to enter.
	States m_nextState;

	//! Shared pointer for better garbage handling.
	Menu2DPtr m_menu;
};

#endif // MENUPLAYERCOLOR_H