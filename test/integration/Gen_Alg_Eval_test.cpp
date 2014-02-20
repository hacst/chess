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

#include <vector>

#include "misc/helper.h"
#include "logic/IncrementalMaterialAndPSTEvaluator.h"
#include "ai/Negamax.h"
#include "logic/GameState.h"

using namespace std;

//! Performs a iterative deepening emulation with given state and negamax.
template <typename TNegamax>
void deepen(TNegamax &negamax, GameState &state, size_t depth) {
    for (size_t i = 1; i <= depth; ++i)
        negamax.search(state, depth);
}

// https://github.com/dD0T/chess/issues/34
TEST(Gen_Alg_Eval, MateInTwoRegression34) {
    const GameState origState(ChessBoard::fromFEN("7k/8/1B6/5K2/8/3Q4/8/8 w - - 1 135"));
    GameState gameState = origState;

    Negamax<> white;
    Negamax<> black;

    // Simulate pondering and iterative deepening
    deepen(white, gameState, 5);
    deepen(white, gameState, 3);
    NegamaxResult first = white.search(gameState, 4);
    ASSERT_TRUE(first.turn) << "1" << gameState << first;
    gameState.applyTurn(*first.turn);
    ASSERT_FALSE(gameState.isGameOver()) << "1" << gameState << first;

    deepen(black, gameState, 5);
    deepen(black, gameState, 3);
    NegamaxResult second = black.search(gameState, 4);
    ASSERT_TRUE(second.turn) << "2" << gameState << first;
    gameState.applyTurn(*second.turn);
    ASSERT_FALSE(gameState.isGameOver()) << "2" << gameState << first;
    
    // Simulate pondering and iterative deepening
    deepen(white, gameState, 6);
    deepen(white, gameState, 4);
    NegamaxResult third = white.search(gameState, 5);
    ASSERT_TRUE(third.turn) << "3" << gameState << third;
    gameState.applyTurn(*third.turn);

    ASSERT_TRUE(gameState.isGameOver())
        << origState << "Results: " << first << second << third
        << "Afterwards: " << gameState;

    EXPECT_EQ(White, gameState.getWinner())
        << origState << "Results: " << first << second << third
        << "Afterwards: " << gameState;
}