#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include <vector>
#include <chrono>

#include "AbstractGameLogic.h"
#include "helper.h"

class GameLogic : public AbstractGameLogic {
public:
	GameLogic(AbstractPlayerPtr &white, AbstractPlayerPtr &black);

	virtual AbstractPlayerPtr getWhite() const override;
	virtual AbstractPlayerPtr getBlack() const override;

	virtual void addObserver(AbstractGameObserverPtr observer) override;
	virtual void start();

	bool isGameOver() const override;
	PlayerColor getWinner() const override;

private:
	template <typename Function> //TODO: Shouldn't need a template here
	void notify(Function&& f) {
		for (auto& observer : m_observers) {
			f(observer);
		}
	}

	AbstractPlayerPtr& currentPlayer();
	AbstractPlayerPtr& togglePlayer();

	std::vector<AbstractGameObserverPtr> m_observers;
	AbstractPlayerPtr m_white;
	AbstractPlayerPtr m_black;
	State m_gameState;
	PlayerColor m_currentColor;
	GameConfiguration m_configuration;
};

#endif // GAMELOGIC_H
