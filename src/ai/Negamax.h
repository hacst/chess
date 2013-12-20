#ifndef NEGAMAX_H
#define NEGAMAX_H

#include <boost/optional/optional.hpp>
#include <string>
#include <sstream>

#include "misc/helper.h"
#include "logic/interface/AbstractEvaluator.h"
#include "logic/GameState.h"
#include "core/Logging.h"

/**
 * @brief Implementation of a Negamax algorithm.
 */
template <typename TEvaluatorPtr = EvaluatorPtr>
class Negamax {
public:
    /**
     * @brief Creates a new algorithm instance.
     * @param evaluator Evaluator to use in search.
     */
    Negamax(TEvaluatorPtr evaluator)
        : m_evaluator(evaluator)
        , m_log(Logging::initLogger("Negamax")) {
    }

    /**
     * @brief Structure for holding search results.
     */
    struct Result {
        //! Evaluator score estimation for this turn.
        Score score;
        //! Turn to make to advance towards score.
        boost::optional<Turn> turn;

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

    /**
     * @brief Search given state up to maxDepth full turns.
     * @tparam TGameState Type of game state so GameState is mockable.
     * @tparam AB_CUTOFF_ENABLE If false Alpha-Beta cutoff feature is disabled.
     * @param state Game state to search.
     * @param maxDepthInTurns Number of full turns (ply and return ply) to search.
     * @return Result of the search.
     */
    template<typename TGameState = GameState, bool AB_CUTOFF_ENABLED = true>
    Result search(const TGameState& state, size_t maxDepthInTurns) {
        LOG(Logging::info) << "Starting search of depth " << maxDepthInTurns << " turns. AB cutoff = " << AB_CUTOFF_ENABLED;
        
        m_counters = PerfCounters();
        
        Result result = search_recurse<TGameState, AB_CUTOFF_ENABLED>(
                    state, 0, maxDepthInTurns * 2, MIN_SCORE, MAX_SCORE);
        
        LOG(Logging::debug) << m_counters;
        return result;
    }

    //! Structure with performance counters used for debugging and evaluation.
    struct PerfCounters {
        PerfCounters() : nodes(0), cutoffs(0), updates(0) {}
        //! Number of nodes searched.
        uint64_t nodes;
        //! Number of branches cut-off using Alpha-Beta.
        uint64_t cutoffs;
        //! Number of best result updates during search.
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

    /**
     * @brief Recursive Negamax search with optional Alpha-Beta cutoff.
     * @tparam TGameState Type of game state so GameState is mockable.
     * @tparam AB_CUTOFF_ENABLE If false Alpha-Beta cutoff feature is disabled.
     * @param state Game state to search from.
     * @param depth Depth in plys already searched.
     * @param maxDepth Maximum depth in plys to search.
     * @param alpha Alpha score.
     * @param beta Beta score.
     */
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
            result.score *= -1;

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
