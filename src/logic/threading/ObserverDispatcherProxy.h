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
