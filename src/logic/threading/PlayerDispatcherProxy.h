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
