#ifndef CONSOLEPLAYER_H
#define CONSOLEPLAYER_H

#include "logic/interface/AbstractPlayer.h"
#include "logic/threading/ServiceDispatcherThread.h"
#include "misc/helper.h"

class ConsolePlayer : public AbstractPlayer, public ServiceDispatcherThread {
public:
	virtual ~ConsolePlayer();

	virtual void onSetColor(PlayerColor color) override;
    virtual void onGameStart(GameState state, GameConfiguration config) override;
    virtual std::future<Turn> doMakeTurn(GameState state) override;
    virtual void onTurnEnd(PlayerColor color, Turn turn, GameState newState) override;
	virtual void doAbortTurn() override;
    virtual void onGameOver(GameState state, PlayerColor winner) override;
private:
	PlayerColor m_color;
};

using ConsolePlayerPtr = std::shared_ptr<ConsolePlayer>;

#endif // CONSOLEPLAYER_H
