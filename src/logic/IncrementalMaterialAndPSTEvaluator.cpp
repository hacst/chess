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
#include "IncrementalMaterialAndPSTEvaluator.h"

#include "GameState.h"
#include <array>
#include <iostream>

using PieceSquareTable = std::array<Score, NUM_FIELDS>;

namespace {
/**
* @brief Piece square table as proposed by http://chessprogramming.wikispaces.com/Simplified+evaluation+function#Piece-Square
* @note Two dimensional. Indexable by Piece then Field from blacks POV.
*/
    const std::array<PieceSquareTable, NUM_PIECETYPES> PIECE_SQUARE_TABLE = { {
    {   // King
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -30, -40, -40, -50, -50, -40, -40, -30,
        -20, -30, -30, -40, -40, -30, -30, -20,
        -10, -20, -20, -20, -20, -20, -20, -10,
        20, 20, 0, 0, 0, 0, 20, 20,
        20, 30, 10, 0, 0, 10, 30, 20
    },

    {   // Queen
        -20, -10, -10, -5, -5, -10, -10, -20,
        -10, 0, 0, 0, 0, 0, 0, -10,
        -10, 0, 5, 5, 5, 5, 0, -10,
        -5, 0, 5, 5, 5, 5, 0, -5,
        0, 0, 5, 5, 5, 5, 0, -5,
        -10, 5, 5, 5, 5, 5, 0, -10,
        -10, 0, 5, 0, 0, 0, 0, -10,
        -20, -10, -10, -5, -5, -10, -10, -20
    },

    {   // Bishop
        -20, -10, -10, -10, -10, -10, -10, -20,
        -10, 0, 0, 0, 0, 0, 0, -10,
        -10, 0, 5, 10, 10, 5, 0, -10,
        -10, 5, 5, 10, 10, 5, 5, -10,
        -10, 0, 10, 10, 10, 10, 0, -10,
        -10, 10, 10, 10, 10, 10, 10, -10,
        -10, 5, 0, 0, 0, 0, 5, -10,
        -20, -10, -10, -10, -10, -10, -10, -20,
    },

    { // Knight
        -50, -40, -30, -30, -30, -30, -40, -50,
        -40, -20, 0, 0, 0, 0, -20, -40,
        -30, 0, 10, 15, 15, 10, 0, -30,
        -30, 5, 15, 20, 20, 15, 5, -30,
        -30, 0, 15, 20, 20, 15, 0, -30,
        -30, 5, 10, 15, 15, 10, 5, -30,
        -40, -20, 0, 5, 5, 0, -20, -40,
        -50, -40, -30, -30, -30, -30, -40, -50,
    },

    {   // Rooks
        0, 0, 0, 0, 0, 0, 0, 0,
        5, 10, 10, 10, 10, 10, 10, 5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        -5, 0, 0, 0, 0, 0, 0, -5,
        0, 0, 0, 5, 5, 0, 0, 0
    },

    {   // Pawn
        0, 0, 0, 0, 0, 0, 0, 0,
        50, 50, 50, 50, 50, 50, 50, 50,
        10, 10, 20, 30, 30, 20, 10, 10,
        5, 5, 10, 25, 25, 10, 5, 5,
        0, 0, 0, 20, 20, 0, 0, 0,
        5, -5, -10, 0, 0, -10, -5, 5,
        5, 10, 10, -20, -20, 10, 10, 5,
        0, 0, 0, 0, 0, 0, 0, 0
    }
}};

/**
* @brief Piece values as proposed by http://chessprogramming.wikispaces.com/Simplified+evaluation+function#Piece-Square
* @note Adjusted for indexing with Piece enum type
*/
const std::array<Score, NUM_PIECETYPES> PIECE_VALUES = {
    20000,  // king
    900,    // queen
    330,    // bishop
    320,    // knight
    500,    // rook
    100,    // pawn
};

} // namespace


IncrementalMaterialAndPSTEvaluator::IncrementalMaterialAndPSTEvaluator()
    : m_estimatedScore(0) {
    // Empty
}

IncrementalMaterialAndPSTEvaluator::IncrementalMaterialAndPSTEvaluator(const std::array<Piece, 64> &board)
    : m_estimatedScore(estimateFullBoard(board)) {
}

Score IncrementalMaterialAndPSTEvaluator::estimateFullBoard(const std::array<Piece, 64> &board) {
    // Calculate from white's perspective
    Score score = 0;

    Field position = A1;
    for (const Piece& piece : board) {
        if (piece.type <= Pawn) {
            const Field psqPosition = (piece.player == Black) ? position : flipHorizontal(position);

            const Score pieceSquareScore = PIECE_SQUARE_TABLE[piece.type][psqPosition];
            const Score pieceOverallScore = PIECE_VALUES[piece.type] + pieceSquareScore;

            if (piece.player == White) {
                score += pieceOverallScore;
            }
            else {
                score -= pieceOverallScore;
            }
        }
        position = nextField(position);
    }

    return score;
}

void IncrementalMaterialAndPSTEvaluator::moveIncrement(const Turn& turn) {
    // Only effects PSQT evaluation due to the move. Always from white's perspective.

    if (turn.piece.player == Black) {
        m_estimatedScore += PIECE_SQUARE_TABLE[turn.piece.type][turn.from];
        m_estimatedScore -= PIECE_SQUARE_TABLE[turn.piece.type][turn.to];
    }
    else {
        m_estimatedScore -= PIECE_SQUARE_TABLE[turn.piece.type][flipHorizontal(turn.from)];
        m_estimatedScore += PIECE_SQUARE_TABLE[turn.piece.type][flipHorizontal(turn.to)];
    }
}

void IncrementalMaterialAndPSTEvaluator::captureIncrement(Field field, const Piece& piece) {
    // Effects material and PSQT evaluation
    if (piece.player == Black) {
        m_estimatedScore += PIECE_SQUARE_TABLE[piece.type][field];
        m_estimatedScore += PIECE_VALUES[piece.type];
    }
    else {
        m_estimatedScore -= PIECE_SQUARE_TABLE[piece.type][flipHorizontal(field)];
        m_estimatedScore -= PIECE_VALUES[piece.type];
    }
}

void IncrementalMaterialAndPSTEvaluator::promotionIncrement(const Turn& turn, const PieceType targetType) {
    if (turn.piece.player == Black) {
        m_estimatedScore += PIECE_SQUARE_TABLE[Pawn][turn.from];
        m_estimatedScore += PIECE_VALUES[Pawn];
        m_estimatedScore -= PIECE_SQUARE_TABLE[targetType][turn.to];
        m_estimatedScore -= PIECE_VALUES[targetType];
    }
    else {
        const Field blackPromSquare = flipHorizontal(turn.to);
        const Field blackToSquare = flipHorizontal(turn.from);

        m_estimatedScore -= PIECE_SQUARE_TABLE[Pawn][blackToSquare];
        m_estimatedScore -= PIECE_VALUES[Pawn];
        m_estimatedScore += PIECE_SQUARE_TABLE[targetType][blackPromSquare];
        m_estimatedScore += PIECE_VALUES[targetType];
    }
}

Score IncrementalMaterialAndPSTEvaluator::getScore(PlayerColor color) const {
    return color == White ? m_estimatedScore : -m_estimatedScore;
}

bool IncrementalMaterialAndPSTEvaluator::operator == (const IncrementalMaterialAndPSTEvaluator& other) const {
    return m_estimatedScore == other.m_estimatedScore;
}
