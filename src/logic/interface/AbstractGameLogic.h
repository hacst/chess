#ifndef ABSTRACTGAMELOGIC_H
#define ABSTRACTGAMELOGIC_H

#include "logic/ChessBoard.h"
#include "logic/interface/AbstractPlayer.h"
#include "logic/interface/AbstractGameObserver.h"

/**
 * @brief Interface for chess game logic implementations.
 */
class AbstractGameLogic {
public:
    virtual ~AbstractGameLogic() { /* Nothing */ }

    virtual AbstractPlayerPtr getWhitePlayer() const = 0;
    virtual AbstractPlayerPtr getBlackPlayer() const = 0;

    /**
     * @brief Registers an observer for game events.
     * @see AbstractGameObserver for the available events.
     * @param observer Observer to register.
     */
    virtual void addObserver(AbstractGameObserverPtr observer) = 0;

    /**
     * @return true if game has ended.
     */
    virtual bool isGameOver() const = 0;
    
    /**
     * @return If isGameOver returns the winner of the game.
     */
    virtual PlayerColor getWinner() const = 0;
    
    /**
     * @return GameConfiguration currently used.
     */
    virtual GameConfigurationPtr getConfiguration() const = 0;

    /**
     * @brief Starts the game logic thread.
     * @see run
     */
    virtual void start() {
        m_thread = std::thread([=]() { run(); });
    }

    /**
     * @brief Will block until the logic thread terminated.
     * Be sure to call stop first to initiate logic thread shutdown.
     */
    virtual void join() {
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }

    /**
     * @brief Initiates a shutdown of the game logic.
     */
    virtual void stop() = 0;

protected:

    /**
     * @brief Actual game logic function.
     * Called by start function on the game logic thread to
     * run the actual logic.
     */
    virtual void run() = 0;

    //! Game logic thread.
    std::thread m_thread;
};

using AbstractGameLogicPtr = std::shared_ptr<AbstractGameLogic>;

#endif // ABSTRACTGAMELOGIC_H
