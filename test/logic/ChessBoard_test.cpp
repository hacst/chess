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

TEST(ChessBoard, MACRO_BB_SET) {
    mt19937 rng(45438);
    uniform_int_distribution<BitBoard> bbDist(1);
    uniform_int_distribution<int> fieldDist(0, 63);
    const int TRIES = 200;
    for (int i = 0; i < TRIES; ++i) {
        Field f = static_cast<Field>(fieldDist(rng));
        BitBoard mask = ~((~(BitBoard)0) << f);
        BitBoard bb = (bbDist(rng) & mask) | BB_SET(f);

        ASSERT_EQ(f, BB_SCAN(bb)) <<
            "From BB: " << bitBoardToString(bb) << endl << "(" << i << ") Raw: " << bb;
    }
}

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
                                      PoF(Piece(Black, Pawn), D3)}, Black));
    // black pawn attacks the white pawn
    ChessBoard cb2(generateChessBoard({PoF(Piece(White, King), D1),
                                      PoF(Piece(Black, Pawn), E2)}, White));
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

TEST(ChessBoard, EnPassant) {
    ChessBoard cb;
    ASSERT_EQ(NoFile, cb.getEnPassantFile());

    for (File file = A; file < H; file = nextFile(file)) {
        Field fieldFrom = fieldFor(file, Two);
        Field fieldTo = fieldFor(file, Four);
        if (cb.getNextPlayer() == Black) {
            fieldFrom = flipHorizontal(fieldFrom);
            fieldTo = flipHorizontal(fieldTo);
        }
        Turn turn = Turn::move(Piece(cb.getNextPlayer(), Pawn), fieldFrom, fieldTo);
        cb.applyTurn(turn);
        ASSERT_EQ(file, cb.getEnPassantFile()) << turn << endl << cb;
    }
    cb.applyTurn(Turn::move(Piece(cb.getNextPlayer(), Pawn), A4, A5));
    ASSERT_EQ(NoFile, cb.getEnPassantFile());
}

TEST(ChessBoard, CastlingRights) {
    {
        ChessBoard cb;
        EXPECT_EQ(true, cb.getShortCastleRights()[White]) << cb;
        EXPECT_EQ(true, cb.getShortCastleRights()[Black]) << cb;
        EXPECT_EQ(true, cb.getLongCastleRights()[White]) << cb;
        EXPECT_EQ(true, cb.getLongCastleRights()[Black]) << cb;
    }

    {
        ChessBoard cb(generateChessBoard({
            PoF(Piece(White, Rook), A1),
            PoF(Piece(White, King), E1),
            PoF(Piece(Black, Rook), H8),
            PoF(Piece(Black, King), E8)
        }));

        EXPECT_EQ(true, cb.getLongCastleRights()[White]) << cb;
        EXPECT_EQ(false, cb.getLongCastleRights()[Black]) << cb;
        EXPECT_EQ(true, cb.getShortCastleRights()[Black]) << cb;
        EXPECT_EQ(false, cb.getShortCastleRights()[White]) << cb;

        Turn turn = Turn::move(Piece(White, King), E1, F2);
        cb.applyTurn(turn);

        EXPECT_EQ(false, cb.getLongCastleRights()[White]) << turn << endl << cb;
        EXPECT_EQ(false, cb.getLongCastleRights()[Black]) << turn << endl << cb;
        EXPECT_EQ(true, cb.getShortCastleRights()[Black]) << turn << endl << cb;
        EXPECT_EQ(false, cb.getShortCastleRights()[White]) << turn << endl << cb;

        turn = Turn::move(Piece(Black, Rook), H8, H1);
        cb.applyTurn(turn);

        EXPECT_EQ(false, cb.getLongCastleRights()[White]) << turn << endl << cb;
        EXPECT_EQ(false, cb.getLongCastleRights()[Black]) << turn << endl << cb;
        EXPECT_EQ(false, cb.getShortCastleRights()[Black]) << turn << endl << cb;
        EXPECT_EQ(false, cb.getShortCastleRights()[White]) << turn << endl << cb;
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
