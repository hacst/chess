/*
    Copyright (c) 2013-2014, Max Stark <max.stark88@googlemail.com>

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
#ifndef TURN_H
#define TURN_H

#include "ChessTypes.h"

/**
 * @brief Represents a chess turn.
 */
class Turn {
public:
    Piece piece;
    Field from, to;
    enum Action {
        Move, Castle, Forfeit, Pass,
        PromotionQueen, PromotionBishop,
        PromotionKnight, PromotionRook
    } action;

    Turn()
        : piece(Piece(White, NoType)), from(), to(), action(Pass) {}
    Turn(Piece piece, Field from, Field to, Action action)
        : piece(piece), from(from), to(to), action(action) {}

    bool isMove() const { return action == Move; }
    bool isCastling() const { return action == Castle; }
    bool isPromotion() const { return action >= PromotionQueen && action <= PromotionRook; }
    bool isForfeit() const { return action == Forfeit; }
    bool isPass() const { return action == Pass; }
    PieceType getPromotionPieceType() const { return static_cast<PieceType>(action - PromotionQueen + 1); }

    static Turn move(Piece piece, Field from, Field to) {
        return Turn(piece, from, to, Move);
    }

    static Turn castle(Piece piece, Field from, Field to) {
        assert(piece.type == King);
        return Turn(piece, from, to, Castle);
    }

    static Turn promotionQueen(Piece piece, Field from, Field to) {
        assert((piece.player == White && rankFor(to) == Eight)
            || (piece.player == Black && rankFor(to) == One));
        assert(piece.type == Pawn);

        return Turn(piece, from, to, PromotionQueen);
    }

    static Turn promotionBishop(Piece piece, Field from, Field to) {
        assert((piece.player == White && rankFor(to) == Eight)
            || (piece.player == Black && rankFor(to) == One));
        assert(piece.type == Pawn);

        return Turn(piece, from, to, PromotionBishop);
    }

    static Turn promotionRook(Piece piece, Field from, Field to) {
        assert((piece.player == White && rankFor(to) == Eight)
            || (piece.player == Black && rankFor(to) == One));
        assert(piece.type == Pawn);

        return Turn(piece, from, to, PromotionRook);
    }

    static Turn promotionKnight(Piece piece, Field from, Field to) {
        assert((piece.player == White && rankFor(to) == Eight)
            || (piece.player == Black && rankFor(to) == One));
        assert(piece.type == Pawn);

        return Turn(piece, from, to, PromotionKnight);
    }

    static Turn pass(PlayerColor player) {
        return Turn(Piece(player, NoType), ERR, ERR, Pass);
    }

    bool operator==(const Turn& other) const;
    bool operator!=(const Turn& other) const;

    std::string toString() const;
};

using Turns = std::vector<Turn>;

/* function for comparing (unsorted) turn vectors */
bool        turnVecCompare(const std::vector<Turn>& left, const std::vector<Turn>& right);
/* function for printing all turns in the vector */
std::string turnVecToString(std::vector<Turn> v);
/* function determindes wheater the given vector is part of the other */
bool        turnVecContains(std::vector<Turn> v, std::vector<Turn> other);
/* function determines wheater the given vector contains the given turn or not */
bool        turnVecContains(std::vector<Turn> v, Turn t);

#endif // TURN_H
