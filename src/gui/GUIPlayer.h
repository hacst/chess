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
* @brief Player implementation for a real user.
*/
class GUIPlayer : public AbstractPlayer {
public:
    GUIPlayer(GamePlay& gp, int seed = 1234)
        : m_seed(seed)
        , m_rng(m_seed)
        , m_msDist(0, 5)
        , m_log(Logging::initLogger("Player"))
        , m_gameplay(gp) {
        LOG(Logging::info) << "Seed: " << m_seed;
    }

    virtual ~GUIPlayer() {
    }

    virtual void onSetColor(PlayerColor color) override {
        LOG(Logging::info) << "(" << m_seed << ") "
            << "Will be playing " << color;
    }

    virtual void doAbortTurn() override {
        LOG(Logging::warning) << "(" << m_seed << ") "
            << "Asked to abort turn. This is not implemented";
        m_gameplay.onPlayerAbortTurn();
    }

    virtual std::future<Turn> doMakeTurn(GameState state) override {
        return m_gameplay.doMakePlayerTurn();

        /*return postPromise([=]() mutable {
            // Always choose a random turn after a random amount of time
            // Obviously can't be aborted
            auto turns = state.getTurnList();
            auto turnIt = random_selection(turns, m_rng);
            Turn turn = (turnIt == std::end(turns)) ? Turn() : (*turnIt);
            int duration = m_msDist(m_rng);

            LOG(Logging::debug) << "(" << m_seed << ") "
            << "Will select " << turn << " after " << duration << " seconds sleep";
            std::this_thread::sleep_for(std::chrono::seconds(duration));
            return turn;

            });*/
    }

private:
    const unsigned int m_seed;
    std::mt19937 m_rng;
    std::uniform_int_distribution<int> m_msDist;
    Logging::Logger m_log;
    GamePlay& m_gameplay;
};

using GUIPlayerPtr = std::shared_ptr<GUIPlayer>;

#endif // PLAYER_H
