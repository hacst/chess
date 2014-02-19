/*
    Copyright (c) 2013-2014, Stefan Hacker <dd0t@users.sourceforge.net>

    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

    3. Neither the name of the copyright holder nor the names of its
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.
*/
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <vector>
#include <random>
#include "ai/Negamax.h"
#include "logic/IncrementalMaterialAndPSTEvaluator.h"
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
    Negamax<MockGameOver> negamax;

    MockGameOver mockGameState;
    auto result = negamax.search(mockGameState, 5);
    EXPECT_FALSE(result.turn);
}


TEST(Negamax, searchDepthExhaustion) {
    Negamax<MockGameState> negamax;

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
    Negamax<MockIncreasingState, false, false, false> negamax;
    {
        MockIncreasingState mockGameState;
        MockIncreasingState::increasingScore = 0;

        auto result = negamax.search(mockGameState, 2);
        EXPECT_TRUE(result.turn);

        /*        Search space                   Depth   Next turn color
         *              0                          0           W
         *    1         2           3              1           B
         * 4  5  6   7  8  9   (10) 11 12          2           W
         */
        EXPECT_EQ(10, result.score);
        EXPECT_EQ(pow(3, 1) + pow(3, 2), MockIncreasingState::increasingScore);
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
         *    1         2           3              1           B
         * 4  5 (6)   7  8  9    10 11 12          2           W
         */

        EXPECT_EQ(-6, result.score);
        EXPECT_EQ(pow(3, 1) + pow(3, 2), MockIncreasingState::increasingScore);
    }
}


TEST(Negamax, DISABLED_searchRelaxationUneven) {


    {
        Negamax<MockIncreasingState, false, false, false> negamax;
        MockIncreasingState mockGameState;
        MockIncreasingState::increasingScore = 0;

        auto result = negamax.search(mockGameState, 3);
        EXPECT_TRUE(result.turn);

        EXPECT_EQ(31, result.score);
        EXPECT_EQ(pow(3, 1) + pow(3, 2) + pow(3, 3), MockIncreasingState::increasingScore);
    }
    // Check symetric case

    {
        Negamax<MockIncreasingState, false, false, false> negamax;
        MockIncreasingState mockGameState;
        mockGameState.nextPlayer = Black;
        MockIncreasingState::increasingScore = 0;

        auto result = negamax.search(mockGameState, 3);
        EXPECT_TRUE(result.turn);

        EXPECT_EQ(-11, result.score);
        EXPECT_EQ(pow(3, 1) + pow(3, 2) + pow(3, 3), MockIncreasingState::increasingScore);
    }
}


TEST(Negamax, AlphaBetaCutoff) {
    const unsigned int TRIES = 5;

    mt19937 rng(1234);
    uniform_int_distribution<size_t> depthDist(3, 4);

    for (size_t i = 0; i < TRIES; ++i) {
        Negamax<GameState, false, false, false> negamax;
        Negamax<GameState, true, false, false> negamaxAB;
        
        GameState gs(generateRandomBoard(50, rng));

        const size_t depth = depthDist(rng);
        auto withABCutoff = negamaxAB.search(gs, depth);
        EXPECT_LT(0, negamaxAB.m_counters.cutoffs);
        auto withoutABCutoff = negamax.search(gs, depth);
        EXPECT_EQ(0, negamax.m_counters.cutoffs);

        EXPECT_EQ(withoutABCutoff, withABCutoff)
            << "Base state (" << i << "): " << gs << endl;
    }

}

TEST(Negamax, TranspositionTableWithMoveOrdering) {
    const unsigned int TRIES = 5;
    
    mt19937 rng(3124);
    uniform_int_distribution<size_t> depthDist(3,4);
    for (size_t i = 0; i < TRIES; ++i) {
        Negamax<GameState, true, false, false> negamaxAB;
        Negamax<GameState, true, true, true> negamaxTTMO;
        
        GameState gs(generateRandomBoard(50, rng));
        
        const size_t depth = depthDist(rng);
        auto withTTMO = negamaxTTMO.search(gs, depth);
        auto ttdur = negamaxTTMO.m_counters.duration;
        auto withTTMO2 = negamaxTTMO.search(gs,depth);
        EXPECT_GT(ttdur, negamaxTTMO.m_counters.duration);
        EXPECT_LT(0, negamaxTTMO.m_counters.transpositionTableHits);
        
        auto withoutTTMO = negamaxAB.search(gs, depth);
        EXPECT_GT(negamaxAB.m_counters.duration, negamaxTTMO.m_counters.duration);
        EXPECT_EQ(0, negamaxAB.m_counters.transpositionTableHits);
        
        EXPECT_EQ(withoutTTMO.score, withTTMO.score)
                << "With TT: " << withTTMO << endl
                << "Without TT: " << withoutTTMO << endl
                << "Base state (" << i << "): " << gs << endl
                << "Depth: " << depth << endl;
        
        EXPECT_EQ(withTTMO, withTTMO2)
                << "Base state (" << i << "): " << gs << endl
                << "Depth: " << depth << endl;
    }
}


TEST(Negamax, TranspositionTable) {
    const unsigned int TRIES = 5;
    
    mt19937 rng(3124);
    uniform_int_distribution<size_t> depthDist(3,4);
    for (size_t i = 0; i < TRIES; ++i) {
        Negamax<GameState, true, false, false> negamaxAB;
        Negamax<GameState, true, false, true> negamaxTT;
        
        GameState gs(generateRandomBoard(50, rng));
        
        const size_t depth = depthDist(rng);
        auto withTT = negamaxTT.search(gs, depth);
        auto ttdur = negamaxTT.m_counters.duration;
        auto withTT2 = negamaxTT.search(gs,depth);
        EXPECT_GT(ttdur, negamaxTT.m_counters.duration);
        EXPECT_LT(0, negamaxTT.m_counters.transpositionTableHits);
        
        auto withoutTT = negamaxAB.search(gs, depth);
        EXPECT_GT(negamaxAB.m_counters.duration, negamaxTT.m_counters.duration);
        EXPECT_EQ(0, negamaxAB.m_counters.transpositionTableHits);
        
        EXPECT_EQ(withoutTT, withTT)
                << " Base state (" << i << "): " << gs << endl;
        EXPECT_EQ(withTT, withTT2)
                << "Base state (" << i << "): " << gs << endl;
    }
}


TEST(Negamax, MoveOrdering) {
    const unsigned int TRIES = 5;
    
    mt19937 rng(3124);
    uniform_int_distribution<size_t> depthDist(3,4);
    for (size_t i = 0; i < TRIES; ++i) {
        Negamax<GameState, true, false, false> negamaxAB;
        Negamax<GameState, true, true, false> negamaxMO;
        
        GameState gs(generateRandomBoard(50, rng));
        
        const size_t depth = depthDist(rng);
        auto withMO = negamaxMO.search(gs, depth);        
        auto withoutMO = negamaxAB.search(gs, depth);
        
        EXPECT_EQ(withoutMO.score, withMO.score)
                << "Base state (" << i << "): " << gs << endl;
    }
}

TEST(Negamax, TTMORegression) {
    const int depth = 4;
    //GameState gs(ChessBoard::fromFEN("rNb4N/2p2k1p/6nb/pP3p2/2Q2B2/1P1P3P/P3P1q1/R2K1BR1 b - - 0 25"));
    // Reduced to:
    GameState gs(ChessBoard::fromFEN("8/2p2k1p/8/8/8/8/8/3K1BR1 b - - 0 25"));
    Negamax<GameState, true, false, false> negamaxAB;
    Negamax<GameState, true, true, true> negamaxTTMO;
    auto withTTMO = negamaxTTMO.search(gs, depth);
    auto withoutTTMO = negamaxAB.search(gs, depth);
    
    EXPECT_EQ(withoutTTMO.score, withTTMO.score)
            << "With TT: " << withTTMO << endl
            << "Without TT: " << withoutTTMO << endl
            << "Base state: " << gs << endl
            << "Depth: " << depth << endl;
    
}
