#ifndef DEBUG_TOOLS_H
#define DEBUG_TOOLS_H

#include <string>
#include <array>
#include <random>

#include "misc/helper.h"
#include "logic/ChessBoard.h"
#include "logic/GameState.h"

/**
 * @brief Contains functions for helping with debugging tasks.
 */
namespace DebugTools {

/**
 * @brief Returns the code needed to initialize a board to the given state
 */
std::string toInitializerList(const std::array<Piece, 64>& board);

template <typename Rng>
GameState generateRandomState(size_t maxTurns, Rng& rng) {
    std::uniform_int_distribution<size_t> dst(0, maxTurns);
    const size_t turnCount = dst(rng);

    GameState gs;
    for (size_t i = 0; i < turnCount; ++i) {
        auto turns = gs.getTurnList();
        auto turn = random_selection(turns, rng);
        if (turn == end(turns)) break;
        gs.applyTurn(*turn);
    }

    return gs;
}

template <typename Rng>
ChessBoard generateRandomBoard(size_t maxTurns, Rng& rng) {
    return generateRandomState(maxTurns, rng).getChessBoard();
}


} // namespace DebugTools


#endif // DEBUG_TOOLS_H
