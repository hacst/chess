#include <algorithm>

#include "GameLogic.h"

using namespace std;
using namespace chrono;

GameLogic::GameLogic(AbstractPlayerPtr &white, AbstractPlayerPtr &black)
	: m_white(white)
	, m_black(black) {
	addObserver(white);
	addObserver(black);
}

AbstractPlayerPtr GameLogic::getWhite() const {
	return m_white;
}

AbstractPlayerPtr GameLogic::getBlack() const {
	return m_black;
}

void GameLogic::addObserver(AbstractGameObserverPtr observer) {
	m_observers.push_back(observer);
}

void GameLogic::start() {
	m_white->onSetColor(White);
	m_black->onSetColor(Black);

	notify([&](AbstractGameObserverPtr& obs) {
		obs->onGameStart(m_gameState, m_configuration); });

	m_currentColor = Black;

	while (!isGameOver()) {
		auto& currentPlayer = togglePlayer();

		notify([&](AbstractGameObserverPtr& obs) {
			obs->onTurnStart(m_currentColor); });

		auto futureTurn = currentPlayer->doMakeTurn(m_gameState);

		Turn turn; // Default pass turn

		const seconds maximumTurnTime(m_configuration.maximumTurnTimeInSeconds);
		if (futureTurn.wait_for(maximumTurnTime) == future_status::timeout) {
			notify([&](AbstractGameObserverPtr& obs) {
				obs->onTurnTimeout(m_currentColor, maximumTurnTime); });

			currentPlayer->doAbortTurn();
		} else {
			turn = futureTurn.get();
		}

		m_gameState.apply(turn);

		notify([&](AbstractGameObserverPtr& obs) {
			obs->onTurnEnd(m_currentColor, turn, m_gameState); });
	}

	notify([&](AbstractGameObserverPtr& obs) {
		obs->onGameOver(m_gameState, getWinner()); });
}

bool GameLogic::isGameOver() const {
	return false;
}

PlayerColor GameLogic::getWinner() const {
	return None;
}

AbstractPlayerPtr& GameLogic::currentPlayer() {
	return (m_currentColor == White) ? m_white : m_black;
}

AbstractPlayerPtr& GameLogic::togglePlayer() {
	m_currentColor = (m_currentColor == White) ? Black : White;
	return currentPlayer();
}
