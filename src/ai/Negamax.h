#ifndef NEGAMAX_H
#define NEGAMAX_H

#include <boost/optional/optional.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/attributes/constant.hpp>
#include <string>
#include <sstream>

#include "misc/helper.h"
#include "logic/interface/AbstractEvaluator.h"
#include "logic/GameState.h"

template <typename TEvaluatorPtr = EvaluatorPtr>
class Negamax {
public:
    Negamax(TEvaluatorPtr evaluator)
        : m_evaluator(evaluator) {
        m_log.add_attribute("Tag", boost::log::attributes::constant< std::string >("Negamax"));
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
        BOOST_LOG(m_log) << "Starting search of depth " << maxDepth;
        m_counters = PerfCounters();
        Result result = search_recurse<TGameState, AB_CUTOFF_ENABLED>(state, 0, maxDepth,
                                                             MIN_SCORE, MAX_SCORE);
        BOOST_LOG(m_log) << m_counters;
        return result;
    }

    struct PerfCounters {
        PerfCounters() : nodes(0), cutoffs(0), updates(0) {}
        uint64_t nodes;
        uint64_t cutoffs;
        uint64_t updates;

        std::string toString() const {
            std::stringstream ss;
            ss << "Nodes visited:   " << nodes << endl
               << "No. of cut offs: " << cutoffs << endl
               << "Result updates:  " << updates << endl;
            return ss.str();
        }
    } m_counters;
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
            ++m_counters.nodes;

            // Simulate ply
            newState.applyTurn(turn);
            // Get best possible return ply score in maxDepth for adversary
            Result result = search_recurse(newState, depth + 1, maxDepth,
                                           -beta, -max(alpha, bestResult.score));
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
                    //LOG(trace) << "(" << depth << ") AB-cutoff " << bestResult.score << " exceeds beta of " << beta;
                    return bestResult;
                }
            }
        }

        //LOG(trace) << "(" << depth << ") Best score " << bestResult.score;
        return bestResult;
    }

    TEvaluatorPtr m_evaluator;
    boost::log::sources::logger m_log;
};

#endif // NEGAMAX_H
