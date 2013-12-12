#ifndef ABSTRACTEVALUATOR_H
#define ABSTRACTEVALUATOR_H

#include <memory>

class GameState;

using Score = int;

class AbstractEvaluator {
public:
	virtual Score getScore(const GameState& gameState) const = 0;
};

using EvaluatorPtr = std::shared_ptr<AbstractEvaluator>;


#endif // ABSTRACTEVALUATOR_H
