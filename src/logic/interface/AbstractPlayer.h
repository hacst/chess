#ifndef ABSTRACTPLAYER_H
#define ABSTRACTPLAYER_H

#include <future>
#include "logic/interface/AbstractGameObserver.h"

/**
 * @brief Class a player has to implement to interact with the GameLogic.
 * Every player is also a AbstractGameObserver which is notified of
 * relevant game events. You do not need to register the player as an
 * observer for this to happen.
 * 
 * @note A Observer is only required to stay in a valid state for one game.
 * It is free to halt its operations after the end of the game.
 *  
 * @warning None of the functions in the class must block.
 */
class AbstractPlayer : public AbstractGameObserver {
public:
    virtual ~AbstractPlayer() { /* Nothing */ }

    /**
     * @brief Notifies that player what color he will be playing.
     * Called before onGameStart.
     * @param color Color the player has.
     */
    virtual void onSetColor(PlayerColor color) = 0;
    
    /**
     * @brief Asks the player to make his turn.
     * @warning This function must not block. It is to return immediatly.
     *       The players turn is to be set on the returned future.
     * @note The game logic can abort its request for a player to make
     *       his turn using the doAbortTurn function at any time.
     * @param state Current state of the game.
     * @return A future to the turn to make.
     */
    virtual std::future<Turn> doMakeTurn(GameState state) = 0;
    
    /**
     * @brief Asks the player to abort a turn asked for with doMakeTurn.
     * When this is called the GameLogic will no longer react to the
     * completion of the future for that turn. A use of this function
     * is the abortion of a turn due to timeout.
     */
	virtual void doAbortTurn() = 0;
};

using AbstractPlayerPtr = std::shared_ptr<AbstractPlayer>;


#endif // ABSTRACTPLAYER_H
