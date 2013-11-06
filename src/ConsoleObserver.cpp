#include <iostream>

#include "ConsoleObserver.h"
#include "helper.h"

using namespace std;

void ConsoleObserver::onGameStart(State state, GameConfiguration config) {
	cout << "ConsoleObserver: Game started" << endl;
	cout << config << endl;
	cout << endl << state << endl << endl;
}

void ConsoleObserver::onTurnStart(PlayerColor who) {
	cout << "ConsoleObserver: " << who << " should now perform his turn" << endl;
}

void ConsoleObserver::onTurnEnd(PlayerColor who, Turn turn, State newState) {
	cout << "ConsoleObserver: " << who << " performed: " << turn << endl;
	cout << endl << newState << endl << endl;
}

void ConsoleObserver::onTurnTimeout(PlayerColor who, std::chrono::seconds timeout) {
	cout << "ConsoleObserver: " << who << " timed out after " << timeout.count() << " seconds" << endl;
}

void ConsoleObserver::onGameOver(State state, PlayerColor winner) {
	cout << "ConsoleObserver: Game Over. Winner: " << winner << endl;
	cout << endl << state << endl << endl;
}

