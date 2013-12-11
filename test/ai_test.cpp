#include <gtest/gtest.h>
#include "ai/AIPlayer.h"

#include <chrono>
#include <thread>

void sleep_ms(unsigned int ms) {
	std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

TEST(AIPlayer, initialization) {
	AIPlayer player;
	EXPECT_EQ(player.getState(), AIPlayer::STOPPED);
	player.start();
	EXPECT_EQ(player.getState(), AIPlayer::PREPARATION);
	player.onSetColor(PlayerColor::Black);
	GameState state;
	GameConfiguration config;
	EXPECT_EQ(player.getState(), AIPlayer::PREPARATION);
	player.onGameStart(state, config);
	EXPECT_EQ(player.getState(), AIPlayer::PONDERING);

	player.doMakeTurn(state);
	EXPECT_EQ(player.getState(), AIPlayer::PLAYING);
	player.doAbortTurn();
	EXPECT_EQ(player.getState(), AIPlayer::PONDERING);

	player.onGameOver(state, PlayerColor::NoPlayer);
	EXPECT_EQ(player.getState(), AIPlayer::STOPPED);
}
