#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <vector>
#include "ai/Negamax.h"
#include "logic/Evaluators.h"

using namespace std;
using ::testing::_;


class MockGameState {
public:
	MockGameState() : nextPlayer(White) {}
	virtual bool isGameOver() { return false; }
	virtual PlayerColor getNextPlayer() const { return nextPlayer; }
	virtual std::vector<Turn> getTurnList() { return std::vector<Turn> { Turn() }; }
	virtual void applyTurn(Turn) { nextPlayer = togglePlayerColor(nextPlayer); }

	PlayerColor nextPlayer;
};

class MockGameOver : public MockGameState {
public:
	virtual bool isGameOver() override { return true; }
	virtual PlayerColor getNextPlayer() const override { return NoPlayer; }
	virtual std::vector<Turn> getTurnList() override { return std::vector<Turn>(); }
	virtual void applyTurn(Turn) override { /* Nothing */ }
};


struct MockGameOverEval {
	MOCK_CONST_METHOD1(getScore, Score (const MockGameOver&));
};

TEST(Negamax, searchGameOver) {
	auto mockEval = make_shared<MockGameOverEval>();
	EXPECT_CALL(*mockEval, getScore(_)).Times(1);

	Negamax<decltype(mockEval)> negamax(mockEval);

	MockGameOver mockGameState;
	auto result = negamax.search(mockGameState, 10);
	EXPECT_FALSE(result.turn);
}

struct MockGameStateEval {
	MOCK_CONST_METHOD1(getScore, Score (const MockGameState&));
};

TEST(Negamax, searchDepthExhaustion) {
	auto mockEval = make_shared<MockGameStateEval>();
	EXPECT_CALL(*mockEval, getScore(_)).Times(1);

	Negamax<decltype(mockEval)> negamax(mockEval);

	MockGameState mockGameState;
	auto result = negamax.search(mockGameState, 10);
	EXPECT_TRUE(result.turn);
	EXPECT_EQ(0, result.score);
}

struct MockIncreasingState : public MockGameState {
	MockIncreasingState() : score(-666) {}

	virtual std::vector<Turn> getTurnList() override {
		return {Turn(), Turn(), Turn() };
	}

	virtual void applyTurn(Turn) override {
		score = ++increasingScore;
	}

	Score score;
	static Score increasingScore;
};

Score MockIncreasingState::increasingScore = 0;

struct MockIncreasingEval {
	Score getScore(const MockIncreasingState& state) {
		return (state.nextPlayer == White) ? state.score : -state.score;
	}
};

TEST(Negamax, searchRelaxation) {
	auto mockEval = make_shared<MockIncreasingEval>();
	Negamax<decltype(mockEval)> negamax(mockEval);

	{
		MockIncreasingState mockGameState;
		MockIncreasingState::increasingScore = 0;

		auto result = negamax.search(mockGameState, 2);
		EXPECT_TRUE(result.turn);

		/*        Search space                   Depth   Next turn color
		 *              0                          0           W
		 *    1         5           9              1           B
		 * 2  3  4   6  7  8   (10) 11 12          2           W
		 */
		EXPECT_EQ(10, result.score);
		EXPECT_EQ(pow(3,1) + pow(3,2), MockIncreasingState::increasingScore);
	}

	// Check symetric case
	{
		MockIncreasingState mockGameStateBlack;
		mockGameStateBlack.nextPlayer = Black;
		MockIncreasingState::increasingScore = 0;

		auto result = negamax.search(mockGameStateBlack, 2);
		EXPECT_TRUE(result.turn);
		/*        Search space                   Depth   Next turn color
		 *              0                          0           W
		 *    1         5           9              1           B
		 * 2  3 (4)   6  7  8    10 11 12          2           W
		 */

		EXPECT_EQ(-4, result.score);
		EXPECT_EQ(pow(3,1) + pow(3,2), MockIncreasingState::increasingScore);
	}
}


TEST(Negamax, DISABLED_searchRelaxationUneven) {
	auto mockEval = make_shared<MockIncreasingEval>();
	Negamax<decltype(mockEval)> negamax(mockEval);

	MockIncreasingState mockGameState;
	MockIncreasingState::increasingScore = 0;

	auto result = negamax.search(mockGameState, 3);
	EXPECT_TRUE(result.turn);

	//TODO: Why does it try to play blacks perspective???
	EXPECT_EQ(28, result.score);
	EXPECT_EQ(pow(3,1) + pow(3,2) + pow(3,3), MockIncreasingState::increasingScore);
}

