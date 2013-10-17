#ifndef ABSTRACTGAMELOGIC_H
#define ABSTRACTGAMELOGIC_H

#include "chesstypes.h"
#include "AbstractPlayer.h"
#include "AbstractObserver.h"

class AbstractGameLogic {
public:
	virtual ~AbstractGameLogic() = 0;

	virtual AbstractPlayerPtr getWhite() const = 0;
	virtual AbstractPlayerPtr getBlack() const = 0;

	virtual void addObserver(AbstractGameObserverPtr observer) = 0;
	virtual bool isGameOver() const = 0;
	virtual void start() = 0;
	virtual void stop() = 0;
};

#endif // ABSTRACTGAMELOGIC_H
