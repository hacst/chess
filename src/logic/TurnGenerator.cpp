#include "TurnGenerator.h"

std::vector<Turn> TurnGenerator::generateTurns(PlayerColor player, const ChessBoard& cb) {
    std::array<BitBoard, 6> bbTurns = calcTurns(player, cb);
    std::vector<Turn>       turns, allTurns;

    Piece piece;
    for (int pieceType = King; pieceType <= Pawn ; pieceType++) {
        piece.type   = (PieceType) pieceType;
        piece.player = player;

        turns = bitBoardToTurns(piece, bbTurns[pieceType], cb.bb[player][pieceType]);
        allTurns.insert(allTurns.end(), turns.begin(), turns.end());
    }

    return allTurns;
}




std::vector<Turn> TurnGenerator::bitBoardToTurns(Piece piece, BitBoard bbTurns, BitBoard bbPiece) {
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


std::array<BitBoard, 6> TurnGenerator::calcTurns(PlayerColor player, const ChessBoard& cb) {
    std::array<BitBoard, 6> bbTurns;
    PlayerColor opp = (player == White) ? Black : White;

    bbTurns[King]   = calcKingTurns  (cb.bb[player][King],   cb.bb[player][AllPieces]);
    bbTurns[Queen]  = calcQueenTurns (cb.bb[player][Queen],  cb.bb[player][AllPieces]);
    bbTurns[Bishop] = calcBishopTurns(cb.bb[player][Bishop], cb.bb[player][AllPieces]);
    bbTurns[Knight] = calcKnightTurns(cb.bb[player][Knight], cb.bb[player][AllPieces]);
    bbTurns[Rook]   = calcRookTurns  (cb.bb[player][Rook],   cb.bb[player][AllPieces]);
    bbTurns[Pawn]   = calcPawnTurns  (cb.bb[player][Pawn],   cb.bb[opp][AllPieces],
                                      cb.bb[player][AllPieces] | cb.bb[opp][AllPieces],
                                      player);

    // TODO: Am Ende pruefen, ob der gegnerische King Schach oder Schachmatt
    // gesetzt wurde -> gs.check(White)

    return bbTurns;
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
    // im schach? <- Diese zuege entfernen
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
    BitBoard oneStep;
    BitBoard twoSteps;
    if (player == White) {
        oneStep  = (pawns                       << 8) & ~allPieces;
        twoSteps = ((oneStep & maskRank(Three)) << 8) & ~allPieces;
    } else if (player == Black) {
        oneStep  = (pawns                       >> 8) & ~allPieces;
        twoSteps = ((oneStep & maskRank(Six))   >> 8) & ~allPieces;
    }
    BitBoard pawnMoves = oneStep | twoSteps;

    // Pawn Attacks
    BitBoard leftAttacks;
    BitBoard rightAttacks;
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

BitBoard TurnGenerator::calcQueenTurns(BitBoard queens, BitBoard allOwnPieces) {
    return 0;
}

BitBoard TurnGenerator::calcBishopTurns(BitBoard bishops, BitBoard allOwnPieces) {
    return 0;
}

BitBoard TurnGenerator::calcRookTurns(BitBoard rooks, BitBoard allOwnPieces) {
    return 0;
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
