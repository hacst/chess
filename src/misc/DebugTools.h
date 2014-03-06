/*
    Copyright (c) 2013-2014, Stefan Hacker <dd0t@users.sourceforge.net>

    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

    3. Neither the name of the copyright holder nor the names of its
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.
*/
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

/**
 * @brief Generates a random GameState.
 * Emulating a games a game with up to maxTurns random moves.
 * @param maxTurns Limit for number of moves.
 * @param rng C++ Random number generator to use.
 */
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

/**
* @brief Generates a random Board.
* @see generateRandomState
*/
template <typename Rng>
ChessBoard generateRandomBoard(size_t maxTurns, Rng& rng) {
    return generateRandomState(maxTurns, rng).getChessBoard();
}


} // namespace DebugTools


#endif // DEBUG_TOOLS_H
