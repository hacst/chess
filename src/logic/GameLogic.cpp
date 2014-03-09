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
#include <algorithm>

#include "GameLogic.h"

using namespace std;
using namespace std::chrono;
using namespace Logging;

GameLogic::GameLogic(AbstractPlayerPtr white, AbstractPlayerPtr black,
    GameConfigurationPtr config,
    GameState initialGameState)

    : m_tickLength(500)
    , m_abort(false)
    , m_white(white)
    , m_black(black)
    , m_gameState(initialGameState)
    , m_config(config)
    , m_log(initLogger("GameLogic")){
    assert(white != black);

    addObserver(white);
    addObserver(black);
}

GameLogic::~GameLogic() {
    m_abort = true;

    if (m_thread.joinable()) {
        m_thread.join();
    }
}

AbstractPlayerPtr GameLogic::getWhitePlayer() const {
    return m_white;
}

AbstractPlayerPtr GameLogic::getBlackPlayer() const {
    return m_black;
}

void GameLogic::addObserver(AbstractGameObserverPtr observer) {
    m_observers.push_back(observer);
}

GameConfigurationPtr GameLogic::getConfiguration() const {
    return m_config;
}

void GameLogic::stop() {
    LOG(debug) << "Asked to stop game logic";
    m_abort = true;
}

void GameLogic::run() {
    m_white->onSetColor(White);
    m_black->onSetColor(Black);
    assert(m_gameState.getNextPlayer() == White);

    LOG(info) << "Game start";
    notify([&](AbstractGameObserverPtr& obs) {
        obs->onGameStart(m_gameState, *m_config);
    });

    wait(seconds(m_config->timeBetweenTurnsInSeconds));

    while (!isGameOver()) {
        auto& currentPlayer = getCurrentPlayer();
        const PlayerColor currentColor = m_gameState.getNextPlayer();

        LOG(debug) << currentColor << "'s turn";

        notify([&](AbstractGameObserverPtr& obs) {
            obs->onTurnStart(currentColor);
        });

        LOG(trace) << "Asking for turn";
        auto futureTurn = currentPlayer->doMakeTurn(m_gameState);

        Turn turn; // Default pass turn

        const seconds maximumTurnTime(m_config->maximumTurnTimeInSeconds);

        if (!wait_for(futureTurn, maximumTurnTime)) {
            if (isGameOver()) {
                // Game aborted
                LOG(info) << "Game aborted";
                currentPlayer->doAbortTurn();
                break;
            }
            //FIXME: Probably should make other player win
            LOG(debug) << "Turn timed out";
            notify([&](AbstractGameObserverPtr& obs) {
                obs->onTurnTimeout(currentColor, maximumTurnTime);
            });

            currentPlayer->doAbortTurn();
        }
        else {
            turn = futureTurn.get();
        }

        LOG(trace) << currentColor << "'s turn: " << turn;

        m_gameState.applyTurn(turn);

        notify([&](AbstractGameObserverPtr& obs) {
            obs->onTurnEnd(currentColor, turn, m_gameState);
        });

        LOG(debug) << currentColor << " ended its turn";

        // Add in a delay between turns
        wait(seconds(m_config->timeBetweenTurnsInSeconds));
    }

    LOG(info) << "Game over";
    notify([&](AbstractGameObserverPtr& obs) {
        obs->onGameOver(m_gameState, getWinner());
    });
}

void GameLogic::wait(std::chrono::milliseconds waitInMs) const {
    std::chrono::milliseconds waited(0);
    while (waited < waitInMs && !m_abort) {
        const auto waitLeft = waitInMs - waited;
        const std::chrono::milliseconds iterTime = std::min(waitLeft, m_tickLength);

        this_thread::sleep_for(iterTime);

        waited += iterTime;
    }
}

bool GameLogic::isGameOver() const {
    return m_gameState.isGameOver() || m_abort;
}

PlayerColor GameLogic::getWinner() const {
    if (m_abort) return NoPlayer;

    return m_gameState.getWinner();
}

AbstractPlayerPtr& GameLogic::getCurrentPlayer() {
    return (m_gameState.getNextPlayer() == White) ? m_white : m_black;
}

