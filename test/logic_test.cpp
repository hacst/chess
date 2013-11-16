#include <gtest/gtest.h>

#include "logic/chesstypes.h"

TEST(State, equality) {
	State a,b;
	EXPECT_EQ(a,b);
	a.apply(Turn::move({2,0}, {4,0}));
	EXPECT_NE(a,b);
}

TEST(State, applyPass) {
	State s;
	Turn t;
	State sc = s;
	s.apply(t);
	
	EXPECT_EQ(s.board, sc.board);
	EXPECT_EQ(s.nextTurn, PlayerColor::Black);
}

TEST(State, defaultState) {
	State s;
	EXPECT_EQ(s.nextTurn, PlayerColor::White);
	EXPECT_EQ(s.board, State::Board());
}
