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

	m_gamePlayState.startShowText(getColorString(who) + " ist an der Reihe.");

	cout << "GuiObserver (" << this_thread::get_id() << "): " << who << " should now perform his turn" << endl;
}

void GuiObserver::onTurnEnd(PlayerColor who, Turn turn, GameState newState) {
	m_chessSetPtr->setState(newState.getChessBoard().getBoard());
	m_chessSetPtr->setTurn(turn);

	cout << "GuiObserver (" << this_thread::get_id() << "): " << who << " performed: " << turn << endl;
	cout << endl << newState << endl << endl;
}

void GuiObserver::onTurnTimeout(PlayerColor who, std::chrono::seconds timeout) {
	m_gamePlayState.startShowText(getColorString(who) + " hat zu lange gebraucht.");

	cout << "GuiObserver (" << this_thread::get_id() << "): " << who << " timed out after " << timeout.count() << " seconds" << endl;
}

void GuiObserver::onGameOver(GameState state, PlayerColor winner) {
	m_gamePlayState.startShowText(winner + " hat gewonnen. Das Spiel ist zu Ende.");

	cout << "GuiObserver (" << this_thread::get_id() << "): Game Over. Winner: " << winner << endl;
	cout << endl << state << endl << endl;
}

std::string GuiObserver::getColorString(PlayerColor who) {
	string colorString;
	if (who == PlayerColor::Black) {
		colorString = "Schwarz";
	}
	else {
		colorString = "Weiss";
	}

	return colorString;
}