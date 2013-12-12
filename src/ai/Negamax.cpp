#include "Negamax.h"

Negamax::Negamax() {
	// Empty
}

Score initialScore() {
	return std::numeric_limits<Score>::min();
}

Negamax::Result Negamax::search(GameState state, size_t remainingDepth) {

	if (state.isGameOver() || remainingDepth == 0) {
		return { state.getNextPlayer(), m_evaluator.getScore(state), boost::none };
	}

	Result bestResult { state.getNextPlayer(), initialScore(), boost::none };

	GameState newState;
	for (auto& turn: state.getTurnList()) {
		newState = state; // Copy state for manipulation
		newState.applyTurn(turn);

		Result result = search(newState, remainingDepth - 1);
		result.switchSides();

		if (result > bestResult) {
			bestResult = result;
		}
	}

	assert(bestResult.color == state.getNextPlayer());
	return bestResult;
}


