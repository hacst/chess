#include <iostream>

#include "ConsolePlayer.h"

using namespace std;

ConsolePlayer::~ConsolePlayer() {
	stop(true);
}

void ConsolePlayer::onSetColor(PlayerColor color) {
	post([=]() {
		cout << "You will be playing " << color << endl;
		m_color = color;
	});
}

void ConsolePlayer::onGameStart(GameState state, GameConfiguration config) {
	post([=]() {
		cout << "Game started" << endl;
			 //<< state << endl
			 //<< config << endl;
	});
}

future<Turn> ConsolePlayer::doMakeTurn(GameState state) {
	return postPromise([=]() {
		cout << "Your turn (press enter)" << endl;
		cin.get();
		return Turn();
	});
}

void ConsolePlayer::onTurnEnd(PlayerColor color, Turn turn, GameState newState) {
	post([=]() {
		if (color != m_color) {
			cout << turn << endl;
		}
		cout << newState << endl;
	});
}

void ConsolePlayer::doAbortTurn() {
	cout << endl << "Please end your turn (press enter)" << endl;
}

void ConsolePlayer::onGameOver(GameState state, PlayerColor winner) {
	cout << "Game over: ";

    if (winner == NoPlayer) {
		cout << "Draw" << endl;
	} else if (winner == m_color) {
		cout << "You won" << endl;
	} else {
		cout << "You lost" << endl;
	}
}

