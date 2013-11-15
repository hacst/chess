#ifndef ABSTRACTGAMELOGIC_H
#define ABSTRACTGAMELOGIC_H

#include "logic/chesstypes.h"
#include "logic/interface/AbstractPlayer.h"
#include "logic/interface/AbstractGameObserver.h"

class AbstractGameLogic {
public:
	virtual ~AbstractGameLogic() { /* Nothing */ }

	virtual AbstractPlayerPtr getWhite() const = 0;
	virtual AbstractPlayerPtr getBlack() const = 0;

	virtual void addObserver(AbstractGameObserverPtr observer) = 0;

	virtual bool isGameOver() const = 0;
	virtual PlayerColor getWinner() const = 0;
	virtual GameConfiguration getConfiguration() const = 0;

	virtual void start() {
		m_thread = std::thread([=]() { run(); });
	};

	virtual void join() {
		if (m_thread.joinable()) {
			m_thread.join();
		}
	}

	virtual void stop() = 0;

protected:

	virtual void run() = 0;

	std::thread m_thread;
};

using AbstractGameLogicPtr = std::shared_ptr<AbstractGameLogic>;

#endif // ABSTRACTGAMELOGIC_H
