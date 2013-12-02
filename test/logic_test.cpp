#include <gtest/gtest.h>
#include "logic/GameState.h"

TEST(GameState, equality) {
    GameState a, b;
    EXPECT_EQ(a, b);

    Turn t = Turn::move(Piece::WhitePawn, Field::B4, Field::C4);
    a.applyTurn(t);
    //EXPECT_NE(a,b);
    EXPECT_NE(a.getNextPlayer(), b.getNextPlayer());
    EXPECT_NE(a.getChessBoard(), b.getChessBoard());

    b.applyTurn(t);
    EXPECT_EQ(a.getNextPlayer(), b.getNextPlayer());
    EXPECT_EQ(a.getChessBoard(), b.getChessBoard());

    Turn t2 = Turn::move(Piece::WhitePawn, Field::B3, Field::C3);
    b.applyTurn(t2);
    EXPECT_NE(a.getNextPlayer(), b.getNextPlayer());
    EXPECT_NE(a.getChessBoard(), b.getChessBoard());
}

TEST(TurnGenerator, calcKingTurns) {
    TurnGenerator tGen;
    BitBoard a = tGen.calcKingTurns(1024, 0);
    LOG(trace) << bitBoardToString(a);

    //BitBoard b = 12345;
    //EXPECT_EQ(a, b);
}


TEST(TurnGenerator, generateBitBoards) {
    TurnGenerator tGen;
    GameState g;

    tGen.generateBitBoards(g.getChessBoard());

    LOG(trace) << bitBoardToString(tGen.m_allPieces[White]);
    LOG(trace) << bitBoardToString(tGen.m_allPieces[Black]);
    LOG(trace) << bitBoardToString(tGen.m_allPieces[None]);

    //LOG(trace) << bitBoardToString(tGen.m_pawns[White]);
    //g.applyTurn(Turn::move(WhitePawn, B3, C3));
    //tGen.generateBitBoards(g.getChessBoard());
    //LOG(trace) << bitBoardToString(tGen.m_pawns[White]);
}


TEST(BitBoard, toString) {
    //BitBoard a = 0;
    //BitBoard b = 2;
    //BitBoard c = 10;
    //LOG(trace) << bitBoardToString(a);
    //LOG(trace) << bitBoardToString(b);
    //LOG(trace) << bitBoardToString(c);
}

/*
TEST(GameState, applyPass) {
    GameState s;
	Turn t;
    GameState sc = s;
    s.applyTurn(t);
	
    EXPECT_EQ(s.getChessBoard(), sc.getChessBoard());
    EXPECT_EQ(s.getNextPlayer(), PlayerColor::Black);
}

TEST(GameState, defaultState) {
    GameState s;
    EXPECT_EQ(s.getNextPlayer(), PlayerColor::White);
    EXPECT_EQ(s.getChessBoard(), GameState::GameState());
}
*/


TEST(Evaluator, getMaterialWorth) {
	ChessBoard board;
	Evaluator eval;
	EXPECT_EQ(0, eval.getMaterialWorth(White,board));
	EXPECT_EQ(0, eval.getMaterialWorth(Black,board));
	
	//TODO: Once a easy way is available to create custom board states finish this
}

TEST(Evaluator, getScore) {
	GameState state;
	Evaluator eval;
	EXPECT_EQ(0, eval.getScore(state));
	
	//TODO: Once a easy way is available to create custom board states finish this
}
