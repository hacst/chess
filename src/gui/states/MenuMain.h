#ifndef MENUMAIN_H
#define MENUMAIN_H

#include "gui/interface/AbstractState.h"
#include "gui/Menu2D.h"

class StateMachine;

/**
 * @brief Class which holds the state MenuMain. in this menu, the user can start a new
 * game, load a previously saved game, go to the options menu or quit the game. 
 *
 * @note To run() a state, first enter() the state.
 */
class MenuMain : public AbstractState {
public:
	/**
	 * @brief Creates a new menu MainMenu State object.
	 */
	MenuMain();

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
	 * @brief This method is called if the user chose to play a new game.
	 */
	void onNewGame();

	/**
	 * @brief This method is called if the user chose to load a game.
	 */
	void onLoadGame();
	
	/**
	 * @brief This method is called if the user chose to go to the options menu.
	 */
	void onOptions();

	/**
	 * @brief This method is called if the user chose to exit the game.
	 */
	void onExitGame();

private:
	//! Reference to the StateMachine.
	StateMachine& fsm;

	//! Internal GameMode states.
	enum States {
		KEEP_CURRENT,

		NEW_GAME,
		LOAD_GAME,
		OPTIONS,
		EXIT_GAME
	};

	//! The next State for the StateMachine to enter.
	States m_nextState;

	//! Shared pointer for better garbage handling.
	Menu2DPtr menu;
};

#endif // MENUMAIN_H