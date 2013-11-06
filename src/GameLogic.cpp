#include <algorithm>

#include "GameLogic.h"

using namespace std;
using namespace chrono;

GameLogic::GameLogic(AbstractPlayerPtr white, AbstractPlayerPtr black)
	: m_tickLength(500)
	, m_abort(false)
	, m_white(white)
	, m_black(black) {
	assert(white != black);

	addObserver(white);
	addObserver(black);
}

GameLogic::~GameLogic() {
	m_abort = true;

	if (m_thread.joinable()) {
		m_thread.join();
	}
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

GameConfiguration GameLogic::getConfiguration() const {
	return m_configuration;
}

void GameLogic::stop() {
	LOG(debug) << "Asked to stop game logic";
	m_abort = true;
}

void GameLogic::run() {
	m_white->onSetColor(White);
	m_black->onSetColor(Black);

	LOG(info) << "Game start";
	notify([&](AbstractGameObserverPtr& obs) {
		obs->onGameStart(m_gameState, m_configuration);
	});

	m_currentColor = Black;

	while (!isGameOver()) {
		auto& currentPlayer = togglePlayer();
		LOG(debug) << m_currentColor << "'s turn";

		notify([&](AbstractGameObserverPtr& obs) {
			obs->onTurnStart(m_currentColor);
		});

		LOG(trace) << "Asking for turn";
		auto futureTurn = currentPlayer->doMakeTurn(m_gameState);

		Turn turn; // Default pass turn

		const seconds maximumTurnTime(m_configuration.maximumTurnTimeInSeconds);

		if (!wait_for(futureTurn, maximumTurnTime)) {
			if (isGameOver()) {
				// Game aborted
				LOG(info) << "Game aborted";
				currentPlayer->doAbortTurn();
				break;
			}

			LOG(debug) << "Turn timed out";
			notify([&](AbstractGameObserverPtr& obs) {
				obs->onTurnTimeout(m_currentColor, maximumTurnTime);
			});

			currentPlayer->doAbortTurn();
		}
		else {
			turn = futureTurn.get();
		}

		LOG(trace) << m_currentColor << "'s turn: " << turn;

		m_gameState.apply(turn);

		notify([&](AbstractGameObserverPtr& obs) {
			obs->onTurnEnd(m_currentColor, turn, m_gameState);
		});

		LOG(debug) << m_currentColor << " ended its turn";
	}

	LOG(info) << "Game over";
	notify([&](AbstractGameObserverPtr& obs) {
		obs->onGameOver(m_gameState, getWinner());
	});
}

bool GameLogic::isGameOver() const {
	return false || m_abort;
}

PlayerColor GameLogic::getWinner() const {
	return None;
}

AbstractPlayerPtr& GameLogic::currentPlayer() {
	return (m_currentColor == White)
		? m_white
		: m_black;
}

AbstractPlayerPtr& GameLogic::togglePlayer() {
	m_currentColor = (m_currentColor == White)
		? Black
		: White;

	return currentPlayer();
}
