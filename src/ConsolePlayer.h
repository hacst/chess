#ifndef CONSOLEPLAYER_H
#define CONSOLEPLAYER_H

#include "AbstractPlayer.h"
#include "helper.h"

class ConsolePlayer : public AbstractPlayer, ServiceDispatcherThread {
public:
	virtual void onSetColor(PlayerColor color) override;
	virtual void onGameStart(State state, GameConfiguration config) override;
	virtual std::future<Turn> doMakeTurn(State state) override;
	virtual void onTurnEnd(PlayerColor color, Turn turn, State newState) override;
	virtual void doAbortTurn() override;
	virtual void onGameOver(State state, PlayerColor winner) override;
private:
	PlayerColor m_color;
};

using ConsolePlayerPtr = std::shared_ptr<ConsolePlayer>;

#endif // CONSOLEPLAYER_H
