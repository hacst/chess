#include <gtest/gtest.h>
#include "logic/ChessBoard.h"
#include "misc/DebugTools.h"
#include "logic/Evaluators.h"

using namespace DebugTools;
using namespace std;

/*
TEST(ChessBoard, toString) {
    ChessBoard cb;
    LOG(trace) << cb.toString();
}

TEST(ChessBoard, bitBoardToString) {
    BitBoard bb = generateBitBoard(A1, D8, E1, H4, ERR);
    LOG(trace) << bitBoardToString(bb);
}
TEST(ChessBoard, generateChessBoard) {
    ChessBoard cb(generateChessBoard(PoF(Piece(White, King), D1),
                                     PoF(Piece(White, Pawn), E2),
                                     PoF(Piece(Black, Pawn), D3),
                                     PoF(Piece(), ERR)));
    LOG(trace) << cb.toString();
}
*/



TEST(ChessBoard, equality) {
    ChessBoard cb1, cb2;
    EXPECT_EQ(cb1, cb2);

    Turn t = Turn::move(Piece(White, Pawn), D2, D3);
    cb1.applyTurn(t);
    EXPECT_NE(cb1, cb2);

    cb2.applyTurn(t);
    EXPECT_EQ(cb1, cb2);
}

TEST(ChessBoard, applyTurn) {
    ChessBoard cb1(generateChessBoard({PoF(Piece(White, King), D1),
                                      PoF(Piece(White, Pawn), E2),
                                      PoF(Piece(Black, Pawn), D3)}));
    // black pawn attacks the white pawn
    ChessBoard cb2(generateChessBoard({PoF(Piece(White, King), D1),
                                      PoF(Piece(Black, Pawn), E2)}));
    Turn t = Turn::move(Piece(Black, Pawn), D3, E2);
    cb1.applyTurn(t);

    EXPECT_EQ(cb1, cb2);
}

TEST(ChessBoard, IncrementalScoreEvaluation) {
    mt19937 rng(3421);
    const int TRIES = 50;
    for (int i = 0; i < TRIES; ++i) {
        ChessBoard b = generateRandomBoard(50, rng);
        ASSERT_EQ(IncrementalBoardEvaluator::estimateFullBoard(b.getBoard()),
            b.getScore(White)) << i << "th Board: " << b;
    }
}

TEST(MaterialEvaluator, MigrationCheck) {
    MaterialEvaluator eval;
    mt19937 rng(45438);
    const int TRIES = 50;
    for (int i = 0; i < TRIES; ++i) {
        GameState gs = generateRandomState(50, rng);
        ASSERT_EQ(eval.getScore(gs),
            gs.getScore()) << i << "th State: " << gs;
    }
}