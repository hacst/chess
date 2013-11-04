#include <iostream>

#include "ConsoleObserver.h"
#include "helper.h"

using namespace std;

void ConsoleObserver::onGameStart(State state, GameConfiguration config) {
	cout << "Game started" << endl;
	cout << config << endl;
	cout << endl << state << endl << endl;
}

void ConsoleObserver::onTurnEnd(PlayerColor who, Turn turn, State newState) {
	cout << who << " performed: " << turn << endl;
	cout << endl << newState << endl << endl;
}

void ConsoleObserver::onGameOver(State state, PlayerColor winner) {
	cout << "Game Over. Winner: " << winner << endl;
	cout << endl << state << endl << endl;
}

