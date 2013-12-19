#include <iostream>

#include "LoggingGameObserver.h"
#include "misc/helper.h"
#include <thread>

using namespace std;
using namespace Logging;

LoggingGameObserver::LoggingGameObserver()
    : m_log(initLogger("LoggingGameObserver")) {
    // Empty
}

void LoggingGameObserver::onGameStart(GameState state, GameConfiguration config) {
	LOG(info) << "Game started";
	LOG(info) << config;
	LOG(info) << endl << state;
}

void LoggingGameObserver::onTurnStart(PlayerColor who) {
	LOG(info) << who << " should now perform his turn" << endl;
}

void LoggingGameObserver::onTurnEnd(PlayerColor who, Turn turn, GameState newState) {
	LOG(info) << who << " performed: " << turn;
	LOG(info) << endl << newState << endl;
}

void LoggingGameObserver::onTurnTimeout(PlayerColor who, std::chrono::seconds timeout) {
	LOG(info) << who << " timed out after " << timeout.count() << " seconds";
}

void LoggingGameObserver::onGameOver(GameState state, PlayerColor winner) {
	LOG(info) << "Game Over. Winner: " << winner;
	LOG(info) << endl << state << endl;
}

