/*
    Copyright (c) 2013-2014, Max Stark <max.stark88@googlemail.com>

    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

    3. Neither the name of the copyright holder nor the names of its
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.
*/
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
     * @param initialGameState Initial state when starting the game
     */
    GameLogic(AbstractPlayerPtr white, AbstractPlayerPtr black,
        GameConfigurationPtr config,
        GameState initialGameState = GameState());

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
