#ifndef NEGAMAX_H
#define NEGAMAX_H

#include <boost/optional/optional.hpp>
#include <string>
#include <sstream>
#include <array>
#include <chrono>

#include "misc/helper.h"
#include "logic/interface/AbstractEvaluator.h"
#include "logic/GameState.h"
#include "core/Logging.h"

/**
 * @brief Implementation of a Negamax algorithm.
 */
class Negamax {
public:
    /**
     * @brief Creates a new algorithm instance.
     */
    Negamax() : m_log(Logging::initLogger("Negamax")) {
        // Empty
    }

    /**
     * @brief Structure for holding search results.
     */
    struct Result {
        //! Evaluator score estimation for this turn.
        Score score;
        //! Turn to make to advance towards score.
        boost::optional<Turn> turn;

        //! Negates score. Syntax sugar to get closer to algorithm notation.
        Result operator-() const { return{ -score, turn }; };

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
     * @tparam MOVE_ORDERING_ENABLED If false move ordering is disabled.
     * @param state Game state to search.
     * @param maxDepthInTurns Number of full turns (ply and return ply) to search.
     * @return Result of the search.
     */
    template<typename TGameState = GameState, bool AB_CUTOFF_ENABLED = true, bool MOVE_ORDERING_ENABLED = true>
    Result search(const TGameState& state, size_t maxDepth) {
        LOG(Logging::info) << "Starting " << maxDepth << " plies deep search. AB-pruning=" << AB_CUTOFF_ENABLED << ") Move ordering=" << MOVE_ORDERING_ENABLED;
        auto start = std::chrono::monotonic_clock::now();

        m_counters = PerfCounters();
        
        Result result = search_recurse<TGameState, AB_CUTOFF_ENABLED, MOVE_ORDERING_ENABLED>(
                    state, 0, maxDepth, MIN_SCORE, MAX_SCORE);

        m_counters.duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::monotonic_clock::now() - start);

        LOG(Logging::debug) << result;
        LOG(Logging::debug) << m_counters;
        return result;
    }

    //! Structure with performance counters used for debugging and evaluation.
    struct PerfCounters {
        PerfCounters() : nodes(0), cutoffs(0), updates(0), duration() {}
        //! Number of nodes searched.
        uint64_t nodes;
        //! Number of branches cut-off using Alpha-Beta.
        uint64_t cutoffs;
        //! Number of best result updates during search.
        uint64_t updates;
        //! Time taken for last search
        std::chrono::milliseconds duration;

        std::string toString() const {
            std::stringstream ss;
            const auto ms = duration.count();
            ss << "PerfCounters:"
               << "Search took:     " << ms << "ms" << std::endl
               << "Nodes visited:   " << nodes << " (" << nodes / ms << " nodes/ms)" << std::endl
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
     * @tparam MOVE_ORDERING_ENABLED If false move ordering is disabled.
     * @param state Game state to search from.
     * @param depth Depth in plys already searched.
     * @param maxDepth Maximum depth in plys to search.
     * @param alpha Alpha score.
     * @param beta Beta score.
     */
    template <typename TGameState = GameState, bool AB_CUTOFF_ENABLED, bool MOVE_ORDERING_ENABLED>
    Result search_recurse(TGameState state, size_t depth, const size_t maxDepth, Score alpha, Score beta) {
        if (state.isGameOver() || depth == maxDepth) {
            return{ state.getScore(), boost::none };
        }

        Result bestResult { MIN_SCORE, boost::none };
        auto possibleTurns = state.getTurnList();

        if (MOVE_ORDERING_ENABLED) {
            performMoveOrdering(possibleTurns);
        }

        assert(possibleTurns.size() > 0);

        for (auto& turn : possibleTurns) {
            TGameState newState = state;
            ++m_counters.nodes;

            newState.applyTurn(turn);

            Result result = -search_recurse<TGameState, AB_CUTOFF_ENABLED, MOVE_ORDERING_ENABLED>(
                        newState, depth + 1, maxDepth,
                        -beta, -alpha);

            // Check if we improved upon previous turns
            if (result > bestResult) {
                ++m_counters.updates;
                //LOG(trace) << "(" << depth << ") Improved score " << bestResult.score << " to " << result.score;

                bestResult = result;
                bestResult.turn = turn;
            }

            alpha = std::max(alpha, result.score);

            if (AB_CUTOFF_ENABLED && alpha >= beta) {
                ++m_counters.cutoffs;
                // Prune the rest of the sibling branches
                break;
            }
        }

        //LOG(trace) << "(" << depth << ") Best score " << bestResult.score;
        return bestResult;
    }

    void performMoveOrdering(std::vector<Turn>& /*moves*/) {
        //TODO: Implement this
    }

    Logging::Logger m_log;
};

#endif // NEGAMAX_H
