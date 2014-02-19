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

// Plays the white player only
TEST(Gen_Alg_Eval, PawnOnesidedStandoff) {
    GameState gameState(generateChessBoard({ PoF(Piece(White, Pawn), C3),
        PoF(Piece(Black, Pawn), D6) }));

    Negamax<> negamax;

    vector<Turn> expectedTurns = {
        Turn::move(Piece(White, Pawn), C3, C4),
        Turn::move(Piece(White, Pawn), C4, C5),
        Turn::move(Piece(White, Pawn), C5, D6)
    };

    for (Turn& expectedTurn : expectedTurns) {
        auto result = negamax.search(gameState, 2);
        ASSERT_TRUE(result.turn) << "Turn: " << expectedTurn;

        Turn turn = result.turn.get();
        EXPECT_EQ(expectedTurn, turn) << "Result: " << result;

        /* //DEBUG OUTPUT
                LOG(trace) << gameState << endl;
                for (Turn& option: gameState.getTurnList()) {
                LOG(trace) << "Option: " << option;
                }
                LOG(trace) << "Chose: " << turn;
                */
        gameState.applyTurn(turn);
        gameState.applyTurn(Turn());
    }
}

// Plays the black player only
TEST(Gen_Alg_Eval, PawnOnesidedStandoffReverse) {
    GameState gameState(generateChessBoard({ PoF(Piece(White, Pawn), C3),
        PoF(Piece(Black, Pawn), D6) }));

    Negamax<> negamax;

    vector<Turn> expectedTurns = {
        Turn::move(Piece(Black, Pawn), D6, D5),
        Turn::move(Piece(Black, Pawn), D5, D4),
        Turn::move(Piece(Black, Pawn), D4, C3)
    };

    for (Turn& expectedTurn : expectedTurns) {
        gameState.applyTurn(Turn());

        auto result = negamax.search(gameState, 2);
        ASSERT_TRUE(result.turn) << "Turn: " << expectedTurn;

        Turn turn = result.turn.get();
        EXPECT_EQ(expectedTurn, turn) << "Result: " << result;
        gameState.applyTurn(turn);
    }
}


// Plays the white player only
TEST(Gen_Alg_Eval, KnightStrikesPawn) {
    GameState gameState(generateChessBoard({ PoF(Piece(White, Knight), A1),
        PoF(Piece(Black, Pawn), D4) }));

    Negamax<> negamax;

    vector<Turn> expectedTurns = {
        Turn::move(Piece(White, Knight), A1, B3),
        Turn::move(Piece(White, Knight), B3, D4)
    };

    for (Turn& expectedTurn : expectedTurns) {
        auto result = negamax.search(gameState, 2);
        ASSERT_TRUE(result.turn) << "Turn: " << expectedTurn;

        Turn turn = result.turn.get();
        EXPECT_EQ(expectedTurn, turn) << "Result: " << result;
        gameState.applyTurn(turn);
        gameState.applyTurn(Turn());
    }
}
