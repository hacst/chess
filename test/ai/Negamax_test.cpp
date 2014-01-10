#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <vector>
#include <random>
#include "ai/Negamax.h"
#include "logic/Evaluators.h"
#include "misc/helper.h"
#include "misc/DebugTools.h"

using namespace std;
using ::testing::_;
using namespace DebugTools;


class MockGameState {
public:
    MockGameState() : nextPlayer(White) {}
    virtual bool isGameOver() { return false; }
    virtual PlayerColor getNextPlayer() const { return nextPlayer; }
    virtual std::vector<Turn> getTurnList() { return std::vector<Turn> { Turn() }; }
    virtual void applyTurn(Turn) { nextPlayer = togglePlayerColor(nextPlayer); }
    virtual Score getScore() const { return 0; }
    virtual Score getHash() const { return 0; }

    PlayerColor nextPlayer;
};

class MockGameOver : public MockGameState {
public:
    virtual bool isGameOver() override { return true; }
    virtual PlayerColor getNextPlayer() const override { return NoPlayer; }
    virtual std::vector<Turn> getTurnList() override { return std::vector<Turn>(); }
    virtual void applyTurn(Turn) override { /* Nothing */ }
};


TEST(Negamax, searchGameOver) {
    Negamax negamax;

    MockGameOver mockGameState;
    auto result = negamax.search(mockGameState, 5);
    EXPECT_FALSE(result.turn);
}


TEST(Negamax, searchDepthExhaustion) {
    Negamax negamax;

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

    virtual Score getScore() const override {
        return (nextPlayer == White) ? score : -score;
    }

    Score score;
    static Score increasingScore;
};

Score MockIncreasingState::increasingScore = 0;


TEST(Negamax, searchRelaxation) {
    Negamax negamax;

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
        Negamax negamax;
        MockIncreasingState mockGameState;
        MockIncreasingState::increasingScore = 0;

        auto result = negamax.search<MockIncreasingState, false>(mockGameState, 3);
        EXPECT_TRUE(result.turn);

        EXPECT_EQ(31, result.score);
        EXPECT_EQ(pow(3, 1) + pow(3, 2) + pow(3, 3), MockIncreasingState::increasingScore);
    }
    // Check symetric case

    {
        Negamax negamax;
        MockIncreasingState mockGameState;
        mockGameState.nextPlayer = Black;
        MockIncreasingState::increasingScore = 0;

        auto result = negamax.search<MockIncreasingState, false>(mockGameState, 3);
        EXPECT_TRUE(result.turn);

        EXPECT_EQ(-11, result.score);
        EXPECT_EQ(pow(3, 1) + pow(3, 2) + pow(3, 3), MockIncreasingState::increasingScore);
    }
}


TEST(Negamax, AlphaBetaCutoff) {
    const unsigned int TRIES = 3;

    mt19937 rng(1234);
    uniform_int_distribution<size_t> depthDist(2, 3);

    for (size_t i = 0; i < TRIES; ++i) {
        Negamax negamax;
        Negamax negamaxAB;
        
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

