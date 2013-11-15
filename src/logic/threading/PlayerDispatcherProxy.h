#ifndef PLAYERDISPATCHERPROXY_H
#define PLAYERDISPATCHERPROXY_H

#include "logic/interface/AbstractPlayer.h"
#include "logic/threading/ObserverDispatcherProxy.h"

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
	
	virtual std::future<Turn> doMakeTurn(State state) override {
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
	
	virtual void onGameStart(State state, GameConfiguration config ) override {
		post([=] {
			m_player->onGameStart(state, config);
		});
	}
	
	virtual void onTurnStart(PlayerColor who) override {
		post([=] {
			m_player->onTurnStart(who);
		});
	}
	
	virtual void onTurnEnd(PlayerColor who, Turn turn, State newState) override {
		post([=] {
			m_player->onTurnEnd(who, turn, newState);
		});
	}
	
	virtual void onTurnTimeout(PlayerColor who, std::chrono::seconds timeout ) override {
		post([=] {
			m_player->onTurnTimeout(who, timeout);
		});
	}
	
	virtual void onGameOver(State state, PlayerColor winner) override {
		post([=] {
			m_player->onGameOver(state, winner);
		});
	}
	
private:
	AbstractPlayerPtr m_player;
};

#endif // PLAYERDISPATCHERPROXY_H
