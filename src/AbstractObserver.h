#ifndef ABSTRACTOBSERVER_H
#define ABSTRACTOBSERVER_H

#include <memory>
#include "chesstypes.h"

class AbstractGameObserver {
public:
	virtual ~AbstractGameObserver() = 0;

	virtual void onGameStart(State /*state*/) { /* Nothing */ }
	virtual void onTurn(PlayerColor /*who*/, Turn /*turn*/, State /*newState*/) { /* Nothing */ }
	virtual void onGameOver(State /*state*/, PlayerColor /*winner*/) { /* Nothing */ }
};

using AbstractGameObserverPtr = std::shared_ptr<AbstractGameObserver>;

#endif // ABSTRACTOBSERVER_H
