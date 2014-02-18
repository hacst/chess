#ifndef NEGAMAX_H
#define NEGAMAX_H

#include <boost/optional/optional.hpp>
#include <string>
#include <sstream>
#include <array>
#include <chrono>
#include <atomic>

#include "misc/helper.h"
#include "ai/TranspositionTable.h"
#include "logic/GameState.h"
#include "core/Logging.h"

/**
 * @brief Structure for holding search results.
 */
struct NegamaxResult {
    //! Evaluator score estimation for this turn.
    Score score;
    //! Turn to make to advance towards score.
    boost::optional<Turn> turn;

    //! Negates score. Syntax sugar to get closer to algorithm notation.
    NegamaxResult operator-() const { return{ -score, turn }; }

    bool operator<(const NegamaxResult& other) { return score < other.score; }
    bool operator<=(const NegamaxResult& other) { return score <= other.score; }
    bool operator>=(const NegamaxResult& other) { return score >= other.score; }
    bool operator>(const NegamaxResult &other) { return score > other.score; }

    bool operator==(const NegamaxResult& other) const {
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
 * @brief Implementation of a Negamax algorithm.
 * @tparam TGameState Type of game state so GameState is mockable.
 * @tparam AB_CUTOFF_ENABLE If false Alpha-Beta cutoff feature is disabled.
 * @tparam MOVE_ORDERING_ENABLED If false move ordering is disabled.
 * @tparam TRANSPOSITION_TABLES_ENABLED If false transposition tables are disabled.
 */
template<typename TGameState = GameState,
         bool AB_CUTOFF_ENABLED = true,
         bool MOVE_ORDERING_ENABLED = true,
         bool TRANSPOSITION_TABLES_ENABLED = true>
class Negamax {
public:
    /**
     * @brief Creates a new algorithm instance.
     */
    Negamax()
        : m_transpositionTable()
        , m_abort(false)
        , m_log(Logging::initLogger("Negamax")) {
        // Empty
    }

    /**
     * @brief Search given state up to maxDepth full turns.
     * @param state Game state to search.
     * @param maxDepthInTurns Number of full turns (ply and return ply) to search.
     * @return Result of the search.
     */
    NegamaxResult search(const TGameState& state, size_t maxDepth) {
        m_abort = false;

        LOG(Logging::info) << "Starting " << maxDepth
                           << " plies deep search. AB-pruning=" << AB_CUTOFF_ENABLED
                           << " Move ordering=" << MOVE_ORDERING_ENABLED
                           << " Transposition tables=" << TRANSPOSITION_TABLES_ENABLED;
        
        auto start = std::chrono::steady_clock::now();

        m_counters = PerfCounters();
        
        NegamaxResult result = search_recurse(state, 0, maxDepth, MIN_SCORE, MAX_SCORE);

        m_counters.duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - start);

        if (m_abort) {
            LOG(Logging::debug) << "Aborted without result";
        } else {
            LOG(Logging::debug) << result;
        }
        LOG(Logging::debug) << m_counters;
        return result;
    }

    /**
     * @brief Aborts the currently running calculation.
     * Call from another thread to  abort currently running search.
     */
    void abort() {
        m_abort = true;
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
    class Option {
    public:
        Option(TGameState& state, Turn& turn, Score score)
            : state(&state)
            , turn(&turn)
            , score(score) {}
        
        TGameState* state;
        Turn* turn;
        
        bool operator<(const Option& other) const { return score > other.score; }
    private:
        Score score;
    };
    
    /**
     * @brief Recursive Negamax search with optional Alpha-Beta cutoff.
     * @param state Game state to search from.
     * @param depth Depth in plys already searched.
     * @param maxDepth Maximum depth in plys to search.
     * @param alpha Alpha score.
     * @param beta Beta score.
     */
    NegamaxResult search_recurse(TGameState state, size_t depth, const size_t maxDepth, Score alpha, Score beta) {
        if (m_abort) return{ 0, boost::none };

        const size_t pliesLeft = maxDepth - depth;

        if (state.isGameOver() || pliesLeft == 0) {
            return { state.getScore(), boost::none };
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
                
                if (alpha >= beta) {
                    return { tableEntry->score, tableEntry->turn };
                }
            }
        }

        NegamaxResult bestResult { MIN_SCORE, boost::none };
        
        std::vector<Turn> possibleTurns = state.getTurnList();
        assert(possibleTurns.size() > 0);
        
        
        std::vector<TGameState> consideredStates(possibleTurns.size(), state);
        std::vector<Option> consideredOptions;
        
        consideredOptions.reserve(possibleTurns.size());
        
        for (int i = 0; i < possibleTurns.size(); ++i) {
            // Need to apply all states at this point to be able to
            // use their data for move ordering.
            consideredStates[i].applyTurn(possibleTurns[i]);
            consideredOptions.emplace_back(consideredStates[i],
                                             possibleTurns[i],
                                             estimateScoreFor(consideredStates[i]));
        }
        
        if (MOVE_ORDERING_ENABLED) {
            std::sort(begin(consideredOptions), end(consideredOptions));
        }
        
        for (Option& option: consideredOptions) {
            Turn& turn = *option.turn;
            TGameState& newState = *option.state;
            
            ++m_counters.nodes;

            NegamaxResult result = -search_recurse(
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

            if (m_abort) return{ 0, boost::none };
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
        
        return bestResult;
    }
    
    Score estimateScoreFor(const TGameState& state) const {
        if (TRANSPOSITION_TABLES_ENABLED) {
            auto entry = m_transpositionTable.lookup(state.getHash());
            if (!entry) return MIN_SCORE; //TODO: Is it negative or neutral to not have a tpt entry
            else return entry->score;
        } else {
            // Greedy estimate based on next move
            return state.getScore();
        }
    }
    
    TranspositionTable m_transpositionTable;
    
    //! Abort flag
    std::atomic<bool> m_abort;

    Logging::Logger m_log;
};

#endif // NEGAMAX_H
