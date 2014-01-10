#ifndef NEGAMAX_H
#define NEGAMAX_H

#include <boost/optional/optional.hpp>
#include <string>
#include <sstream>
#include <array>
#include <chrono>

#include "misc/helper.h"
#include "ai/TranspositionTable.h"
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
        Result operator-() const { return{ -score, turn }; }

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
     * @tparam TRANSPOSITION_TABLES_ENABLED If false transposition tables are disabled.
     * @param state Game state to search.
     * @param maxDepthInTurns Number of full turns (ply and return ply) to search.
     * @return Result of the search.
     */
    template<typename TGameState = GameState,
             bool AB_CUTOFF_ENABLED = true,
             bool MOVE_ORDERING_ENABLED = true,
             bool TRANSPOSITION_TABLES_ENABLED = true>
    Result search(const TGameState& state, size_t maxDepth) {
        LOG(Logging::info) << "Starting " << maxDepth
                           << " plies deep search. AB-pruning=" << AB_CUTOFF_ENABLED
                           << ") Move ordering=" << MOVE_ORDERING_ENABLED
                           << " Transposition tables=" << TRANSPOSITION_TABLES_ENABLED;
        auto start = std::chrono::steady_clock::now();

        m_counters = PerfCounters();
        
        Result result = search_recurse<
                TGameState,
                AB_CUTOFF_ENABLED,
                MOVE_ORDERING_ENABLED,
                TRANSPOSITION_TABLES_ENABLED>(
                    state, 0, maxDepth, MIN_SCORE, MAX_SCORE);

        m_counters.duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - start);

        LOG(Logging::debug) << result;
        LOG(Logging::debug) << m_counters;
        return result;
    }

    //! Structure with performance counters used for debugging and evaluation.
    struct PerfCounters {
        PerfCounters()
            : nodes(0), cutoffs(0), updates(0)
            , transpositionTableHits(0), duration() {}
        
        //! Number of nodes searched.
        uint64_t nodes;
        //! Number of branches cut-off using Alpha-Beta.
        uint64_t cutoffs;
        //! Number of best result updates during search.
        uint64_t updates;
        //! Number of transposition table hits during search.
        uint64_t transpositionTableHits;
        //! Time taken for last search
        std::chrono::milliseconds duration;

        std::string toString() const {
            std::stringstream ss;
            const auto ms = duration.count() + 1;
            ss << "PerfCounters:" << std::endl
               << "Search took:     " << ms - 1<< "ms" << std::endl
               << "Nodes visited:   " << nodes << " (~" << nodes / ms << " nodes/ms)" << std::endl
               << "No. of cut offs: " << cutoffs << std::endl
               << "Result updates:  " << updates << std::endl
               << "Tr. Tbl. Hits:   " << transpositionTableHits << std::endl;
            
            return ss.str();
        }
    } m_counters;
    
private:

    /**
     * @brief Recursive Negamax search with optional Alpha-Beta cutoff.
     * @tparam TGameState Type of game state so GameState is mockable.
     * @tparam AB_CUTOFF_ENABLE If false Alpha-Beta cutoff feature is disabled.
     * @tparam MOVE_ORDERING_ENABLED If false move ordering is disabled.
     * @tparam TRANSPOSITION_TABLES_ENABLED If false transposition tables are disabled.
     * @param state Game state to search from.
     * @param depth Depth in plys already searched.
     * @param maxDepth Maximum depth in plys to search.
     * @param alpha Alpha score.
     * @param beta Beta score.
     */
    template <typename TGameState = GameState,
              bool AB_CUTOFF_ENABLED,
              bool MOVE_ORDERING_ENABLED,
              bool TRANSPOSITION_TABLES_ENABLED>
    Result search_recurse(TGameState state, size_t depth, const size_t maxDepth, Score alpha, Score beta) {
        const size_t pliesLeft = maxDepth - depth;
        
        if (state.isGameOver() || pliesLeft == 0) {
            return{ state.getScore(), boost::none };
        }
        
        const Score initialAlpha = alpha;
        
        if (TRANSPOSITION_TABLES_ENABLED) {
            auto tableEntry = m_transpositionTable.lookup(state.getHash());
            if (tableEntry && tableEntry->depth >= pliesLeft) {
                ++m_counters.transpositionTableHits;
                
                // Deep enough to use directly
                if (tableEntry->isExactBound()) {
                    // This is an actual result
                    return { tableEntry->score, tableEntry->turn };
                } else if (tableEntry->isLowerBound()) {
                    // We have a lower bound, adjust alpha accordingly
                    alpha = std::max(alpha, tableEntry->score);
                } else {
                    assert(tableEntry->isUpperBound());
                    // Upper bound, adjust beta
                    beta = std::min(beta, tableEntry->score);
                }
                
                if (alpha > beta) {
                    return { tableEntry->score, tableEntry->turn };
                }
            }
        }

        Result bestResult { MIN_SCORE, boost::none };
        std::vector<std::pair<Turn, TGameState>> options;
        {
            auto possibleTurns = state.getTurnList();
            assert(possibleTurns.size() > 0);
            
            options.reserve(possibleTurns.size());
            
            // Need to apply all states at this point to be able to
            // use their data for move ordering.
            for (auto& turn: possibleTurns) {
                TGameState newState = state;
                state.applyTurn(turn);
                options.emplace_back(std::make_pair(turn, newState));
            }
        }
        
        if (MOVE_ORDERING_ENABLED) {
            performMoveOrdering<TGameState, TRANSPOSITION_TABLES_ENABLED>(options);
        }
        
        for (auto& option: options) {
            Turn& turn = option.first;
            TGameState& newState = option.second;
            
            ++m_counters.nodes;

            Result result = -search_recurse<
                    TGameState,
                    AB_CUTOFF_ENABLED,
                    MOVE_ORDERING_ENABLED,
                    TRANSPOSITION_TABLES_ENABLED>(
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
        
        if (TRANSPOSITION_TABLES_ENABLED) {
            TranspositionTableEntry entry;
            entry.score = bestResult.score;
            entry.turn = *bestResult.turn;
            entry.hash = state.getHash();
            entry.depth = pliesLeft; // Our results comes from pliesLeft deep
    
            if (bestResult.score <= initialAlpha) {
                entry.boundType = TranspositionTableEntry::UPPER;
            } else if (bestResult.score >= beta) {
                entry.boundType = TranspositionTableEntry::LOWER;
            } else {
                entry.boundType = TranspositionTableEntry::EXACT;
            }
            
            m_transpositionTable.update(entry);
        }
        
        //LOG(trace) << "(" << depth << ") Best score " << bestResult.score;
        return bestResult;
    }

    template<typename TGameState, bool TRANSPOSITION_TABLES_ENABLED>
    void performMoveOrdering(std::vector<std::pair<Turn, TGameState>> &options) {
        if (TRANSPOSITION_TABLES_ENABLED) {
            // Use transposition table for ordering
            auto cmp = [this](const std::pair<Turn, TGameState>& a,
                    const std::pair<Turn, TGameState>& b) {
                auto atp = m_transpositionTable.lookup(a.second.getHash());
                auto btp = m_transpositionTable.lookup(b.second.getHash());
                
                if (atp && !btp) return true;
                else if (btp && !atp) return false;
                //TODO: Figure out if depth and type should be considered
                return atp->score < btp->score;
            };
            
            std::sort(begin(options), end(options), cmp);
        } else {
            // Greedily order on score for single move
            auto cmp = [](const std::pair<Turn, TGameState>& a,
                    const std::pair<Turn, TGameState>& b) {
                return a.second.getScore() < b.second.getScore();
            };
            
            std::sort(begin(options), end(options), cmp);
        }
    }
    
    TranspositionTable m_transpositionTable;

    Logging::Logger m_log;
};

#endif // NEGAMAX_H
