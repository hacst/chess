#include <gtest/gtest.h>
#include "logic/ChessBoard.h"
#include "misc/DebugTools.h"
#include "logic/IncrementalMaterialAndPSTEvaluator.h"

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

TEST(ChessBoard, toFEN) {
    ChessBoard cb;
    EXPECT_EQ("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1", cb.toFEN());
    cb.applyTurn(Turn::move(Piece(White, Pawn), E2, E4));
    EXPECT_EQ("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1", cb.toFEN());
    cb.applyTurn(Turn::move(Piece(Black, Pawn), C7, C5));
    EXPECT_EQ("rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2", cb.toFEN());
}

TEST(ChessBoard, fromFEN) {
    ChessBoard cb;
    EXPECT_EQ(cb, ChessBoard::fromFEN("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"));
    cb.applyTurn(Turn::move(Piece(White, Pawn), E2, E4));
    EXPECT_EQ(cb, ChessBoard::fromFEN("rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1"));
    cb.applyTurn(Turn::move(Piece(Black, Pawn), C7, C5));
    EXPECT_EQ(cb, ChessBoard::fromFEN("rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2"));
}

TEST(ChessBoard, fromFENRoundtrip) {
    const int TRIES = 50;
    mt19937 rng(34234);
    for (int i = 0; i < TRIES; ++i) {
        ChessBoard cb = generateRandomBoard(50, rng);
        ASSERT_EQ(cb, ChessBoard::fromFEN(cb.toFEN())) << cb.toFEN();
    }
}

TEST(ChessBoard, HashOnlyTwoKings) {
    {
        ChessBoard cb = ChessBoard::fromFEN("k7/8/8/8/8/8/8/K7 w - - 0 1");
        EXPECT_EQ(0x6265c813fad05d17, cb.getHash()) << cb;
    }
    {
        ChessBoard cb = ChessBoard::fromFEN("k7/8/8/8/8/8/8/K7 b - - 0 1");
        EXPECT_EQ(0x9ab3eeb955f7d81e, cb.getHash()) << cb;
    }
}

TEST(ChessBoard, HashRookRegression) {
    ChessBoard cb = ChessBoard::fromFEN("7r/8/8/8/8/8/8/8 b - - 0 1");
    EXPECT_EQ(0x8fe88b57305e2ab6, cb.getHash()) << cb;
}


TEST(ChessBoard, HashShortCastle) {
    {
        ChessBoard cb = ChessBoard::fromFEN("4k2r/8/8/8/8/8/8/8 w k - 0 1");
        EXPECT_EQ(0xc1ee59494bab0948, cb.getHash()) << cb;
    }
    {
        ChessBoard cb = ChessBoard::fromFEN("8/8/8/8/8/8/8/4K2R w K - 0 1");
        EXPECT_EQ(0xad7142702be7486b, cb.getHash()) << cb;
    }
}

TEST(ChessBoard, HashLongCastle) {
    {
        ChessBoard cb = ChessBoard::fromFEN("r3k3/8/8/8/8/8/8/K7 w q - 0 1");
        EXPECT_EQ(0x86deb83f13c0ccc7, cb.getHash()) << cb;
    }
    {
        ChessBoard cb = ChessBoard::fromFEN("7k/8/8/8/8/8/8/R3K3 w Q - 0 1");
        EXPECT_EQ(0xe38791deb5785ce5, cb.getHash()) << cb;
    }
}

TEST(ChessBoard, HashEnPassant) {
    {
        ChessBoard cb = ChessBoard::fromFEN("8/8/8/8/6pP/8/8/8 b - h3 0 1");
        EXPECT_EQ(0x230ebdada9634818, cb.getHash()) << cb;
    }
    {
        ChessBoard cb = ChessBoard::fromFEN("8/8/8/pP6/8/8/8/8 b - a6 0 1");
        EXPECT_EQ(0xff6d187f7e0a8caa, cb.getHash()) << cb;
    }
}

TEST(ChessBoard, HashIncrementalEnPassant) {
    ChessBoard cb = ChessBoard::fromFEN("8/8/8/8/6pP/8/8/8 b - h3 0 1");
    cb.applyTurn(Turn::move(Piece(Black, Pawn), G4, G3));
    EXPECT_EQ(IncrementalZobristHasher::hashFullBoard(cb), cb.getHash());
}

TEST(ChessBoard, HashEnPassantSample) {
    {
        ChessBoard cb = ChessBoard::fromFEN("rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPP1PPP/RNBQKBNR w KQkq f6 0 3");
        EXPECT_EQ(0x22a48b5a8e47ff78, cb.getHash());
    }
    {
        ChessBoard cb = ChessBoard::fromFEN("rnbqkbnr/ppp1p1pp/8/3pPp2/8/8/PPPP1PPP/RNBQKBNR w KQkq - 0 3");
        EXPECT_EQ(0xf240c920db53040a, cb.getHash());
    }
}

TEST(ChessBoard, HashPolyglotSequence) {
    ChessBoard cb;
    ASSERT_EQ(0x463b96181691fc9c, cb.getHash());
    
    vector<pair<Turn, IncrementalZobristHasher::Hash>> turnToHashes = {
        { Turn::move(Piece(White, Pawn), E2, E4), 0x823c9b50fd114196 },
        { Turn::move(Piece(Black, Pawn), D7, D5), 0x0756b94461c50fb0 },
        { Turn::move(Piece(White, Pawn), E4, E5), 0x662fafb965db29d4 }
    };
    
    for (auto& tth: turnToHashes) {
        cb.applyTurn(tth.first);
        ASSERT_EQ(tth.second, cb.getHash()) << tth.first << endl << cb;
        ASSERT_EQ(tth.second, IncrementalZobristHasher::hashFullBoard(cb)) << tth.first << endl << cb;
    }
}

TEST(ChessBoard, IncrementalHashing) {
    mt19937 rng(34241);
    const int TRIES = 100;
    for (int i = 0; i < TRIES; ++i) {
        ChessBoard cb = generateRandomBoard(100, rng);
        ASSERT_EQ(IncrementalZobristHasher::hashFullBoard(cb),
            cb.getHash()) << i << "th Board: " << cb;
    }
}

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
        ASSERT_EQ(IncrementalMaterialAndPSTEvaluator::estimateFullBoard(b.getBoard()),
            b.getScore(White)) << i << "th Board: " << b;
    }
}

TEST(ChessBoard, EnPassant_1) {
    ChessBoard cb;
    ASSERT_EQ(ERR, cb.getEnPassantSquare());

    for (File file = A; file < H; file = nextFile(file)) {
        Field fieldFrom = fieldFor(file, Two);
        Field fieldTo = fieldFor(file, Four);
        Field fieldEnPassant = fieldFor(file, Three);
        if (cb.getNextPlayer() == Black) {
            fieldFrom = flipHorizontal(fieldFrom);
            fieldTo = flipHorizontal(fieldTo);
            fieldEnPassant = flipHorizontal(fieldEnPassant);
        }
        Turn turn = Turn::move(Piece(cb.getNextPlayer(), Pawn), fieldFrom, fieldTo);
        cb.applyTurn(turn);
        ASSERT_EQ(fieldEnPassant, cb.getEnPassantSquare()) << turn << endl << cb;
    }
    cb.applyTurn(Turn::move(Piece(cb.getNextPlayer(), Pawn), A4, A5));
    ASSERT_EQ(ERR, cb.getEnPassantSquare());
}

TEST(ChessBoard, EnPassant_2) {
    ChessBoard cb1(generateChessBoard({PoF(Piece(White, Pawn), C2),
                                       PoF(Piece(Black, Pawn), D4)}, White));
    ASSERT_EQ(ERR, cb1.getEnPassantSquare());
    cb1.applyTurn(Turn::move(Piece(cb1.getNextPlayer(), Pawn), C2, C4));
    ASSERT_EQ(cb1.getEnPassantSquare(), C3);
    cb1.applyTurn(Turn::move(Piece(cb1.getNextPlayer(), Pawn), D4, C3));
    ASSERT_EQ(ERR, cb1.getEnPassantSquare());

    ChessBoard cb2(generateChessBoard({PoF(Piece(Black, Pawn), C3)},
                                      cb1.getNextPlayer()));
    EXPECT_EQ(cb2, cb1);
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


TEST(ChessBoard, ScoringPieceSquareTableReadout) {
    // Check whether the scores match for some selected positions
    {
        ChessBoard b = generateChessBoard({
            PoF(Piece(White, Pawn), D7)
        });
        const Score score = 100 + 50;
        EXPECT_EQ(score, b.getScore(White)) << "Board: " << b;
        EXPECT_EQ(-score, b.getScore(Black)) << "Board: " << b;
    }

    {
        ChessBoard b = generateChessBoard({
            PoF(Piece(White, King), G1)
        });
        const Score score = 20000 + 30;
        EXPECT_EQ(score, b.getScore(White)) << "Board: " << b;
        EXPECT_EQ(-score, b.getScore(Black)) << "Board: " << b;
    }

    {
        ChessBoard b = generateChessBoard({
            PoF(Piece(White, Rook), H8)
        });
        const Score score = 500;
        EXPECT_EQ(score, b.getScore(White)) << "Board: " << b;
        EXPECT_EQ(-score, b.getScore(Black)) << "Board: " << b;
    }

    {
        ChessBoard b = generateChessBoard({
            PoF(Piece(White, Queen), A5)
        });
        const Score score = 900 - 5;
        EXPECT_EQ(score, b.getScore(White)) << "Board: " << b;
        EXPECT_EQ(-score, b.getScore(Black)) << "Board: " << b;
    }
}

TEST(ChessBoard, ScoringEvaluationSymmetry) {
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
            ASSERT_EQ(0, b.getScore(White)) << "Board: " << b;
            ASSERT_EQ(0, b.getScore(Black)) << "Board: " << b;
        }

        {
            ChessBoard b = generateChessBoard({
                PoF(Piece(Black, piece), field),
                PoF(Piece(White, piece), flipHorizontal(field))
            });

            // Check PSQ readout and symmetry
            ASSERT_EQ(0, b.getScore(White)) << "Board: " << b;
            ASSERT_EQ(0, b.getScore(Black)) << "Board: " << b;
        }
    }
}

TEST(ChessBoard, HashAndScoreChangeWhenCastlingRegression) {
    ChessBoard cb = ChessBoard::fromFEN("8/8/8/8/8/8/8/4K2R w K - 0 1");
    ASSERT_EQ(0xad7142702be7486b, cb.getHash()) << cb;
    cb.applyTurn(Turn::castle(Piece(White, King), E1, G1));

    ChessBoard cbAfterCastle = ChessBoard::fromFEN("8/8/8/8/8/8/8/5RK1 b - - 1 2");
    
    EXPECT_EQ(cbAfterCastle, cb);
}