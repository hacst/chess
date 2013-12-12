#ifndef CONSOLEOBSERVER_H
#define CONSOLEOBSERVER_H

#include "logic/interface/AbstractGameObserver.h"

class ConsoleObserver: public AbstractGameObserver {
public:
    void onGameStart(GameState state, GameConfiguration config) override;
	void onTurnStart(PlayerColor who) override;
    void onTurnEnd(PlayerColor who, Turn turn, GameState newState) override;
	void onTurnTimeout(PlayerColor who, std::chrono::seconds timeout) override;
    void onGameOver(GameState state, PlayerColor winner) override;
};

using ConsoleObserverPtr = std::shared_ptr<ConsoleObserver>;

#endif // CONSOLEOBSERVER_H
