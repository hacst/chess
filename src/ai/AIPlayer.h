#ifndef AIPLAYER_H
#define AIPLAYER_H

#include <boost/log/sources/logger.hpp>
#include "logic/interface/AbstractPlayer.h"
#include "ai/Negamax.h"

class AIPlayer: public AbstractPlayer {
public:
	AIPlayer();
	~AIPlayer();

	void start();

	virtual void onSetColor(PlayerColor color) override;
	virtual void onGameStart(GameState state, GameConfiguration config) override;
	virtual std::future<Turn> doMakeTurn(GameState state) override;
	virtual void doAbortTurn() override;
	virtual void onGameOver(GameState, PlayerColor) override;

	enum States {
		PREPARATION, // Game preparation phase
		PONDERING, // Pondering during enemies turn
		PLAYING, // Playing own turn
		STOPPED
	};

	States getState() const;

private:
	void run();

	void play();
	void ponder();

	void changeState(States newState);

	std::promise<Turn> m_promisedTurn;

	States m_playerState;

	GameState m_gameState;
	GameConfiguration m_gameConfig;

	PlayerColor m_color;
	boost::log::sources::logger m_log;
	Negamax<> m_algorithm;
	std::thread m_thread;
};

namespace std {
std::ostream& operator <<(std::ostream& stream, const AIPlayer::States s);
}

using AIPlayerPtr = std::shared_ptr<AIPlayer>;

#endif // AIPLAYER_H
