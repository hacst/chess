#ifndef NEGAMAX_H
#define NEGAMAX_H

#include <boost/optional/optional.hpp>
#include <string>
#include <sstream>

#include "misc/helper.h"
#include "logic/interface/AbstractEvaluator.h"
#include "logic/GameState.h"
#include "core/Logging.h"

template <typename TEvaluatorPtr = EvaluatorPtr>
class Negamax {
public:
    Negamax(TEvaluatorPtr evaluator)
        : m_evaluator(evaluator)
        , m_log(Logging::initLogger("Negamax")) {
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

        bool operator==(const Result& other) const {
            return score == other.score && turn == other.turn;
        }
        std::string toString() const {
            std::stringstream ss;
            ss << "Result(Score=" << score << ", turn=";
            if (turn) ss << turn.get();
            else ss << "None";
            ss << ")";
            return ss.str();
        }
    };

    template<typename TGameState = GameState, bool AB_CUTOFF_ENABLED = true>
    Result search(const TGameState& state, size_t maxDepth) {
        LOG(Logging::info) << "Starting search of depth " << maxDepth << " AB cutoff=" << AB_CUTOFF_ENABLED;
        m_counters = PerfCounters();
        Result result = search_recurse<TGameState, AB_CUTOFF_ENABLED>(state, 0, maxDepth,
                                                             MIN_SCORE, MAX_SCORE);
        LOG(Logging::debug) << m_counters;
        return result;
    }

    struct PerfCounters {
        PerfCounters() : nodes(0), cutoffs(0), updates(0) {}
        uint64_t nodes;
        uint64_t cutoffs;
        uint64_t updates;

        std::string toString() const {
            std::stringstream ss;
            ss << "Nodes visited:   " << nodes << std::endl
               << "No. of cut offs: " << cutoffs << std::endl
               << "Result updates:  " << updates << std::endl;
            return ss.str();
        }
    } m_counters;
private:

    // Recursive Negamax
    template <typename TGameState = GameState, bool AB_CUTOFF_ENABLED>
    Result search_recurse(TGameState state, size_t depth, const size_t maxDepth, Score alpha, Score beta) {

        if (state.isGameOver() || depth == maxDepth) {
            return { m_evaluator->getScore(state) , boost::none };
        }

        Result bestResult { MIN_SCORE, boost::none };
        TGameState newState;
        for (auto& turn: state.getTurnList()) {
            newState = state;
            ++m_counters.nodes;

            // Simulate ply
            newState.applyTurn(turn);
            // Get best possible return ply score in maxDepth for adversary
            Result result = search_recurse<TGameState, AB_CUTOFF_ENABLED>(
                        newState, depth + 1, maxDepth,
                        -beta, -std::max(alpha, bestResult.score));
            
            // Convert the result to our POV (Zero-Sum)
            result.switchSides();

            // Check if we improved upon previous turns
            if (result > bestResult) {
                ++m_counters.updates;
                //LOG(trace) << "(" << depth << ") Improved score " << bestResult.score << " to " << result.score;
                bestResult = result;
                bestResult.turn = turn;

                if (AB_CUTOFF_ENABLED && bestResult.score >= beta) {
                    ++m_counters.cutoffs;
                    // Prune the rest of the sibling branches
                    return bestResult;
                }
            }
        }

        //LOG(trace) << "(" << depth << ") Best score " << bestResult.score;
        return bestResult;
    }

    TEvaluatorPtr m_evaluator;
    Logging::Logger m_log;
};

#endif // NEGAMAX_H
