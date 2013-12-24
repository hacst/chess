#ifndef ABSTRACTEVALUATOR_H
#define ABSTRACTEVALUATOR_H

#include <memory>
#include <limits>

class GameState;

using Score = int;
// Provide MAX_SCORE/MIN_SCORE to ensure that both are negatable to reach each other
static const Score MAX_SCORE = std::numeric_limits<Score>::max();
static const Score MIN_SCORE = -std::numeric_limits<Score>::max();

class AbstractEvaluator {
public:
    virtual Score getScore(const GameState& gameState) const = 0;
};

using EvaluatorPtr = std::shared_ptr<AbstractEvaluator>;


#endif // ABSTRACTEVALUATOR_H
