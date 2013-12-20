#include <iostream>

#include "GuiObserver.h"
#include "misc/helper.h"
#include <thread>

#include "gui/states/GamePlay.h"

using namespace std;

GuiObserver::GuiObserver(ChessSetPtr chessSetPtr, GamePlay& gamePlayState)
	: m_chessSetPtr(chessSetPtr)
	, m_gamePlayState(gamePlayState) {
}

void GuiObserver::onGameStart(GameState state, GameConfiguration config) {
	// white begins the match -> camera is at start always behind white!!!
	m_currentPlayerColor = PlayerColor::White;
	m_chessSetPtr->setState(state.getChessBoard().getBoard());
	
	cout << "GuiObserver (" << this_thread::get_id() << "): Game started" << endl;
	cout << config << endl;
	cout << endl << state << endl << endl;
}

void GuiObserver::onTurnStart(PlayerColor who) {
	if (who != m_currentPlayerColor) {
		m_currentPlayerColor = who;
		m_gamePlayState.startCameraRotation();
	}

	cout << "GuiObserver (" << this_thread::get_id() << "): " << who << " should now perform his turn" << endl;
}

void GuiObserver::onTurnEnd(PlayerColor who, Turn turn, GameState newState) {
	m_chessSetPtr->setState(newState.getChessBoard().getBoard());
	
	cout << "GuiObserver (" << this_thread::get_id() << "): " << who << " performed: " << turn << endl;
	cout << endl << newState << endl << endl;
}

void GuiObserver::onTurnTimeout(PlayerColor who, std::chrono::seconds timeout) {
	cout << "GuiObserver (" << this_thread::get_id() << "): " << who << " timed out after " << timeout.count() << " seconds" << endl;
}

void GuiObserver::onGameOver(GameState state, PlayerColor winner) {
	cout << "GuiObserver (" << this_thread::get_id() << "): Game Over. Winner: " << winner << endl;
	cout << endl << state << endl << endl;
}

