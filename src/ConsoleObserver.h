#ifndef CONSOLEOBSERVER_H
#define CONSOLEOBSERVER_H

#include "AbstractObserver.h"

class ConsoleObserver: public AbstractGameObserver {
public:
	void onGameStart(State state, GameConfiguration config) override;
	void onTurnEnd(PlayerColor who, Turn turn, State newState) override;
	void onGameOver(State state, PlayerColor winner) override;
};

using ConsoleObserverPtr = std::shared_ptr<ConsoleObserver>;

#endif // CONSOLEOBSERVER_H
