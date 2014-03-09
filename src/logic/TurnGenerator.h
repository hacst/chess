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
#ifndef TURNGENERATOR_H
#define TURNGENERATOR_H

#include "ChessTypes.h"
#include "ChessBoard.h"

/**
 * @brief Turn generation (based on bitboards) and gameover detection.
 */
class TurnGenerator {
public:
    /**
     * @brief Returns the generated turns.
     * @warning The generateTurns-function needs to be called previously.
     */
    std::vector<Turn> getTurnList() const;

    //! Generates turns for the given player color, based on the given chessboard.
    void generateTurns(PlayerColor player, ChessBoard& cb);

    //! Sets the kingInCheck-Flag, based on the given chessboard.
    void initFlags(ChessBoard &cb);

//private: /* provide access for gtest functions */

    //! Creates turn objects from bitboards and adds it to turnsOut list
    void bitBoardToTurns(Piece piece,
                         Field from,
                         BitBoard bbTurns,
                         BitBoard bbAllOppTurns,
                         ChessBoard& cb,
                         Turns& turnsOut);

    //! Calculates all "normal" move turns
    BitBoard calcMoveTurns(Piece piece,
                           BitBoard bbPiece,
                           BitBoard bbAllOppTurns,
                           const ChessBoard& cb);
    //! Calculates a bitboard with all possible opponent turns
    BitBoard calcAllOppTurns(PlayerColor opp,
                             const ChessBoard& cb);
    /**
     * @brief If king is in check position, this function calculates
     * a bitboard with possible fields to uncheck the king.
     */
    BitBoard calcUnCheckFields(PlayerColor opp,
                               const ChessBoard& cb);
    //! Checks the requirements for the short castle turn.
    BitBoard calcShortCastleTurns(PlayerColor player,
                                  BitBoard bbAllPieces,
                                  BitBoard bbAllOppTurns);
    //! Checks the requirements for the long castle turn.
    BitBoard calcLongCastleTurns(PlayerColor player,
                                 BitBoard bbAllPieces,
                                 BitBoard bbAllOppTurns);

    /* Turn generation for the sliding pieces */

    //! Turn calculation for bishops.
    BitBoard calcBishopTurns(BitBoard bishops,
                             BitBoard allOppPieces,
                             BitBoard allPieces) const;
    //! Turn calculation for rooks.
    BitBoard calcRookTurns  (BitBoard rooks,
                             BitBoard allOppPieces,
                             BitBoard allPieces) const;
    //! Turn calculation for queen(s), combination of the the two previously.
    BitBoard calcQueenTurns (BitBoard queens,
                             BitBoard allOppPieces,
                             BitBoard allPieces) const;

    /* Turn generation for the nonsliding pieces */

    //! Turn calculation for the king.
    BitBoard calcKingTurns      (BitBoard king,
                                 BitBoard allOwnPieces,
                                 BitBoard allOppTurns) const;
    //! Turn calculation for knights.
    BitBoard calcKnightTurns    (BitBoard knights,
                                 BitBoard allOwnPieces) const;
    //! Turn calculation for pawns (move and attack turns).
    BitBoard calcPawnTurns      (BitBoard pawns,
                                 BitBoard allPieces,
                                 BitBoard allOppPieces,
                                 PlayerColor player,
                                 Field enPassantSquare) const;
    //! Calculates the move turns for pawns.
    BitBoard calcPawnMoveTurns  (BitBoard pawns,
                                 BitBoard allPieces,
                                 PlayerColor player) const;
    //! Calculates the attack turns for pawns.
    BitBoard calcPawnAttackTurns(BitBoard pawns,
                                 BitBoard allOppPieces,
                                 PlayerColor player,
                                 Field enPassantSquare) const;

    /* Some inline helper functions for bit pushing */

    BitBoard maskRank (Rank rank) const;
    BitBoard clearRank(Rank rank) const;
    BitBoard maskFile (File file) const;
    BitBoard clearFile(File file) const;

    BitBoard getBitsE(BitBoard bbPiece) const;
    BitBoard getBitsW(BitBoard bbPiece) const;
    BitBoard getBitsN(BitBoard bbPiece) const;
    BitBoard getBitsS(BitBoard bbPiece) const;

    BitBoard getBitsNE(BitBoard bbPiece) const;
    BitBoard getBitsNW(BitBoard bbPiece) const;
    BitBoard getBitsSE(BitBoard bbPiece) const;
    BitBoard getBitsSW(BitBoard bbPiece) const;

    //! Contains the generated turns.
    std::vector<Turn> turnList;
};

inline BitBoard TurnGenerator::maskRank(Rank rank) const {
    return (BitBoard) 0x00000000000000FF << (rank * 8);
}

inline BitBoard TurnGenerator::clearRank(Rank rank) const {
    return ~(maskRank(rank));
}

inline BitBoard TurnGenerator::maskFile(File file) const {
    return 0x0101010101010101 << file;
}

inline BitBoard TurnGenerator::clearFile(File file) const {
    return ~(maskFile(file));
}

inline BitBoard TurnGenerator::getBitsE(BitBoard bbPiece) const {
    int field = BB_SCAN(bbPiece);
    int shift = 56 - field;
    BitBoard bb;

    if (shift > 0) {
        bb = 0xFE00000000000000 >> shift;
    }
    else {
        bb = 0xFE00000000000000 << (shift * (-1));
    }

    bb &= maskRank(static_cast<Rank>(field / 8));

    return bb;
}

inline BitBoard TurnGenerator::getBitsW(BitBoard bbPiece) const {
    int field = BB_SCAN(bbPiece);
    BitBoard bb = 0xFE00000000000000 >> (64 - field);
    bb &= maskRank(static_cast<Rank>(field / 8));

    return bb;
}

inline BitBoard TurnGenerator::getBitsN(BitBoard bbPiece) const {
    int field = BB_SCAN(bbPiece);
    BitBoard bb = 0x0101010101010100 << field;

    return bb;
}

inline BitBoard TurnGenerator::getBitsS(BitBoard bbPiece) const {
    int field = BB_SCAN(bbPiece);
    BitBoard bb = 0x0080808080808080 >> (64 - (field + 1));

    return bb;
}

inline BitBoard TurnGenerator::getBitsNE(BitBoard bbPiece) const {
    int field = BB_SCAN(bbPiece);
    BitBoard bb = 0x8040201008040200 << field;

    BitBoard bbMask = 0;
    for (int i = (field % 8) + 1; i < 8; i++) {
        bbMask |= maskFile(static_cast<File>(i));
    }

    return bb & bbMask;
}

inline BitBoard TurnGenerator::getBitsNW(BitBoard bbPiece) const {
    int field = BB_SCAN(bbPiece);
    int shift = field - 7;
    BitBoard bb;

    if (shift < 0) {
        bb = 0x0102040810204000 >> shift*(-1);
    }
    else {
        bb = 0x0102040810204000 << shift;
    }

    BitBoard bbMask = 0;
    for (int i = 0; i < (field % 8); i++) {
        bbMask |= maskFile(static_cast<File>(i));
    }

    return bb & bbMask;
}

inline BitBoard TurnGenerator::getBitsSE(BitBoard bbPiece) const {
    int field = BB_SCAN(bbPiece);
    int shift = 56 - field;
    BitBoard bb;

    if (shift < 0) {
        bb = 0x0002040810204080 << shift*(-1);
    }
    else {
        bb = 0x0002040810204080 >> shift;
    }

    BitBoard bbMask = 0;
    for (int i = (field % 8) + 1; i < 8; i++) {
        bbMask |= maskFile(static_cast<File>(i));
    }

    return bb & bbMask;
}

inline BitBoard TurnGenerator::getBitsSW(BitBoard bbPiece) const {
    int field = BB_SCAN(bbPiece);
    BitBoard bb = 0x0040201008040201 >> (64 - (field + 1));

    BitBoard bbMask = 0;
    for (int i = 0; i < (field % 8); i++) {
        bbMask |= maskFile(static_cast<File>(i));
    }

    return bb & bbMask;
}

#endif // TURNGENERATOR_H
