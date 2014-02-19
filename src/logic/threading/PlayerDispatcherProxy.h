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
#ifndef PLAYERDISPATCHERPROXY_H
#define PLAYERDISPATCHERPROXY_H

#include "logic/interface/AbstractPlayer.h"
#include "logic/threading/ObserverDispatcherProxy.h"

/**
 * @brief Proxy for transporting AbstractGamePlayer events between threads.
 * 
 * As the GameLogic and other game components run on different threads it
 * is essential to safely transport game events between them. Without any
 * additional precautions AbstractGamePlayer implementations will have their
 * handlers called on the GameLogic thread they are registered on with all
 * implied thread safety concerns.
 * 
 * This proxy will serialize calls coming in from the game logic in a thread-safe
 * way and replay them once its poll method is called in the customers thread.
 */
class PlayerDispatcherProxy : public AbstractPlayer, public ServiceDispatcher {
public:
    PlayerDispatcherProxy(AbstractPlayerPtr player)
        : m_player(player) {
        // Nothing
    }
    
    
    virtual void onSetColor(PlayerColor color) override {
        post([=] {
            m_player->onSetColor(color);
        });
    }
    
    virtual std::future<Turn> doMakeTurn(GameState state) override {
        // To make sure we execute in the right thread use a promise to
        // the proxied player and a blocking get. This ofc means this
        // function might lock up which kinda sucks.
        return postPromise([&] {
            return m_player->doMakeTurn(state);
        }).get();
    }
    
    virtual void doAbortTurn() override {
        post([=] {
            m_player->doAbortTurn();
        });
    }
    
    virtual void onGameStart(GameState state, GameConfiguration config ) override {
        post([=] {
            m_player->onGameStart(state, config);
        });
    }
    
    virtual void onTurnStart(PlayerColor who) override {
        post([=] {
            m_player->onTurnStart(who);
        });
    }
    
    virtual void onTurnEnd(PlayerColor who, Turn turn, GameState newState) override {
        post([=] {
            m_player->onTurnEnd(who, turn, newState);
        });
    }
    
    virtual void onTurnTimeout(PlayerColor who, std::chrono::seconds timeout ) override {
        post([=] {
            m_player->onTurnTimeout(who, timeout);
        });
    }
    
    virtual void onGameOver(GameState state, PlayerColor winner) override {
        post([=] {
            m_player->onGameOver(state, winner);
        });
    }
    
private:
    AbstractPlayerPtr m_player;
};

#endif // PLAYERDISPATCHERPROXY_H
