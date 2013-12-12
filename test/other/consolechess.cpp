#include <iostream>

#include "misc/helper.h"
#include "logic/Evaluators.h"
#include "ai/Negamax.h"

using namespace std;

int main(void) {
	GameState gameState;
	auto eval = make_shared<MaterialEvaluator>();
	Negamax<> negamax(eval);

	cout << "Initial state" << endl;
	cout << gameState.getChessBoard() << endl;

	for (size_t i = 0; i < 10; ++i) {
		cout << "== " << gameState.getNextPlayer() << " ==" << endl;
		cout << "Calculating turn " << i << "...";
		cout.flush();

		auto result = negamax.search(gameState, 4);
		cout << "Done" << endl;
		cout.flush();
		if (!result.turn)
			break;

		auto turn = result.turn.get();
		cout << "Turn: " << turn << endl;

		gameState.applyTurn(turn);
		cout << gameState.getChessBoard() << endl << endl;
	}
	cout << "No more moves possible" << endl;
	return 0;
}
