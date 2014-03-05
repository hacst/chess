/*
    Copyright (c) 2013-2014, Patrick Hillert <silent@gmx.biz>

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

#ifndef PLAYER_H
#define PLAYER_H

#include "logic/interface/AbstractPlayer.h"
#include "logic/threading/ServiceDispatcherThread.h"
#include "misc/helper.h"
#include "core/Logging.h"

#include "gui/states/GamePlay.h"

#include <random>
#include <thread>
#include <chrono>
#include <iostream>

/**
* @brief Player implementation for a real/human user.
*/
class GUIPlayer : public AbstractPlayer {
public:
    /**
     * @brief Creates a new human player.
     * @param gp The reference to the GamePlay state.
     */
    GUIPlayer(GamePlay& gp)
        : m_log(Logging::initLogger("Player"))
        , m_gameplay(gp) {
    }

    virtual ~GUIPlayer() {
    }

    /**
     * @brief Notifies that player what color he will be playing.
     * Called before onGameStart.
     * @param color Color the player has.
     */
    virtual void onSetColor(PlayerColor color) override {
        LOG(Logging::info) << "Will be playing " << color;
    }

    /**
     * @brief Asks the player to make his turn.
     * @warning This function must not block. It is to return immediatly.
     * The players turn is to be set on the returned future.
     * @note The game logic can abort its request for a player to make
     * his turn using the doAbortTurn function at any time.
     * @param state Current state of the game.
     * @return A future to the turn to make.
     */
    virtual std::future<Turn> doMakeTurn(GameState state) override {
        return m_gameplay.doMakePlayerTurn();
    }

    /**
     * @brief Asks the player to abort a turn asked for with doMakeTurn.
     * When this is called the GameLogic will no longer react to the
     * completion of the future for that turn. A use of this function
     * is the abortion of a turn due to timeout.
     */
    virtual void doAbortTurn() override {
        LOG(Logging::warning) << "Asked to abort turn. This is not implemented";
        m_gameplay.onPlayerAbortTurn();
    }

private:
    //! The boost::Logger
    Logging::Logger m_log;

    //! Reference to the GamePlay state
    GamePlay& m_gameplay;
};

using GUIPlayerPtr = std::shared_ptr<GUIPlayer>;

#endif // PLAYER_H
