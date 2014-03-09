/*
    Copyright (c) 2013-2014, Max Stark <max.stark88@googlemail.com>

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

#include <random>

#include "logic/GameState.h"
#include "logic/IncrementalMaterialAndPSTEvaluator.h"

using namespace std;

TEST(GameState, defaultState) {
    GameState s;
    EXPECT_EQ(PlayerColor::White, s.getNextPlayer());
}

TEST(GameState, equality) {
    GameState a, b;
    EXPECT_EQ(a, b);

    Turn t = Turn::move(Piece(White, Pawn), B2, B4);
    a.applyTurn(t);
    EXPECT_NE(a.getNextPlayer(), b.getNextPlayer());
    EXPECT_NE(a.getChessBoard(), b.getChessBoard());

    b.applyTurn(t);
    EXPECT_EQ(a.getNextPlayer(), b.getNextPlayer());
    EXPECT_EQ(a.getChessBoard(), b.getChessBoard());
}


/* Gameover Detection */
TEST(GameState, gameOverDetection_FiftyMoveRule) {
    GameState gs(ChessBoard::fromFEN("8/k7/8/8/8/8/K7/8 b - - 99 90"));
    ASSERT_FALSE(gs.isGameOver());
    ASSERT_FALSE(gs.isDrawDueTo50MovesRule());

    gs.applyTurn(Turn::move(Piece(Black, King), A7, A6));
    EXPECT_TRUE(gs.isGameOver()) << gs;
    EXPECT_TRUE(gs.isDrawDueTo50MovesRule()) << gs;
    EXPECT_EQ(NoPlayer, gs.getWinner()) << gs;
}
TEST(GameState, gameOverDetection_Checkmate) {
    GameState gs(generateChessBoard({PoF(Piece(White, King), H1),
                                     PoF(Piece(Black, King), A8),
                                     PoF(Piece(White, Rook), A5),
                                     PoF(Piece(White, Rook), B5)}, Black));
    gs.getTurnList();
    EXPECT_TRUE(gs.getChessBoard().getKingInCheck()[Black]);
    EXPECT_TRUE(gs.isGameOver());
    EXPECT_EQ(gs.getWinner(), White);
}
TEST(GameState, gameOverDetection_Stalemate) {
    GameState gs(generateChessBoard({PoF(Piece(White, King), H1),
                                     PoF(Piece(Black, King), A1),
                                     PoF(Piece(Black, Pawn), A2),
                                     PoF(Piece(White, Queen), B3)}, Black));
    gs.getTurnList();
    EXPECT_TRUE(gs.isGameOver());
    EXPECT_EQ(gs.getWinner(), NoPlayer);
}

/* Set Flags correctly on GameState init */
TEST(GameState, setFlagsOnGameStateLoad_KingInCheck_1) {
    GameState gs(ChessBoard::fromFEN("2br4/1pp2p1k/4p2P/3r2K/8/4q3/8/8 w - - 0 40"));

    std::vector<Turn> turns_calc = gs.getTurnList();
    EXPECT_FALSE(turns_calc.empty())
            << gs << turnVecToString(turns_calc);

    EXPECT_TRUE(gs.getChessBoard().getKingInCheck()[White]);
    EXPECT_FALSE(gs.getChessBoard().getKingInCheck()[Black]);

    EXPECT_FALSE(gs.getChessBoard().getCheckmate()[White]);
    EXPECT_FALSE(gs.getChessBoard().getCheckmate()[Black]);

    EXPECT_FALSE(gs.getChessBoard().isGameOver());
    EXPECT_EQ(gs.getChessBoard().getWinner(), NoPlayer);
}
TEST(GameState, setFlagsOnGameStateLoad_KingInCheck_2) {
    GameState gs(generateChessBoard({PoF(Piece(White, King), E1),
                                     PoF(Piece(Black, King), G8),
                                     PoF(Piece(White, Pawn), F7)}, Black));
    std::vector<Turn> turns_calc = gs.getTurnList();
    EXPECT_FALSE(turns_calc.empty())
            << gs << turnVecToString(turns_calc);

    EXPECT_TRUE(gs.getChessBoard().getKingInCheck()[Black]);
    EXPECT_FALSE(gs.getChessBoard().getKingInCheck()[White]);

    EXPECT_FALSE(gs.getChessBoard().getCheckmate()[White]);
    EXPECT_FALSE(gs.getChessBoard().getCheckmate()[Black]);

    EXPECT_FALSE(gs.getChessBoard().isGameOver());
    EXPECT_EQ(gs.getChessBoard().getWinner(), NoPlayer);
}
TEST(GameState, setFlagsOnGameStateLoad_KingInCheck_3) {
    GameState gs(generateChessBoard({PoF(Piece(White, King), E1),
                                     PoF(Piece(Black, King), E8),
                                     PoF(Piece(Black, Queen), C7)}, Black));
    std::vector<Turn> turns_calc = gs.getTurnList();
    EXPECT_FALSE(turns_calc.empty())
            << gs << turnVecToString(turns_calc);

    gs.applyTurn(Turn::move(Piece(Black, Queen), C7, E5));
    EXPECT_TRUE(gs.getChessBoard().getKingInCheck()[White]);

    EXPECT_FALSE(gs.getChessBoard().getCheckmate()[White]);
    EXPECT_FALSE(gs.getChessBoard().getCheckmate()[Black]);

    EXPECT_FALSE(gs.getChessBoard().isGameOver());
    EXPECT_EQ(gs.getChessBoard().getWinner(), NoPlayer);
}
TEST(GameState, setFlagsOnGameStateLoad_Checkmate) {
    GameState gs(ChessBoard::fromFEN("2br4/1pp2p1k/4p2P/3r2K/8/4q3/8/8 b - - 0 39"));

    std::vector<Turn> turns_calc = gs.getTurnList();
    EXPECT_TRUE(turns_calc.empty())
            << gs << turnVecToString(turns_calc);

    EXPECT_TRUE(gs.getChessBoard().getKingInCheck()[White]);
    EXPECT_FALSE(gs.getChessBoard().getKingInCheck()[Black]);

    EXPECT_TRUE(gs.getChessBoard().getCheckmate()[White]);
    EXPECT_FALSE(gs.getChessBoard().getCheckmate()[Black]);

    EXPECT_TRUE(gs.getChessBoard().isGameOver());
    EXPECT_EQ(gs.getChessBoard().getWinner(), Black);
}
TEST(GameState, setFlagsOnGameStateLoad_Stalemate) {
    GameState gs(ChessBoard::fromFEN("8/8/8/8/8/1Q5K/p7/k7 b - - 0 39"));

    std::vector<Turn> turns_calc = gs.getTurnList();
    EXPECT_TRUE(turns_calc.empty())
            << gs << turnVecToString(turns_calc);

    EXPECT_FALSE(gs.getChessBoard().getKingInCheck()[White]);
    EXPECT_FALSE(gs.getChessBoard().getKingInCheck()[Black]);

    EXPECT_FALSE(gs.getChessBoard().getCheckmate()[White]);
    EXPECT_FALSE(gs.getChessBoard().getCheckmate()[Black]);

    EXPECT_TRUE(gs.getChessBoard().isGameOver());
    EXPECT_EQ(gs.getChessBoard().getWinner(), NoPlayer);
}
