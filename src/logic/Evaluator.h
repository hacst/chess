#ifndef EVALUATOR_H
#define EVALUATOR_H

#include "ChessTypes.h"

class GameState;
class ChessBoard;

using Score = int;

class Evaluator {
public:
	Score getScore(const GameState& gameState) const;
	Score getMaterialWorth(PlayerColor player, const ChessBoard& board) const;
};

using EvaluaterPtr = std::shared_ptr<Evaluator>;


#endif // EVALUATOR_H
