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
#ifndef OBSERVERDISPATCHERPROXY_H
#define OBSERVERDISPATCHERPROXY_H

#include "logic/interface/AbstractGameObserver.h"
#include "logic/threading/ServiceDispatcher.h"

/**
 * @brief Proxy for transporting AbstractGameObserver events between threads.
 * 
 * As the GameLogic and other game components run on different threads it
 * is essential to safely transport game events between them. Without any
 * additional precautions AbstractGameObserver implementations will have their
 * handlers called on the GameLogic thread they are registered on with all
 * implied thread safety concerns.
 * 
 * This proxy will serialize calls coming in from the game logic in a thread-safe
 * way and replay them once its poll method is called in the customers thread.
 */
class ObserverDispatcherProxy : public AbstractGameObserver, public ServiceDispatcher {
public:
    ObserverDispatcherProxy(AbstractGameObserverPtr observer)
        : ServiceDispatcher()
        , m_observer(observer) {
        // Nothing
    }
    
    virtual void onGameStart(GameState state, GameConfiguration config ) override {
        post([=] {
            m_observer->onGameStart(state, config);
        });
    }
    
    virtual void onTurnStart(PlayerColor who) override {
        post([=] {
            m_observer->onTurnStart(who);
        });
    }
    
    virtual void onTurnEnd(PlayerColor who, Turn turn, GameState newState) override {
        post([=] {
            m_observer->onTurnEnd(who, turn, newState);
        });
    }
    
    virtual void onTurnTimeout(PlayerColor who, std::chrono::seconds timeout ) override {
        post([=] {
            m_observer->onTurnTimeout(who, timeout);
        });
    }
    
    virtual void onGameOver(GameState state, PlayerColor winner) override {
        post([=] {
            m_observer->onGameOver(state, winner);
        });
    }
    
private:
    AbstractGameObserverPtr m_observer;
};

using ObserverDispatcherProxyPtr = std::shared_ptr<ObserverDispatcherProxy>;

#endif // OBSERVERDISPATCHERPROXY_H
