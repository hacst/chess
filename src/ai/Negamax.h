#ifndef NEGAMAX_H
#define NEGAMAX_H

#include <boost/optional/optional.hpp>

#include "logic/interface/AbstractEvaluator.h"
#include "logic/GameState.h"

template <typename TEvaluatorPtr = EvaluatorPtr>
class Negamax {
public:
	Negamax(TEvaluatorPtr evaluator)
		: m_evaluator(evaluator) {
		// Empty
	}

	struct Result {
		Score score;
		boost::optional<Turn> turn;

		void switchSides() {
			score *= -1;
		}

		bool operator<(const Result& other) { return score < other.score; }
		bool operator<=(const Result& other) { return score <= other.score; }
		bool operator>=(const Result& other) { return score >= other.score; }
		bool operator>(const Result &other) { return score > other.score; }
	};

	template<typename TGameState = GameState>
	Result search(const TGameState& state, size_t maxDepth) {
		return search_recurse(state, 0, maxDepth);
	}

private:

	// Recursive Negamax
	template <typename TGameState = GameState>
	Result search_recurse(TGameState state, size_t depth, const size_t maxDepth) {

		if (state.isGameOver() || depth == maxDepth) {
			return { m_evaluator->getScore(state) , boost::none };
		}

		Result bestResult { initialScore(), boost::none };
		TGameState newState;
		for (auto& turn: state.getTurnList()) {
			newState = state;

			// Simulate ply
			newState.applyTurn(turn);
			// Get best possible return ply score in maxDepth for adversary
			Result result = search_recurse(newState, depth + 1, maxDepth);
			// Convert the result to our POV (Zero-Sum)
			result.switchSides();

			// Check if we improved upon previous turns
			if (result > bestResult) {
				//LOG(trace) << "(" << depth << ") Improved score " << bestResult.score << " to " << result.score;
				bestResult = result;
				bestResult.turn = turn;
			}
		}

		//LOG(trace) << "(" << depth << ") Best score " << bestResult.score;
		return bestResult;
	}

	static Score initialScore() {
		return std::numeric_limits<Score>::min();
	}

	TEvaluatorPtr m_evaluator;
};

#endif // NEGAMAX_H
