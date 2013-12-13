#include <gtest/gtest.h>
#include "logic/TurnGenerator.h"

TurnGenerator tGen;



/* TESTING mask and clear functions */
BitBoard bb_calc;
BitBoard bb_fine;

TEST(TurnGenerator, maskRank_One) {
    bb_calc = tGen.maskRank(One);
    bb_fine = generateBitBoard(A1, B1, C1, D1, E1, F1, G1, H1, ERR);
    EXPECT_EQ(bb_calc, bb_fine);
}
TEST(TurnGenerator, maskRank_Four) {
    bb_calc = tGen.maskRank(Four);
    bb_fine = generateBitBoard(A4, B4, C4, D4, E4, F4, G4, H4, ERR);
    EXPECT_EQ(bb_calc, bb_fine);
}
TEST(TurnGenerator, clearRank_One) {
    bb_calc = tGen.clearRank(One);
    bb_fine = ULLONG_MAX - generateBitBoard(A1, B1, C1, D1, E1, F1, G1, H1, ERR);
    EXPECT_EQ(bb_calc, bb_fine);
}
TEST(TurnGenerator, clearRank_Four) {
    bb_calc = tGen.clearRank(Four);
    bb_fine = ULLONG_MAX - generateBitBoard(A4, B4, C4, D4, E4, F4, G4, H4, ERR);
    EXPECT_EQ(bb_calc, bb_fine);
}
TEST(TurnGenerator, maskFile_A) {
    bb_calc = tGen.maskFile(A);
    bb_fine = generateBitBoard(A1, A2, A3, A4, A5, A6, A7, A8, ERR);
    EXPECT_EQ(bb_calc, bb_fine);
}
TEST(TurnGenerator, maskFile_D) {
    bb_calc = tGen.maskFile(D);
    bb_fine = generateBitBoard(D1, D2, D3, D4, D5, D6, D7, D8, ERR);
    EXPECT_EQ(bb_calc, bb_fine);
}
TEST(TurnGenerator, clearFile_A) {
    bb_calc = tGen.clearFile(A);
    bb_fine = ULLONG_MAX - generateBitBoard(A1, A2, A3, A4, A5, A6, A7, A8, ERR);
    EXPECT_EQ(bb_calc, bb_fine);
}
TEST(TurnGenerator, clearFile_D) {
    bb_calc = tGen.clearFile(D);
    bb_fine = ULLONG_MAX - generateBitBoard(D1, D2, D3, D4, D5, D6, D7, D8, ERR);
    EXPECT_EQ(bb_calc, bb_fine);
}



/* TESTING turn generation */
BitBoard bbKing;
BitBoard bbKnights;
BitBoard bbPawns;
// ...
BitBoard bbOwnPieces;
BitBoard bbOppPieces;

BitBoard bbTurns_calc;
BitBoard bbTurns_fine;

/* TESTING calcKingTurns */
TEST(TurnGenerator, calcKingTurns_A1) {
    bbKing = BB_SET(A1);
    bbTurns_calc = tGen.calcKingTurns(bbKing, 0);
    bbTurns_fine = generateBitBoard(B1, B2, A2, ERR);
    EXPECT_EQ(bbTurns_calc, bbTurns_fine);
}
TEST(TurnGenerator, calcKingTurns_H1) {
    bbKing = BB_SET(H1);
    bbTurns_calc = tGen.calcKingTurns(bbKing, 0);
    bbTurns_fine = generateBitBoard(G1, G2, H2, ERR);
    EXPECT_EQ(bbTurns_calc, bbTurns_fine);
}
TEST(TurnGenerator, calcKingTurns_A8) {
    bbKing = BB_SET(A8);
    bbTurns_calc = tGen.calcKingTurns(bbKing, 0);
    bbTurns_fine = generateBitBoard(B8, B7, A7, ERR);
    EXPECT_EQ(bbTurns_calc, bbTurns_fine);
}
TEST(TurnGenerator, calcKingTurns_H8) {
    bbKing = BB_SET(H8);
    bbTurns_calc = tGen.calcKingTurns(bbKing, 0);
    bbTurns_fine = generateBitBoard(G8, G7, H7, ERR);
    EXPECT_EQ(bbTurns_calc, bbTurns_fine);
}
TEST(TurnGenerator, calcKingTurns_C3) {
    bbKing = BB_SET(C3);
    bbTurns_calc = tGen.calcKingTurns(bbKing, 0);
    bbTurns_fine = generateBitBoard(D4, C4, B4, D3, B3, D2, C2, B2, ERR);
    EXPECT_EQ(bbTurns_calc, bbTurns_fine);
}
TEST(TurnGenerator, calcKingTurns_F4) {
    bbKing = BB_SET(F4);
    bbOwnPieces = generateBitBoard(C5, B3, E5, F5, F3, ERR);
    bbTurns_calc = tGen.calcKingTurns(bbKing, bbOwnPieces);
    bbTurns_fine = generateBitBoard(G5, G4, G3, E3, E4, ERR);
    EXPECT_EQ(bbTurns_calc, bbTurns_fine);
}
// TODO: test check and checkmate



/* TESTING calcKnightTurns */
TEST(TurnGenerator, calcKnightTurns_Start) {
    bbKnights = generateBitBoard(B1, G1, ERR);
    bbOwnPieces = tGen.maskRank(Two);
    bbTurns_calc = tGen.calcKnightTurns(bbKnights, bbOwnPieces);
    bbTurns_fine = generateBitBoard(C3, A3, F3, H3, ERR);
    EXPECT_EQ(bbTurns_calc, bbTurns_fine);
}
TEST(TurnGenerator, calcKnightTurns_All) {
    bbKnights = BB_SET(D4);
    bbTurns_calc = tGen.calcKnightTurns(bbKnights, 0);
    bbTurns_fine = generateBitBoard(E6, F5, F3, E2, C2, B3, B5, C6, ERR);
    EXPECT_EQ(bbTurns_calc, bbTurns_fine);
}



/* TESTING calcPawnTurns */
TEST(TurnGenerator, calcPawnTurns_StartWhite) {
    bbPawns = generateBitBoard(A2, B2, C2, D2, E2, F2, G2, H2, ERR);
    bb_calc = tGen.calcPawnTurns(bbPawns, 0, bbPawns, White);
    bb_fine = tGen.maskRank(Three) + tGen.maskRank(Four);
    EXPECT_EQ(bb_calc, bb_fine);
}
TEST(TurnGenerator, calcPawnTurns_StartBlack) {
    bbPawns = generateBitBoard(A7, B7, C7, D7, E7, F7, G7, H7, ERR);
    bb_calc = tGen.calcPawnTurns(bbPawns, 0, bbPawns, Black);
    bb_fine = tGen.maskRank(Six) + tGen.maskRank(Five);
    EXPECT_EQ(bb_calc, bb_fine);
}
TEST(TurnGenerator, calcPawnTurns_AttackWhite_1) {
    bbPawns = BB_SET(A4);
    bbOppPieces = generateBitBoard(A5, B5, ERR);
    bb_calc = tGen.calcPawnTurns(bbPawns, bbOppPieces,
                                 bbPawns | bbOppPieces, White);
    bb_fine = BB_SET(B5);
    EXPECT_EQ(bb_calc, bb_fine);
}
TEST(TurnGenerator, calcPawnTurns_AttackBlack_1) {
    bbPawns = BB_SET(A7);
    bbOppPieces = BB_SET(B6);
    bb_calc = tGen.calcPawnTurns(bbPawns, bbOppPieces,
                                 bbPawns | bbOppPieces, Black);
    bb_fine = generateBitBoard(A6, A5, B6, ERR);
    EXPECT_EQ(bb_calc, bb_fine);
}
TEST(TurnGenerator, calcPawnTurns_AttackWhite_2) {
    bbPawns = BB_SET(H2);
    bbOppPieces = BB_SET(G3);
    bb_calc = tGen.calcPawnTurns(bbPawns, bbOppPieces,
                                 bbPawns | bbOppPieces, White);
    bb_fine = generateBitBoard(G3, H3, H4, ERR);
    EXPECT_EQ(bb_calc, bb_fine);
}
TEST(TurnGenerator, calcPawnTurns_AttackBlack_2) {
    bbPawns = BB_SET(H5);
    bbOppPieces = generateBitBoard(H4, G4, ERR);
    bb_calc = tGen.calcPawnTurns(bbPawns, bbOppPieces,
                                 bbPawns | bbOppPieces, Black);
    bb_fine = BB_SET(G4);
    EXPECT_EQ(bb_calc, bb_fine);
}
TEST(TurnGenerator, calcPawnTurns_AttackWhite_3) {
    bbPawns = BB_SET(D2);
    bbOppPieces = generateBitBoard(C3, E3, ERR);
    bb_calc = tGen.calcPawnTurns(bbPawns, bbOppPieces,
                                 bbPawns | bbOppPieces, White);
    bb_fine = generateBitBoard(C3, E3, D3, D4, ERR);
    EXPECT_EQ(bb_calc, bb_fine);
}
TEST(TurnGenerator, calcPawnTurns_AttackBlack_3) {
    bbPawns = BB_SET(C5);
    bbOppPieces = BB_SET(D4);
    bb_calc = tGen.calcPawnTurns(bbPawns, bbOppPieces,
                                 bbPawns | bbOppPieces, Black);
    bb_fine = generateBitBoard(D4, C4, ERR);
    EXPECT_EQ(bb_calc, bb_fine);
}
// TODO: test en passant



/* TESTING calcQueenTurns */
/* TESTING calcBishopTurns */
/* TESTING calcRookTurns */

/*
LOG(trace) << bitBoardToString(bb_calc);
LOG(trace) << bitBoardToString(bb_fine);
*/



/* TESTING the generated turns */
std::vector<Turn> turns_calc;
std::vector<Turn> turns_fine;

TEST(TurnGenerator, generateTurns_1) {
    ChessBoard cb(generateChessBoard(PoF(Piece(White, King), E1),
                                     PoF(Piece(White, Pawn), E2),
                                     PoF(Piece(Black, Pawn), F2),
                                     PoF(Piece(), ERR)));
    turns_calc = tGen.generateTurns(White, cb);

    turns_fine.clear();
    turns_fine.push_back(Turn::move(Piece(White, King), E1, F2));
    turns_fine.push_back(Turn::move(Piece(White, King), E1, D2));
    turns_fine.push_back(Turn::move(Piece(White, King), E1, F1));
    turns_fine.push_back(Turn::move(Piece(White, King), E1, D1));
    turns_fine.push_back(Turn::move(Piece(White, Pawn), E2, E3));
    turns_fine.push_back(Turn::move(Piece(White, Pawn), E2, E4));

    // TODO: FALSCH! Koenig steht im Schach und daher sind die
    // gueltigen Turns nur:
    //turns_fine.push_back(Turn::move(Piece(White, King), E1, F2));
    //turns_fine.push_back(Turn::move(Piece(White, King), E1, D2));
    //turns_fine.push_back(Turn::move(Piece(White, King), E1, F1));
    //turns_fine.push_back(Turn::move(Piece(White, King), E1, D1));

    EXPECT_EQ(turnVecCompare(turns_calc, turns_fine), true);
}
TEST(TurnGenerator, generateTurns_2) {
    ChessBoard cb(generateChessBoard(PoF(Piece(White, King), E1),
                                     PoF(Piece(White, Pawn), E2),
                                     PoF(Piece(White, Pawn), F2),
                                     PoF(Piece(), ERR)));
    turns_calc = tGen.generateTurns(White, cb);

    turns_fine.clear();
    turns_fine.push_back(Turn::move(Piece(White, King), E1, D2));
    turns_fine.push_back(Turn::move(Piece(White, King), E1, F1));
    turns_fine.push_back(Turn::move(Piece(White, King), E1, D1));
    turns_fine.push_back(Turn::move(Piece(White, Pawn), E2, E3));
    turns_fine.push_back(Turn::move(Piece(White, Pawn), E2, E4));
    turns_fine.push_back(Turn::move(Piece(White, Pawn), F2, F3));
    turns_fine.push_back(Turn::move(Piece(White, Pawn), F2, F4));

    EXPECT_EQ(turnVecCompare(turns_calc, turns_fine), true);


    //LOG(trace) << cb;
    //LOG(trace) << turnVecToString(turns_calc);
}


TEST(TurnGenerator, generateTurns_3) {
    ChessBoard cb(generateChessBoard(PoF(Piece(White, Pawn), C5),
                                     PoF(Piece(White, Pawn), D6),
                                     PoF(Piece(), ERR)));
    turns_calc = tGen.generateTurns(White, cb);

    turns_fine.clear();
    turns_fine.push_back(Turn::move(Piece(White, Pawn), C5, C6));
    turns_fine.push_back(Turn::move(Piece(White, Pawn), D6, D7));

    EXPECT_TRUE(turnVecCompare(turns_calc, turns_fine));

    LOG(trace) << cb;
    LOG(trace) << turnVecToString(turns_calc);

}


/*
TEST(TurnGenerator, calcPawnTurns_AttackWhite_5) {
    bbPawns = generateBitBoard(F4, G5, ERR);
    bbOppPieces = 0; //generateBitBoard(D6, ERR);
    bb_calc = tGen.calcPawnTurns(bbPawns, bbOppPieces,
                                 bbPawns | bbOppPieces, White);
    bb_fine = generateBitBoard(F5, G6, ERR);
    EXPECT_EQ(bb_calc, bb_fine);

    LOG(trace) << bitBoardToString(bb_calc);
    LOG(trace) << bitBoardToString(bb_fine);
}

TEST(TurnGenerator, calcPawnTurns_AttackWhite_6) {
    bbPawns = generateBitBoard(F4, ERR);
    bbOppPieces = generateBitBoard(G5, ERR);
    bb_calc = tGen.calcPawnTurns(bbPawns, bbOppPieces,
                                 bbPawns | bbOppPieces, White);
    bb_fine = generateBitBoard(F5, G5, ERR);
    EXPECT_EQ(bb_calc, bb_fine);

    LOG(trace) << bitBoardToString(bb_calc);
    LOG(trace) << bitBoardToString(bb_fine);
}
*/

/*
LOG(trace) << cb;
LOG(trace) << turnVecToString(turns_calc);
*/
