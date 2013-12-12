#include <gtest/gtest.h>

#include <vector>

#include "misc/helper.h"
#include "logic/Evaluators.h"
#include "ai/Negamax.h"
#include "logic/GameState.h"

using namespace std;

// Plays the white player only
TEST(Gen_Alg_Eval, PawnOnesidedStandoff) {
	GameState gameState(generateChessBoard(PoF(Piece(White, Pawn), C3),
										   PoF(Piece(Black, Pawn), D6),
										   PoF(Piece(), ERR)));

	Negamax<> negamax(make_shared<MaterialEvaluator>());

	vector<Turn> expectedTurns = {
		Turn::move(Piece(White,Pawn), C3, C4),
		Turn::move(Piece(White,Pawn), C4, C5),
		Turn::move(Piece(White,Pawn), C5, D6)
	};

	for (Turn& expectedTurn: expectedTurns) {
		auto result = negamax.search(gameState, 4);
		ASSERT_TRUE(result.turn);

		Turn turn = result.turn.get();
		EXPECT_EQ(expectedTurn, turn);

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
	GameState gameState(generateChessBoard(PoF(Piece(White, Pawn), C3),
										   PoF(Piece(Black, Pawn), D6),
										   PoF(Piece(), ERR)));

	Negamax<> negamax(make_shared<MaterialEvaluator>());

	vector<Turn> expectedTurns = {
		Turn::move(Piece(Black,Pawn), D6, D5),
		Turn::move(Piece(Black,Pawn), D5, D4),
		Turn::move(Piece(Black,Pawn), D4, C3)
	};

	for (Turn& expectedTurn: expectedTurns) {
		gameState.applyTurn(Turn());

		auto result = negamax.search(gameState, 4);
		ASSERT_TRUE(result.turn);

		Turn turn = result.turn.get();
		EXPECT_EQ(expectedTurn, turn);
		gameState.applyTurn(turn);
	}
}


// Plays the white player only
TEST(Gen_Alg_Eval, KnightStrikesPawn) {
	GameState gameState(generateChessBoard(PoF(Piece(White, Knight), A2),
										   PoF(Piece(Black, Pawn), D4),
										   PoF(Piece(), ERR)));

	Negamax<> negamax(make_shared<MaterialEvaluator>());

	vector<Turn> expectedTurns = {
		Turn::move(Piece(White,Knight), A2, B5),
		Turn::move(Piece(White,Knight), B5, D4)
	};

	for (Turn& expectedTurn: expectedTurns) {
		auto result = negamax.search(gameState, 4);
		ASSERT_TRUE(result.turn);

		Turn turn = result.turn.get();
		EXPECT_EQ(expectedTurn, turn);
		gameState.applyTurn(turn);
		gameState.applyTurn(Turn());
	}
}