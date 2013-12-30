#include <gtest/gtest.h>

#include <random>

#include "logic/GameState.h"
#include "logic/Evaluators.h"

using namespace std;

/*

TEST(GameState, defaultState) {
    GameState s;
    EXPECT_EQ(s.getNextPlayer(), PlayerColor::White);
    //EXPECT_EQ(s.getChessBoard(), GameState::GameState());
}

TEST(GameState, equality) {
    GameState a, b;
    EXPECT_EQ(a, b);

    Turn t = Turn::move(White, WhitePawn, B4, C4);
    a.applyTurn(t);
    EXPECT_NE(a.getNextPlayer(), b.getNextPlayer());
    EXPECT_NE(a.getChessBoard(), b.getChessBoard());

    b.applyTurn(t);
    EXPECT_EQ(a.getNextPlayer(), b.getNextPlayer());
    EXPECT_EQ(a.getChessBoard(), b.getChessBoard());
}

*/

TEST(MaterialEvaluator, EvaluationSymmetry) {
    MaterialEvaluator eval;

    const unsigned int TRIES = 100;

    mt19937 rng;
    uniform_int_distribution<> fieldDist(0,63);
    uniform_int_distribution<> pieceDist(0,5);

    for (size_t i = 0; i < TRIES; ++i) {
        const Field field = static_cast<Field>(fieldDist(rng));
        const PieceType piece = static_cast<PieceType>(pieceDist(rng));
        {
            ChessBoard b = generateChessBoard({
                PoF(Piece(White, piece), field),
                PoF(Piece(Black, piece), flipHorizontal(field))
            });

            // Check PSQ readout and symmetry
            ASSERT_EQ(0, eval.getMaterialWorth(White, b)) << "Board: " << b;
            ASSERT_EQ(0, eval.getMaterialWorth(Black, b)) << "Board: " << b;
        }

        {
            ChessBoard b = generateChessBoard({
                PoF(Piece(Black, piece), field),
                PoF(Piece(White, piece), flipHorizontal(field))
            });

            // Check PSQ readout and symmetry
            ASSERT_EQ(0, eval.getMaterialWorth(White, b)) << "Board: " << b;
            ASSERT_EQ(0, eval.getMaterialWorth(Black, b)) << "Board: " << b;
        }
    }
}

TEST(MaterialEvaluator, PieceSquareTableReadout) {
    MaterialEvaluator eval;

    // Check whether the scores match for some selected positions
    {
        ChessBoard b = generateChessBoard({
            PoF(Piece(White, Pawn), D7)
        });
        const Score score = 100 + 50;
        EXPECT_EQ(score, eval.getMaterialWorth(White, b)) << "Board: " << b;
        EXPECT_EQ(-score, eval.getMaterialWorth(Black, b)) << "Board: " << b;
    }

    {
        ChessBoard b = generateChessBoard({
            PoF(Piece(White, King), G1)
        });
        const Score score = 20000 + 30;
        EXPECT_EQ(score, eval.getMaterialWorth(White, b)) << "Board: " << b;
        EXPECT_EQ(-score, eval.getMaterialWorth(Black, b)) << "Board: " << b;
    }

    {
        ChessBoard b = generateChessBoard({
            PoF(Piece(White, Rook), H8)
        });
        const Score score = 500;
        EXPECT_EQ(score, eval.getMaterialWorth(White, b)) << "Board: " << b;
        EXPECT_EQ(-score, eval.getMaterialWorth(Black, b)) << "Board: " << b;
    }

    {
        ChessBoard b = generateChessBoard({
            PoF(Piece(White, Queen), A5)
        });
        const Score score = 900 - 5;
        EXPECT_EQ(score, eval.getMaterialWorth(White, b)) << "Board: " << b;
        EXPECT_EQ(-score, eval.getMaterialWorth(Black, b)) << "Board: " << b;
    }
}