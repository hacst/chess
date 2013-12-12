#ifndef EVALUATORS_H
#define EVALUATORS_H

#include "interface/AbstractEvaluator.h"
#include "logic/ChessTypes.h"

class ChessBoard;

class MaterialEvaluator: public AbstractEvaluator {
public:
	virtual Score getScore(const GameState& gameState) const override;
	virtual Score getMaterialWorth(PlayerColor player, const ChessBoard& board) const;
};

using MaterialEvaluatorPtr = std::shared_ptr<MaterialEvaluator>;


#endif // EVALUATORS_H
