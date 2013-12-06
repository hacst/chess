#include <gtest/gtest.h>
#include "logic/GameState.h"

/*

TEST(GameState, defaultState) {
    GameState s;
    EXPECT_EQ(s.getActPlayer(), PlayerColor::White);
    //EXPECT_EQ(s.getChessBoard(), GameState::GameState());
}

TEST(GameState, equality) {
    GameState a, b;
    EXPECT_EQ(a, b);

    Turn t = Turn::move(White, WhitePawn, B4, C4);
    a.applyTurn(t);
    EXPECT_NE(a.getActPlayer(), b.getActPlayer());
    EXPECT_NE(a.getChessBoard(), b.getChessBoard());

    b.applyTurn(t);
    EXPECT_EQ(a.getActPlayer(), b.getActPlayer());
    EXPECT_EQ(a.getChessBoard(), b.getChessBoard());
}

*/
