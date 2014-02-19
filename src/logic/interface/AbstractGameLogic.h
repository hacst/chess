/*
    Copyright (c) 2013-2014, Stefan Hacker <dd0t@users.sourceforge.net>

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
