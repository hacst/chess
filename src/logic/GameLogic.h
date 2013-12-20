#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include <vector>
#include <chrono>
#include <future>

#include "GameState.h"
#include "core/GameConfiguration.h"
#include "logic/interface/AbstractGameLogic.h"
#include "misc/helper.h"
#include "core/Logging.h"

/**
 * @brief GameLogic implementation for a game of chess with observers.
 */
class GameLogic : public AbstractGameLogic {
public:
    /**
     * @brief Sets up a GameLogic object for one chess game.
     * @note Don't forget to start operation by calling start.
     * @param white White player reference
     * @param black Black player reference
     * @param config Configuration for this game
     */
    GameLogic(AbstractPlayerPtr white, AbstractPlayerPtr black, GameConfigurationPtr config);
    ~GameLogic();

    virtual AbstractPlayerPtr getWhitePlayer() const override;
    virtual AbstractPlayerPtr getBlackPlayer() const override;

    virtual void addObserver(AbstractGameObserverPtr observer) override;

    virtual bool isGameOver() const override;
    virtual PlayerColor getWinner() const override;
    virtual GameConfigurationPtr getConfiguration() const override;

    virtual void stop() override;

private:
    /**
     * @brief Executes a blocking game loop implementation.
     * Repeatedly asks the white and black player to take turns
     * until one wins the game or another game terminating event
     * occurs. Notifies registered observer of game state changes
     * and handles timeout events.
     */
    virtual void run() override;

    /**
     * @brief Helper function for performing an interruptable wait on a future.
     * Splits up a usually uninterruptable wait on a future into m_tickLength
     * long waits with checks for the m_abort condition.
     * @see std::future<>::wait_for
     * @param fut Future to wait on
     * @param waitInMs Maximum waiting time
     */
    template <typename Future>
    bool wait_for(Future&& fut, std::chrono::milliseconds waitInMs) {
        std::chrono::milliseconds waited(0);
        while (waited < waitInMs && !m_abort) {
            const auto waitLeft = waitInMs - waited;
            const std::chrono::milliseconds iterTime = std::min(waitLeft, m_tickLength);

            if (fut.wait_for(iterTime) == std::future_status::ready) {
                return true;
            }

            waited += iterTime;
        }
        // Future couldn't provide result till timeout
        return false;
    }

    /**
     * @brief Helper function for performing an interruptable wait.
     * Splits up a blocking wait into m_tickLength long waits with
     * checks for the m_abort condition.
     * @param waitInMs Time to wait.
     */
    void wait(std::chrono::milliseconds waitInMs) const;

    /** 
     * @brief Function to call a given function on all attached observers.
     * Basically a observers.map(function).
     * 
     * Usage:   notify([&](AbstractGameObserverPtr& obs) {
     *              // This is your function which is handed obs
     *              obs->someObserverFunction(parameters);
     *          });
     * 
     * @param f Function which takes an AbstractGameObserverPtr as an argument.
     */
    template <typename Function>
    void notify(Function&& f) {
        for (auto& observer : m_observers) {
            f(observer);
        }
    }

    /**
     * @return Returns a reference to the next player to make his turn.
     */
    AbstractPlayerPtr& getCurrentPlayer();

    //! Interval in which the GameLogic should check for aborts (@see m_abort)
    std::chrono::milliseconds m_tickLength;
    //! If true the running game is aborted
    bool m_abort;

    //! List of observers for the game to be notified of game events (contains players)
    std::vector<AbstractGameObserverPtr> m_observers;
    AbstractPlayerPtr m_white;
    AbstractPlayerPtr m_black;
    GameState m_gameState;
    GameConfigurationPtr m_config;
    Logging::Logger m_log;
};

#endif // GAMELOGIC_H
