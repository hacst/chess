#ifndef OBSERVERDISPATCHERPROXY_H
#define OBSERVERDISPATCHERPROXY_H

#include "AbstractGameObserver.h"
#include "ServiceDispatcher.h"

class ObserverDispatcherProxy : public AbstractGameObserver, public ServiceDispatcher {
public:
	ObserverDispatcherProxy(AbstractGameObserverPtr observer)
	    : ServiceDispatcher()
	    , m_observer(observer) {
		// Nothing
	}
	
	virtual void onGameStart(State state, GameConfiguration config ) override {
		post([=] {
			m_observer->onGameStart(state, config);
		});
	}
	
	virtual void onTurnStart(PlayerColor who) override {
		post([=] {
			m_observer->onTurnStart(who);
		});
	}
	
	virtual void onTurnEnd(PlayerColor who, Turn turn, State newState) override {
		post([=] {
			m_observer->onTurnEnd(who, turn, newState);
		});
	}
	
	virtual void onTurnTimeout(PlayerColor who, std::chrono::seconds timeout ) override {
		post([=] {
			m_observer->onTurnTimeout(who, timeout);
		});
	}
	
	virtual void onGameOver(State state, PlayerColor winner) override {
		post([=] {
			m_observer->onGameOver(state, winner);
		});
	}
	
private:
	AbstractGameObserverPtr m_observer;
};

#endif // OBSERVERDISPATCHERPROXY_H
