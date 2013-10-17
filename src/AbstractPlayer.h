#ifndef ABSTRACTPLAYER_H
#define ABSTRACTPLAYER_H

#include <future>
#include "AbstractObserver.h"


class AbstractPlayer : public AbstractGameObserver {
public:
	virtual ~AbstractPlayer() = 0; // Purely virtual

	virtual void onSetColor(PlayerColor /*color*/) { /* Nothing */ }
	virtual std::future<Turn> doMakeTurn(State state) = 0;
	virtual void doAbortTurn() { /* Nothing */ }
};

using AbstractPlayerPtr = std::shared_ptr<AbstractPlayer>;


#endif // ABSTRACTPLAYER_H
