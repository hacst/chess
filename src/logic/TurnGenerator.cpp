#include "TurnGenerator.h"

std::vector<Turn> TurnGenerator::generateTurns(PlayerColor player, const ChessBoard& cb) const {
    std::vector<Turn> vecTurns, vecAllTurns;
    BitBoard bbTurns, bbCurPieceType, bbCurPiece;
    Field curPiecePos;
    Piece piece;

    for (int pieceType = King; pieceType <= Pawn ; pieceType++) {
        piece.type   = (PieceType) pieceType;
        piece.player = player;

        bbCurPieceType = cb.m_bb[player][pieceType];
        while (bbCurPieceType != 0) {
            bbCurPiece  = 0;
            curPiecePos = BB_SCAN(bbCurPieceType);
            BIT_CLEAR(bbCurPieceType, curPiecePos);
            BIT_SET  (bbCurPiece,     curPiecePos);

            bbTurns  = calcTurns      (piece, bbCurPiece, cb);
            vecTurns = bitBoardToTurns(piece, bbCurPiece, bbTurns);

            vecAllTurns.insert(vecAllTurns.end(), vecTurns.begin(), vecTurns.end());
        }
    }

    return vecAllTurns;
}

std::vector<Turn> TurnGenerator::bitBoardToTurns(Piece piece, BitBoard bbPiece, BitBoard bbTurns) const {
    std::vector<Turn> turns;
    Field from, to;

    from = BB_SCAN(bbPiece);
    while (bbTurns != 0) {
        to = BB_SCAN(bbTurns);
        BIT_CLEAR(bbTurns, to);

        turns.push_back(Turn::move(piece, from, to));
    }

    return turns;
}

BitBoard TurnGenerator::calcTurns(Piece piece, BitBoard bbPiece, const ChessBoard& cb) const {
    PlayerColor opp = (piece.player == White) ? Black : White;

    switch (piece.type) {
    case King:   return calcKingTurns  (bbPiece, cb.m_bb[piece.player][AllPieces]);
    case Queen:  return calcQueenTurns (bbPiece, cb.m_bb[opp][AllPieces],
                                        cb.m_bb[piece.player][AllPieces] | cb.m_bb[opp][AllPieces]);
    case Bishop: return calcBishopTurns(bbPiece, cb.m_bb[opp][AllPieces],
                                        cb.m_bb[piece.player][AllPieces] | cb.m_bb[opp][AllPieces]);
    case Knight: return calcKnightTurns(bbPiece, cb.m_bb[piece.player][AllPieces]);
    case Rook:   return calcRookTurns  (bbPiece, cb.m_bb[opp][AllPieces],
                                        cb.m_bb[piece.player][AllPieces] | cb.m_bb[opp][AllPieces]);
    case Pawn:   return calcPawnTurns  (bbPiece, cb.m_bb[opp][AllPieces],
                                        cb.m_bb[piece.player][AllPieces] | cb.m_bb[opp][AllPieces], piece.player);
    default:     return 0;
    }
}

BitBoard TurnGenerator::calcKingTurns(BitBoard king, BitBoard allOwnPieces) const {

    // TODO: Steht der King im schach? Nur Zuege berechnen, um aus dem
    // schach rauszukommen -> wenn keine moeglichen zuege gefunden -> sieger?
    // TODO: patt?

    BitBoard turn1 = (king & clearFile(H)) << 9;
    BitBoard turn2 = king                  << 8;
    BitBoard turn3 = (king & clearFile(A)) << 7;
    BitBoard turn4 = (king & clearFile(H)) << 1;

    BitBoard turn5 = (king & clearFile(A)) >> 1;
    BitBoard turn6 = (king & clearFile(H)) >> 7;
    BitBoard turn7 = king                  >> 8;
    BitBoard turn8 = (king & clearFile(A)) >> 9;

    BitBoard kingTurns = turn1 | turn2 | turn3 | turn4 |
                         turn5 | turn6 | turn7 | turn8;
    kingTurns &= ~allOwnPieces;

    // TODO: Rochade

    // TODO: steht der (eigene) king nach einem der ermittelten zuege
    // im schach? <- Diese zuege entfernenQt Creator 3.0
    // dazu alle möeglichen zuege des anderen spielers berechnen
    // (in einem bb) und dann verunden mit king bb. ergebniss bb
    // enthaelt felder, die der king nicht betreten darf, also
    // nochmal xor verknüpfen

    return kingTurns;
}

BitBoard TurnGenerator::calcKnightTurns(BitBoard knights, BitBoard allOwnPieces) const {
    BitBoard turn1 = (knights & (clearFile(A) & clearFile(B))) << 6;
    BitBoard turn2 = (knights & (clearFile(A)))                << 15;
    BitBoard turn3 = (knights & (clearFile(H)))                << 17;
    BitBoard turn4 = (knights & (clearFile(H) & clearFile(G))) << 10;

    BitBoard turn5 = (knights & (clearFile(H) & clearFile(G))) >> 6;
    BitBoard turn6 = (knights & (clearFile(H)))                >> 15;
    BitBoard turn7 = (knights & (clearFile(A)))                >> 17;
    BitBoard turn8 = (knights & (clearFile(A) & clearFile(B))) >> 10;

    BitBoard knightTurns = turn1 | turn2 | turn3 | turn4 |
                           turn5 | turn6 | turn7 | turn8;
    knightTurns &= ~allOwnPieces;

    return knightTurns;
}

BitBoard TurnGenerator::calcPawnTurns(BitBoard pawns, BitBoard allOppPieces,
    BitBoard allPieces, PlayerColor player) const {

    // TODO: en passant

    // Pawn Moves
    BitBoard oneStep = 0;
    BitBoard twoSteps = 0;
    if (player == White) {
        oneStep  = (pawns                       << 8) & ~allPieces;
        twoSteps = ((oneStep & maskRank(Three)) << 8) & ~allPieces;
    } else if (player == Black) {
        oneStep  = (pawns                       >> 8) & ~allPieces;
        twoSteps = ((oneStep & maskRank(Six))   >> 8) & ~allPieces;
    }
    BitBoard pawnMoves = oneStep | twoSteps;

    // Pawn Attacks
    BitBoard leftAttacks = 0;
    BitBoard rightAttacks = 0;
    if (player == White) {
        leftAttacks  = (pawns & clearFile(A)) << 7;
        rightAttacks = (pawns & clearFile(H)) << 9;
    } else if (player == Black) {
        leftAttacks  = (pawns & clearFile(A)) >> 9;
        rightAttacks = (pawns & clearFile(H)) >> 7;
    }
    BitBoard pawnAttacks = (leftAttacks | rightAttacks) & allOppPieces;

    // valid moves + attacks
    return pawnMoves | pawnAttacks;
}

BitBoard TurnGenerator::calcQueenTurns(BitBoard queens, BitBoard allOppPieces,
    BitBoard allPieces) const {
    return calcRookTurns  (queens, allOppPieces, allPieces) |
           calcBishopTurns(queens, allOppPieces, allPieces);
}

BitBoard TurnGenerator::calcBishopTurns(BitBoard bishops, BitBoard allOppPieces,
    BitBoard allPieces) const {
    BitBoard bbNE = getBitsNE(bishops);
    BitBoard bbNW = getBitsNW(bishops);
    BitBoard bbSE = getBitsSE(bishops);
    BitBoard bbSW = getBitsSW(bishops);


    //LOG(info) << bitBoardToString(bbNE);
    //LOG(info) << bitBoardToString(bbNW);
    //LOG(info) << bitBoardToString(bbSE);
    //LOG(info) << bitBoardToString(bbSW);


    // north east moves
    BitBoard neMoves = bbNE & allPieces;
    neMoves = (neMoves <<  9) | (neMoves << 18) | (neMoves << 27) |
              (neMoves << 36) | (neMoves << 45) | (neMoves << 54);
    neMoves &= bbNE;
    neMoves ^= bbNE;
    neMoves &= (allOppPieces | ~(allPieces));

    // north west moves
    BitBoard nwMoves = bbNW & allPieces;
    nwMoves = (nwMoves <<  7) | (nwMoves << 14) | (nwMoves << 21) |
              (nwMoves << 28) | (nwMoves << 35) | (nwMoves << 42);
    nwMoves &= bbNW;
    nwMoves ^= bbNW;
    nwMoves &= (allOppPieces | ~(allPieces));

    // south east movespp
    BitBoard seMoves = bbSE & allPieces;
    seMoves = (seMoves >>  7) | (seMoves >> 14) | (seMoves >> 21) |
              (seMoves >> 28) | (seMoves >> 35) | (seMoves >> 42);
    seMoves &= bbSE;
    seMoves ^= bbSE;
    seMoves &= (allOppPieces | ~(allPieces));

    // south west moves
    BitBoard swMoves = bbSW & allPieces;
    swMoves = (swMoves >>  9) | (swMoves >> 18) | (swMoves >> 27) |
              (swMoves >> 36) | (swMoves >> 45) | (swMoves >> 54);
    swMoves &= bbSW;
    swMoves ^= bbSW;
    swMoves &= (allOppPieces | ~(allPieces));

    return neMoves | nwMoves | seMoves | swMoves;
}

BitBoard TurnGenerator::calcRookTurns(BitBoard rooks, BitBoard allOppPieces,
    BitBoard allPieces) const {
    BitBoard bbE = getBitsE(rooks);
    BitBoard bbW = getBitsW(rooks);
    BitBoard bbN = getBitsN(rooks);
    BitBoard bbS = getBitsS(rooks);

    // right moves
    BitBoard rightMoves = bbE & allPieces;
    rightMoves = (rightMoves << 1) | (rightMoves << 2) | (rightMoves << 3) |
                 (rightMoves << 4) | (rightMoves << 5) | (rightMoves << 6) |
                 (rightMoves << 7);
    //rightMoves = 0xFE00000000000000 >> (56-LSB von rightMoves);
    rightMoves &= bbE;
    rightMoves ^= bbE;
    rightMoves &= (allOppPieces | ~(allPieces));

    // left moves
    BitBoard leftMoves = bbW & allPieces;
    leftMoves = 0xFE00000000000000 >> (64-BB_SCAN(leftMoves));
    leftMoves &= bbW;
    leftMoves ^= bbW;
    leftMoves &= (allOppPieces | ~(allPieces));

    // up moves
    BitBoard upMoves = bbN & allPieces;
    upMoves = (upMoves <<  8) | (upMoves << 16) | (upMoves << 24) |
              (upMoves << 32) | (upMoves << 40) | (upMoves << 48) |
              (upMoves << 56);
    //upMoves = 0x0101010101010100 << LSB;
    upMoves &= bbN;
    upMoves ^= bbN;
    upMoves &= (allOppPieces | ~(allPieces));


    // down moves
    BitBoard downMoves = bbS & allPieces;
    downMoves = 0x0101010101010100 >> (64-BB_SCAN(downMoves));
    downMoves &= bbS;
    downMoves ^= bbS;
    downMoves &= (allOppPieces | ~(allPieces));

    return rightMoves | leftMoves | upMoves | downMoves;
}



BitBoard TurnGenerator::getBitsNE(BitBoard bbPiece) const {
    int field = BB_SCAN(bbPiece);
    BitBoard bb = 0x8040201008040200 << field;

    BitBoard bbMask = 0;
    for (int i = (field % 8) + 1; i < 8; i++) {
        bbMask |= maskFile(static_cast<File>(i));
    }

    return bb & bbMask;
}

BitBoard TurnGenerator::getBitsNW(BitBoard bbPiece) const {
    int field = BB_SCAN(bbPiece);
    int shift = field - 7;
    BitBoard bb;

    if (shift < 0) {
        bb = 0x0102040810204000 >> shift*(-1);
    } else {
        bb = 0x0102040810204000 << shift;
    }

    BitBoard bbMask = 0;
    for (int i = 0; i < (field % 8); i++) {
        bbMask |= maskFile(static_cast<File>(i));
    }

    return bb & bbMask;
}

BitBoard TurnGenerator::getBitsSE(BitBoard bbPiece) const {
    int field = BB_SCAN(bbPiece);
    int shift = 56 - field;
    BitBoard bb;

    if (shift < 0) {
        bb = 0x0002040810204080 << shift*(-1);
    } else {
        bb = 0x0002040810204080 >> shift;
    }

    BitBoard bbMask = 0;
    for (int i = (field % 8) + 1; i < 8; i++) {
        bbMask |= maskFile(static_cast<File>(i));
    }

    return bb & bbMask;
}

BitBoard TurnGenerator::getBitsSW(BitBoard bbPiece) const {
    int field = BB_SCAN(bbPiece);
    BitBoard bb = 0x0040201008040201 >> (64-(field+1));

    BitBoard bbMask = 0;
    for (int i = 0; i < (field % 8); i++) {
        bbMask |= maskFile(static_cast<File>(i));
    }

    return bb & bbMask;
}




// TODO: Shift mit mehr als 32 geht nicht?
//BitBoard bb = 0x00000000000000FE << 32;
//bb &= maskRank(static_cast<Rank>(field / 8));

BitBoard TurnGenerator::getBitsE(BitBoard bbPiece) const {
    int field = BB_SCAN(bbPiece);
    BitBoard bb = 0xFE00000000000000 >> (56-field);
    bb &= maskRank(static_cast<Rank>(field / 8));

    return bb;
}

BitBoard TurnGenerator::getBitsW(BitBoard bbPiece) const {
    int field = BB_SCAN(bbPiece);
    BitBoard bb = 0xFE00000000000000 >> (64-field);
    bb &= maskRank(static_cast<Rank>(field / 8));

    return bb;
}

BitBoard TurnGenerator::getBitsN(BitBoard bbPiece) const {
    int field = BB_SCAN(bbPiece);
    BitBoard bb = 0x0101010101010100 << field;

    return bb;
}

// TODO: shifting a number of bits that is equal to or larger than
// its width is undefined behavior. You can only safely shift a 64-bit
// integer between 0 and 63 positions
// BitBoard bb = 0x0101010101010100 >> (64-field);

BitBoard TurnGenerator::getBitsS(BitBoard bbPiece) const {
    int field = BB_SCAN(bbPiece);
    BitBoard bb = 0x0080808080808080 >> (64-(field+1));

    return bb;
}


BitBoard TurnGenerator::maskRank(Rank rank) const {
    // Geht nicht! Da die Zahl als 32 Bit Integer aufgefasst wird
    // funktioniert der Shift nicht korrekt!
    // http://stackoverflow.com/questions/10499104/is-shifting-more-than-32-bits-of-a-uint64-t-integer-on-an-x86-machine-undefined
    //return 0x00000000000000FF << (rank * 8);

    return (BitBoard)0x00000000000000FF << (rank *8);

    //BitBoard bb = 0x00000000000000FF;
    //return bb << (rank * 8);
}

BitBoard TurnGenerator::clearRank(Rank rank) const {
    return ~(maskRank(rank));
}

BitBoard TurnGenerator::maskFile(File file) const {
    return 0x0101010101010101 << file;
}

BitBoard TurnGenerator::clearFile(File file) const {
    return ~(maskFile(file));
}
