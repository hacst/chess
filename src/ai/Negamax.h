#ifndef NEGAMAX_H
#define NEGAMAX_H

#include <boost/optional/optional.hpp>

#include "logic/GameState.h"
#include "logic/Evaluator.h"

class Negamax {
public:
	Negamax();

	struct Result {
		PlayerColor color;
		Score score;
		boost::optional<Turn> turn;

		void switchSides() {
			color = togglePlayerColor(color);
			score *= -1;
		}

		bool operator<(const Result& other) {
			return score < other.score;
		}

		bool operator>(const Result &other) {
			return score > other.score;
		}
	};

	// Recursive Negamax
	Result search(GameState state, size_t depth);

private:
	Evaluator m_evaluator;
};

#endif // NEGAMAX_H
