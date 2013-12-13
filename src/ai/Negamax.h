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

    template<typename TGameState = GameState, bool AB_CUTOFF_ENABLED = true>
    Result search(const TGameState& state, size_t maxDepth) {
        return search_recurse<TGameState, AB_CUTOFF_ENABLED>(state, 0, maxDepth,
                                                             MIN_SCORE, MAX_SCORE);
    }

private:

    // Recursive Negamax
    template <typename TGameState = GameState, bool AB_CUTOFF_ENABLED = true>
    Result search_recurse(TGameState state, size_t depth, const size_t maxDepth, Score alpha, Score beta) {

        if (state.isGameOver() || depth == maxDepth) {
            return { m_evaluator->getScore(state) , boost::none };
        }

        Result bestResult { MIN_SCORE, boost::none };
        TGameState newState;
        for (auto& turn: state.getTurnList()) {
            newState = state;

            // Simulate ply
            newState.applyTurn(turn);
            // Get best possible return ply score in maxDepth for adversary
            Result result = search_recurse(newState, depth + 1, maxDepth,
                                           -beta, -max(alpha, bestResult.score));
            // Convert the result to our POV (Zero-Sum)
            result.switchSides();

            // Check if we improved upon previous turns
            if (result > bestResult) {
                //LOG(trace) << "(" << depth << ") Improved score " << bestResult.score << " to " << result.score;
                bestResult = result;
                bestResult.turn = turn;

                if (AB_CUTOFF_ENABLED && bestResult.score >= beta) {
                    // Prune the rest of the sibling branches
                    //LOG(trace) << "(" << depth << ") AB-cutoff " << bestResult.score << " exceeds beta of " << beta;
                    return bestResult;
                }
            }
        }

        //LOG(trace) << "(" << depth << ") Best score " << bestResult.score;
        return bestResult;
    }

    TEvaluatorPtr m_evaluator;
};

#endif // NEGAMAX_H
