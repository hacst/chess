#ifndef TURNGENERATOR_H
#define TURNGENERATOR_H

#include "ChessTypes.h"
#include "ChessBoard.h"

class TurnGenerator {
public:
    void              initFlags(ChessBoard &cb);
    std::vector<Turn> getTurnList() const;
    void              generateTurns(PlayerColor player,
                                            ChessBoard& cb);
//private:
    std::vector<Turn> turnList;

    void bitBoardToTurns(Piece piece,
                                Field from,
                                BitBoard bbTurns,
                                BitBoard bbAllOppTurns,
                                ChessBoard& cb,
                                Turns& turnsOut);

    BitBoard calcMoveTurns(Piece piece,
                                   BitBoard bbPiece,
                                   BitBoard bbAllOppTurns,
                                   const ChessBoard& cb);
    BitBoard calcUnCheckFields(PlayerColor opp,
                                       const ChessBoard& cb);
    BitBoard calcAllOppTurns(PlayerColor opp,
                                     const ChessBoard& cb);
    BitBoard calcShortCastleTurns(PlayerColor player,
                                          BitBoard bbAllPieces,
                                          BitBoard bbAllOppTurns);
    BitBoard calcLongCastleTurns(PlayerColor player,
                                         BitBoard bbAllPieces,
                                         BitBoard bbAllOppTurns);

    // nonsliding pieces
    BitBoard calcKingTurns      (BitBoard king,
                                         BitBoard allOwnPieces,
                                         BitBoard allOppTurns) const;
    BitBoard calcKnightTurns    (BitBoard knights,
                                         BitBoard allOwnPieces) const;
    BitBoard calcPawnTurns      (BitBoard pawns,
                                         BitBoard allPieces,
                                         BitBoard allOppPieces,
                                         PlayerColor player,
                                         Field enPassantSquare) const;
    BitBoard calcPawnMoveTurns  (BitBoard pawns,
                                         BitBoard allPieces,
                                         PlayerColor player) const;
    BitBoard calcPawnAttackTurns(BitBoard pawns,
                                         BitBoard allOppPieces,
                                         PlayerColor player,
                                         Field enPassantSquare) const;
    // sliding pieces
    BitBoard calcQueenTurns (BitBoard queens,
                                     BitBoard allOppPieces,
                                     BitBoard allPieces) const;
    BitBoard calcBishopTurns(BitBoard bishops,
                                     BitBoard allOppPieces,
                                     BitBoard allPieces) const;
    BitBoard calcRookTurns  (BitBoard rooks,
                                     BitBoard allOppPieces,
                                     BitBoard allPieces) const;

    // some helper functions
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
};

using TurnGeneratorPtr = std::shared_ptr<TurnGenerator>;

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




// TODO: Shift mit mehr als 32 geht nicht?
//BitBoard bb = 0x00000000000000FE << 32;
//bb &= maskRank(static_cast<Rank>(field / 8));

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


    //BitBoard bb = 0xFE00000000000000 >> (56-field);
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

// TODO: shifting a number of bits that is equal to or larger than
// its width is undefined behavior. You can only safely shift a 64-bit
// integer between 0 and 63 positions
// BitBoard bb = 0x0101010101010100 >> (64-field);

inline BitBoard TurnGenerator::getBitsS(BitBoard bbPiece) const {
    int field = BB_SCAN(bbPiece);
    BitBoard bb = 0x0080808080808080 >> (64 - (field + 1));

    return bb;
}


inline BitBoard TurnGenerator::maskRank(Rank rank) const {
    // Geht nicht! Da die Zahl als 32 Bit Integer aufgefasst wird
    // funktioniert der Shift nicht korrekt!
    // http://stackoverflow.com/questions/10499104/is-shifting-more-than-32-bits-of-a-uint64-t-integer-on-an-x86-machine-undefined
    //return 0x00000000000000FF << (rank * 8);

    return (BitBoard) 0x00000000000000FF << (rank * 8);

    //0x5ULL; // 64 bit u long long

    //BitBoard bb = 0x00000000000000FF;
    //return bb << (rank * 8);
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


#endif // TURNGENERATOR_H
