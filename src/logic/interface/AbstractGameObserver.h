#ifndef ABSTRACTGAMEOBSERVER_H
#define ABSTRACTGAMEOBSERVER_H

#include <memory>
#include <chrono>

#include "../GameState.h"
#include "core/GameConfiguration.h"

/**
 * @brief Allows to observe relevant GameEvents inside the GameLogic.
 * Classes of this type can be registered with the GameLogic to be
 * notified of relevant game events.
 * 
 * @note A Observer is only required to stay in a valid state for one game.
 * It is free to halt its operations after the end of the game.
 * 
 * @warning None of the functions in the class must block.
 */
class AbstractGameObserver {
public:
    virtual ~AbstractGameObserver() { /* Nothing */ }
    
    /**
     * @brief Called when the game starts.
     * @param state GameState on game start.
     * @param config Valid GameConfiguration for this game.
     */
    virtual void onGameStart(GameState state, GameConfiguration config ) { /* Nothing */ (void)state; (void)config; }
    
    /**
     * @brief Called if a player is asked to perform a turn.
     * @param who Color of the player doing the turn.
     */
    virtual void onTurnStart(PlayerColor who ) { /* Nothing */ (void)who; }
    
    /**
     * @brief Called if a player ended its turn.
     * @param who Color of the player doing the turn.
     * @param turn Turn the player decided on.
     * @param newState State after the player performed the turn.
     */
    virtual void onTurnEnd(PlayerColor who, Turn turn, GameState newState) { /* Nothing */ (void)who; (void)turn; (void)newState; }
    
    /**
     * @brief Called if a players turn is aborted due to timeout.
     * @param who Color of the player who got interrupted.
     * @param timeout Length of the time limit that got violated.
     */
    virtual void onTurnTimeout(PlayerColor who, std::chrono::seconds timeout) { /* Nothing */ (void)who; (void)timeout; }
    
    /**
     * @brief Called when a game started with onGameStart is over.
     * @param state State on game over.
     * @param winner Winner of the game.
     */
    virtual void onGameOver(GameState state, PlayerColor winner) { /* Nothing */ (void)state; (void) winner; }
};

using AbstractGameObserverPtr = std::shared_ptr<AbstractGameObserver>;

#endif // ABSTRACTGAMEOBSERVER_H
