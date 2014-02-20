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
#ifndef INCREMENTALMATERIALANDPSTEVALUATOR_H
#define INCREMENTALMATERIALANDPSTEVALUATOR_H

#include <array>
#include "logic/ChessTypes.h"

class ChessBoard;
class Turn;

/**
 * @brief Class for incrementally estimating game state using PST and Material.
 * Uses fixed piece square tables and a fixed material evaluation to
 * incrementally calculate a score for the current board position during
 * the game.
 * @warning Does not handle game over conditions
 * @see ChessBoard
 * @note Not valid once game is over.
 */
class IncrementalMaterialAndPSTEvaluator {
public:
    //! Initializes the evaluator for a prestine board.
    IncrementalMaterialAndPSTEvaluator();
    //! Initializes the evaluator for an already played board.
    explicit IncrementalMaterialAndPSTEvaluator(const std::array<Piece, 64> &board);

    //! Updates estimate for the moving of the piece in give turn.
    void moveIncrement(const Turn& turn);
    //! Updates estimate for a capture of the given piece on the given field.
    void captureIncrement(Field field, const Piece& piece);
    //! Updates estimate for the promotion of a piece
    void promotionIncrement(const Turn& turn, PieceType targetType);

    //! Gives a full estimate for the given board
    static Score estimateFullBoard(const std::array<Piece, 64> &board);

    //! Returns the score from the perspective of the given player color.
    Score getScore(PlayerColor color) const;

    bool operator==(const IncrementalMaterialAndPSTEvaluator& other) const;
private:
    //! Score estimation for white player
    Score m_estimatedScore;
};

#endif // INCREMENTALMATERIALANDPSTEVALUATOR_H
