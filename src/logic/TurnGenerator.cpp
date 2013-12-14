#include "TurnGenerator.h"

std::vector<Turn> TurnGenerator::generateTurns(PlayerColor player, const ChessBoard& cb) {
    std::vector<Turn> vecTurns, vecAllTurns;
    BitBoard bbTurns, bbCurPieceType, bbCurPiece;
    Field curPiecePos;
    Piece piece;

    for (int pieceType = King; pieceType <= Pawn ; pieceType++) {
        piece.type   = (PieceType) pieceType;
        piece.player = player;

        bbCurPieceType = cb.bb[player][pieceType];
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

std::vector<Turn> TurnGenerator::bitBoardToTurns(Piece piece, BitBoard bbPiece, BitBoard bbTurns) {
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

BitBoard TurnGenerator::calcTurns(Piece piece, BitBoard bbPiece, const ChessBoard& cb) {
    PlayerColor opp = (piece.player == White) ? Black : White;

    switch (piece.type) {
    case King:   return calcKingTurns  (bbPiece, cb.bb[piece.player][AllPieces]);
    case Queen:  return calcQueenTurns (bbPiece, cb.bb[opp][AllPieces],
                                        cb.bb[piece.player][AllPieces] | cb.bb[opp][AllPieces]);
    case Bishop: return calcBishopTurns(bbPiece, cb.bb[opp][AllPieces],
                                        cb.bb[piece.player][AllPieces] | cb.bb[opp][AllPieces]);
    case Knight: return calcKnightTurns(bbPiece, cb.bb[piece.player][AllPieces]);
    case Rook:   return calcRookTurns  (bbPiece, cb.bb[opp][AllPieces],
                                        cb.bb[piece.player][AllPieces] | cb.bb[opp][AllPieces]);
    case Pawn:   return calcPawnTurns  (bbPiece, cb.bb[opp][AllPieces],
                                        cb.bb[piece.player][AllPieces] | cb.bb[opp][AllPieces], piece.player);
    default:     return 0;
    }
}

BitBoard TurnGenerator::calcKingTurns(BitBoard king, BitBoard allOwnPieces) {

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

BitBoard TurnGenerator::calcKnightTurns(BitBoard knights, BitBoard allOwnPieces) {
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
                                      BitBoard allPieces, PlayerColor player) {

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
                                       BitBoard allPieces) {
    return calcRookTurns  (queens, allOppPieces, allPieces) |
           calcBishopTurns(queens, allOppPieces, allPieces);
}

BitBoard TurnGenerator::calcBishopTurns(BitBoard bishops, BitBoard allOppPieces,
                                        BitBoard allPieces) {
    return 0;

    BitBoard bbNE = getBitsNE(bishops);
    BitBoard bbNW = getBitsNW(bishops);
    BitBoard bbSE = getBitsSE(bishops);
    BitBoard bbSW = getBitsSW(bishops);


    LOG(info) << bitBoardToString(bbNE);
    //LOG(info) << bitBoardToString(bbNW);
    //LOG(info) << bitBoardToString(bbSE);
    //LOG(info) << bitBoardToString(bbSW);


    // north east moves
    BitBoard neMoves = bbNE & allPieces;
    neMoves = (neMoves << 9) | (neMoves << 18) | (neMoves << 27) | (neMoves << 36) |
              (neMoves << 45) | (neMoves << 54);
    neMoves &= bbNE;
    neMoves ^= bbNE;
    neMoves &= (allOppPieces | ~(allPieces));

    // north west moves
    BitBoard nwMoves = bbNW & allPieces;

    nwMoves &= bbNW;
    nwMoves ^= bbNW;
    nwMoves &= (allOppPieces | ~(allPieces));

    // south east moves
    BitBoard seMoves = bbSE & allPieces;

    seMoves &= bbSE;
    seMoves ^= bbSE;
    seMoves &= (allOppPieces | ~(allPieces));

    // south west moves
    BitBoard swMoves = bbSW & allPieces;

    swMoves &= bbSW;
    swMoves ^= bbSW;
    swMoves &= (allOppPieces | ~(allPieces));

    return neMoves | nwMoves | seMoves | swMoves;
}

BitBoard TurnGenerator::calcRookTurns(BitBoard rooks, BitBoard allOppPieces,
                                      BitBoard allPieces) {
    BitBoard rightBits = getRightBits(rooks);
    BitBoard leftBits  = getLeftBits (rooks);
    BitBoard upperBits = getUpperBits(rooks);
    BitBoard lowerBits = getLowerBits(rooks);

    // right moves
    BitBoard rightMoves = rightBits & allPieces;
    rightMoves = (rightMoves << 1) | (rightMoves << 2) | (rightMoves << 3) |
                 (rightMoves << 4) | (rightMoves << 5) | (rightMoves << 6) |
                 (rightMoves << 7);
    //rightMoves = 0xFE00000000000000 >> (56-LSB von rightMoves);
    rightMoves &= rightBits;
    rightMoves ^= rightBits;
    rightMoves &= (allOppPieces | ~(allPieces));

    // left moves
    BitBoard leftMoves = leftBits & allPieces;
    leftMoves = 0xFE00000000000000 >> (64-BB_SCAN(leftMoves));
    leftMoves &= leftBits;
    leftMoves ^= leftBits;
    leftMoves &= (allOppPieces | ~(allPieces));

    // up moves
    BitBoard upMoves = upperBits & allPieces;
    upMoves = (upMoves <<  8) | (upMoves << 16) | (upMoves << 24) |
              (upMoves << 32) | (upMoves << 40) | (upMoves << 48) |
              (upMoves << 56);
    //upMoves = 0x0101010101010100 << LSB;
    upMoves &= upperBits;
    upMoves ^= upperBits;
    upMoves &= (allOppPieces | ~(allPieces));

    // down moves
    BitBoard downMoves = lowerBits & allPieces;
    downMoves = 0x0101010101010100 >> (64-BB_SCAN(downMoves));
    downMoves &= lowerBits;
    downMoves ^= lowerBits;
    downMoves &= (allOppPieces | ~(allPieces));

    return rightMoves | leftMoves | upMoves | downMoves;
}



BitBoard TurnGenerator::getBitsNE(BitBoard bbPiece) {
    int field = BB_SCAN(bbPiece);
    BitBoard bb = 0x8040201008040200 << field;

    return bb;
}

BitBoard TurnGenerator::getBitsNW(BitBoard bbPiece) {
    return 0;
}

BitBoard TurnGenerator::getBitsSE(BitBoard bbPiece) {
    int field = BB_SCAN(bbPiece);
    BitBoard bb = 0x8040201008040200 >> (56-field);

    return bb;
}

BitBoard TurnGenerator::getBitsSW(BitBoard bbPiece) {
    return 0;
}





BitBoard TurnGenerator::getRightBits(BitBoard bbPiece) {
    int field = BB_SCAN(bbPiece);
    BitBoard bb = 0xFE00000000000000 >> (56-field);
    bb &= maskRank(static_cast<Rank>(field / 8));

    // TODO: Shift mit mehr als 32 geht nicht?
    //BitBoard bb = 0x00000000000000FE << 32;
    //bb &= maskRank(static_cast<Rank>(field / 8));

    return bb;
}

BitBoard TurnGenerator::getLeftBits(BitBoard bbPiece) {
    int field = BB_SCAN(bbPiece);
    BitBoard bb = 0xFE00000000000000 >> (64-field);
    bb &= maskRank(static_cast<Rank>(field / 8));

    return bb;
}

BitBoard TurnGenerator::getUpperBits(BitBoard bbPiece) {
    int field = BB_SCAN(bbPiece);
    BitBoard bb = 0x0101010101010100 << field;

    return bb;
}

BitBoard TurnGenerator::getLowerBits(BitBoard bbPiece) {
    int field = BB_SCAN(bbPiece);
    BitBoard bb = 0x0101010101010100 >> (64-field);

    return bb;
}



BitBoard TurnGenerator::maskRank(Rank rank) {
    BitBoard bb = 0;
    int offset = rank * 8;

    BIT_SET(bb, offset);
    BIT_SET(bb, offset + 1);
    BIT_SET(bb, offset + 2);
    BIT_SET(bb, offset + 3);
    BIT_SET(bb, offset + 4);
    BIT_SET(bb, offset + 5);
    BIT_SET(bb, offset + 6);
    BIT_SET(bb, offset + 7);

    return bb;
}

BitBoard TurnGenerator::clearRank(Rank rank) {
    BitBoard bb = ULLONG_MAX;
    int offset = rank * 8;

    BIT_CLEAR(bb, offset);
    BIT_CLEAR(bb, offset + 1);
    BIT_CLEAR(bb, offset + 2);
    BIT_CLEAR(bb, offset + 3);
    BIT_CLEAR(bb, offset + 4);
    BIT_CLEAR(bb, offset + 5);
    BIT_CLEAR(bb, offset + 6);
    BIT_CLEAR(bb, offset + 7);

    return bb;
}

BitBoard TurnGenerator::maskFile(File file) {
    BitBoard bb = 0;

    BIT_SET(bb, file);
    BIT_SET(bb, file + 8);
    BIT_SET(bb, file + 16);
    BIT_SET(bb, file + 24);
    BIT_SET(bb, file + 32);
    BIT_SET(bb, file + 40);
    BIT_SET(bb, file + 48);
    BIT_SET(bb, file + 56);

    return bb;
}

BitBoard TurnGenerator::clearFile(File file) {
    BitBoard bb = ULLONG_MAX;

    BIT_CLEAR(bb, file);
    BIT_CLEAR(bb, file + 8);
    BIT_CLEAR(bb, file + 16);
    BIT_CLEAR(bb, file + 24);
    BIT_CLEAR(bb, file + 32);
    BIT_CLEAR(bb, file + 40);
    BIT_CLEAR(bb, file + 48);
    BIT_CLEAR(bb, file + 56);

    return bb;
    // TODO: Gehts so auch?
    //bitBoard &= ~((BitBoard)1 << file << file + 8 << file + 16);
}
