#ifndef LOGGINGGAMEOBSERVER_H
#define LOGGINGGAMEOBSERVER_H

#include "logic/interface/AbstractGameObserver.h"
#include "core/Logging.h"

/**
 * @brief AbstractGameObserver which simply logs occuring events.
 */
class LoggingGameObserver: public AbstractGameObserver {
public:
    LoggingGameObserver();
    void onGameStart(GameState state, GameConfiguration config) override;
	void onTurnStart(PlayerColor who) override;
    void onTurnEnd(PlayerColor who, Turn turn, GameState newState) override;
	void onTurnTimeout(PlayerColor who, std::chrono::seconds timeout) override;
    void onGameOver(GameState state, PlayerColor winner) override;
private:
    Logging::Logger m_log;
};

using LoggingGameObserverPtr = std::shared_ptr<LoggingGameObserver>;

#endif // LOGGINGGAMEOBSERVER_H
