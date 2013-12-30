#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <vector>
#include <random>
#include "ai/Negamax.h"
#include "logic/Evaluators.h"
#include "misc/helper.h"

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
    MOCK_CONST_METHOD1(getScore, Score(const MockGameOver&));
};

TEST(Negamax, searchGameOver) {
    auto mockEval = make_shared<MockGameOverEval>();
    EXPECT_CALL(*mockEval, getScore(_)).Times(1);

    Negamax<decltype(mockEval)> negamax(mockEval);

    MockGameOver mockGameState;
    auto result = negamax.search(mockGameState, 5);
    EXPECT_FALSE(result.turn);
}

struct MockGameStateEval {
    MOCK_CONST_METHOD1(getScore, Score(const MockGameState&));
};

TEST(Negamax, searchDepthExhaustion) {
    auto mockEval = make_shared<MockGameStateEval>();
    EXPECT_CALL(*mockEval, getScore(_)).Times(1);

    Negamax<decltype(mockEval)> negamax(mockEval);

    MockGameState mockGameState;
    auto result = negamax.search(mockGameState, 5);
    EXPECT_TRUE(result.turn);
    EXPECT_EQ(0, result.score);
}

struct MockIncreasingState : public MockGameState {
    MockIncreasingState() : score(-666) {}

    virtual std::vector<Turn> getTurnList() override {
        return{ Turn(), Turn(), Turn() };
    }

    virtual void applyTurn(Turn t) override {
        score = ++increasingScore;
        MockGameState::applyTurn(t);
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

        auto result = negamax.search<MockIncreasingState, false>(mockGameState, 2);
        EXPECT_TRUE(result.turn);

        /*        Search space                   Depth   Next turn color
         *              0                          0           W
         *    1         5           9              1           B
         * 2  3  4   6  7  8   (10) 11 12          2           W
         */
        EXPECT_EQ(10, result.score);
        EXPECT_EQ(pow(3, 1) + pow(3, 2), MockIncreasingState::increasingScore);
    }

    // Check symetric case
    {
        MockIncreasingState mockGameStateBlack;
        mockGameStateBlack.nextPlayer = Black;
        MockIncreasingState::increasingScore = 0;

        auto result = negamax.search<MockIncreasingState, false>(mockGameStateBlack, 2);
        EXPECT_TRUE(result.turn);
        /*        Search space                   Depth   Next turn color
         *              0                          0           W
         *    1         5           9              1           B
         * 2  3 (4)   6  7  8    10 11 12          2           W
         */

        EXPECT_EQ(-4, result.score);
        EXPECT_EQ(pow(3, 1) + pow(3, 2), MockIncreasingState::increasingScore);
    }
}


TEST(Negamax, searchRelaxationUneven) {


    {
        auto mockEval = make_shared<MockIncreasingEval>();
        Negamax<decltype(mockEval)> negamax(mockEval);
        MockIncreasingState mockGameState;
        MockIncreasingState::increasingScore = 0;

        auto result = negamax.search<MockIncreasingState, false>(mockGameState, 3);
        EXPECT_TRUE(result.turn);

        EXPECT_EQ(31, result.score);
        EXPECT_EQ(pow(3, 1) + pow(3, 2) + pow(3, 3), MockIncreasingState::increasingScore);
    }
    // Check symetric case

    {
        auto mockEval = make_shared<MockIncreasingEval>();
        Negamax<decltype(mockEval)> negamax(mockEval);
        MockIncreasingState mockGameState;
        mockGameState.nextPlayer = Black;
        MockIncreasingState::increasingScore = 0;

        auto result = negamax.search<MockIncreasingState, false>(mockGameState, 3);
        EXPECT_TRUE(result.turn);

        EXPECT_EQ(-11, result.score);
        EXPECT_EQ(pow(3, 1) + pow(3, 2) + pow(3, 3), MockIncreasingState::increasingScore);
    }
}



template <typename Rng>
ChessBoard generateRandomBoard(size_t maxTurns, Rng& rng) {
    uniform_int_distribution<size_t> dst(0, maxTurns);
    const size_t turnCount = dst(rng);

    GameState gs;
    for (size_t i = 0; i < turnCount; ++i) {
        auto turns = gs.getTurnList();
        auto turn = random_selection(turns, rng);
        if (turn == end(turns)) break;
        gs.applyTurn(*turn);
    }

    return gs.getChessBoard();
}

TEST(Negamax, AlphaBetaCutoff) {
    const unsigned int TRIES = 3;

    mt19937 rng(1234);
    uniform_int_distribution<size_t> depthDist(2, 3);

    for (size_t i = 0; i < TRIES; ++i) {
        Negamax<> negamax(make_shared<MaterialEvaluator>());
        Negamax<> negamaxAB(make_shared<MaterialEvaluator>());
        
        GameState gs(generateRandomBoard(50, rng));

        const size_t depth = depthDist(rng);
        auto withABCutoff = negamaxAB.search<GameState, true>(gs, depth);
        EXPECT_LT(0, negamaxAB.m_counters.cutoffs);
        auto withoutABCutoff = negamax.search<GameState, false>(gs, depth);
        EXPECT_EQ(0, negamax.m_counters.cutoffs);

        EXPECT_EQ(withoutABCutoff, withABCutoff)
            << "Base state (" << i << "): " << gs << endl;
    }

}

