#include "TurnGenerator.h"

void TurnGenerator::initFlags(ChessBoard &cb) {
    BitBoard bbAllOppTurns = calcAllOppTurns(Black, cb);
    BitBoard bbKingInCheck = cb.m_bb[White][King] & bbAllOppTurns;
    if (bbKingInCheck == cb.m_bb[White][King]) {
        cb.setKingInCheck(White, true);
    }

    bbAllOppTurns = calcAllOppTurns(White, cb);
    bbKingInCheck = cb.m_bb[Black][King] & bbAllOppTurns;
    if (bbKingInCheck == cb.m_bb[Black][King]) {
        cb.setKingInCheck(Black, true);
    }
}

std::vector<Turn> TurnGenerator::getTurnList() const {
    return turnList;
}

void TurnGenerator::generateTurns(PlayerColor player, ChessBoard &cb) {
    PlayerColor opp = togglePlayerColor(player);
    Field curPiecePos;
    Piece piece;

    BitBoard bbCurPieceType, bbCurPiece, bbTurns;
    BitBoard bbAllPieces   = cb.m_bb[White][AllPieces] | cb.m_bb[Black][AllPieces];
    BitBoard bbAllOppTurns = calcAllOppTurns(opp, cb);
    BitBoard bbKingInCheck = cb.m_bb[player][King] & bbAllOppTurns;

    turnList.clear();

    // Der gegnerische King kann in keinem (korrekten) Fall im Schach stehen
    // wenn man selbst an der Reihe ist, da sonst das Spiel beendet waere,
    // allerdings kann das Flag noch vom vorherigen Halbzug gesetzt sein,
    // daher zuruecksetzen
    if (cb.getKingInCheck()[opp]) {
        // Immernoch im Schach? -> Kann nur der Fall sein, wenn der GameState
        // aus einem ungueltigen (bereits "beendetem") Chessboard geladen wurde!
        BitBoard bb = calcAllOppTurns(player, cb);
        BitBoard bb2 = cb.m_bb[opp][King] & bb;
        if (bb2 == cb.m_bb[opp][King]) {
            cb.setCheckmate(opp);
            return;
        }

        cb.setKingInCheck(opp, false);
    }

    if (bbKingInCheck == cb.m_bb[player][King]) {
        // Wenn King im Schach, dann nur Zuege berechnen um aus dem Schach
        // raus zu kommen. Wenn keine Zuege gefunden -> Schachmatt
        cb.setKingInCheck(player, true);

        // Um die legalen Zuege der Figuren (ausgenommen King) herauszufinden:
        // Welche gegnerische Figur setzt den King ins Schach?
        // -> Diese Figur entweder schlagen oder den Weg abschneiden
        // -> Von dieser Figur wird also die Position benoetigt und (bei einem
        //    sliding piece) die Turns, die ihm zum King fuehren (der "Weg")
        BitBoard bbUnCheckFields = calcUnCheckFields(opp, cb);

        // Normale Zugberechnung, ABER: Turns werden mit bbUnCheckFields verundet
        for (int pieceType = King; pieceType <= Pawn ; pieceType++) {
            piece.type   = (PieceType) pieceType;
            piece.player = player;

            bbCurPieceType = cb.m_bb[player][pieceType];
            while (bbCurPieceType != 0) {
                bbCurPiece  = 0;
                curPiecePos = BB_SCAN(bbCurPieceType);
                BIT_CLEAR(bbCurPieceType, curPiecePos);
                BIT_SET  (bbCurPiece,     curPiecePos);
                bbTurns = calcMoveTurns(piece, bbCurPiece, bbAllOppTurns, cb);

                if (pieceType != King) {
                    bbTurns &= bbUnCheckFields;
                }

                bitBoardToTurns(
                    piece, (Field) curPiecePos,
                    bbTurns, bbAllOppTurns, cb,
                    turnList);
            }
        }

        if (turnList.empty()) {
            cb.setCheckmate(player);
        }

    } else {
        // Normale Zugberechnung durchfuehren; werden keine Zuege gefunden
        // liegt eine Pattstellung vor
        cb.setKingInCheck(player, false);

        // short castle turns
        if (cb.m_shortCastleRight[player]) {
            BitBoard bbShortCastleKingTurn = calcShortCastleTurns(player,
                                                                  bbAllPieces,
                                                                  bbAllOppTurns);
            if (bbShortCastleKingTurn != 0) {
                if (player == White) {
                    turnList.push_back(Turn::castle(Piece(White, King), E1, G1));
                } else {
                    turnList.push_back(Turn::castle(Piece(Black, King), E8, G8));
                }
            }
        }

        // long castle turns
        if (cb.m_longCastleRight[player]) {
            BitBoard bbLongCastleKingTurn = calcLongCastleTurns(player,
                                                                bbAllPieces,
                                                                bbAllOppTurns);
            if (bbLongCastleKingTurn != 0) {
                if (player == White) {
                    turnList.push_back(Turn::castle(Piece(White, King), E1, C1));
                } else {
                    turnList.push_back(Turn::castle(Piece(Black, King), E8, C8));
                }
            }
        }

        // move turns
        for (int pieceType = King; pieceType <= Pawn ; pieceType++) {
            piece.type   = (PieceType) pieceType;
            piece.player = player;

            bbCurPieceType = cb.m_bb[player][pieceType];
            while (bbCurPieceType != 0) {
                bbCurPiece  = 0;
                curPiecePos = BB_SCAN(bbCurPieceType);
                BIT_CLEAR(bbCurPieceType, curPiecePos);
                BIT_SET  (bbCurPiece,     curPiecePos);
                bbTurns     = calcMoveTurns(piece, bbCurPiece, bbAllOppTurns, cb);

                bitBoardToTurns(piece, (Field) curPiecePos,
                                bbTurns, bbAllOppTurns, cb,
                                turnList);
            }
        }

        if (turnList.empty()) {
            cb.setStalemate();
        }
    }

    //turnList.push_back(Turn::Forfeit);
}

void TurnGenerator::bitBoardToTurns(Piece piece,
                                                 Field from,
                                                 BitBoard bbTurns,
                                                 BitBoard bbAllOppTurns,
                                                 ChessBoard& cb,
                                                 Turns& turnsOut) {
    Field to;

    while (bbTurns != 0) {
        to = BB_SCAN(bbTurns);
        BIT_CLEAR(bbTurns, to);

        // Pruefung: Eine Figur darf den eigenen King nicht durch einen Zug
        // in Schach setzen (z.B. "Weg freimachen")
        // Ist die Position der aktuellen Figur bei den allOppTurns enthalten?
        // -> Wenn ja, dann verhindert die Figur evtl. dass der King im Schach
        //    steht.
        if (BIT_ISSET(bbAllOppTurns, from)) {
            const PlayerColor opp = togglePlayerColor(piece.player);
            bool captured = false;
            PieceType capturedPieceType = NoType;
            // Zug anwenden (ziehen und evtl. schlagen)
            BIT_CLEAR(cb.m_bb[piece.player][piece.type], from);
            BIT_SET(cb.m_bb[piece.player][piece.type], to);
            for (int pieceType = King; pieceType < NUM_PIECETYPES; pieceType++) {
                if (BIT_ISSET(cb.m_bb[opp][pieceType], to)) {
                    BIT_CLEAR(cb.m_bb[opp][pieceType], to);
                    captured = true;
                    capturedPieceType = static_cast<PieceType>(pieceType);
                    break;
                }
            }
            cb.updateBitBoards();

            // Danach pruefen ob King im Schach steht
            BitBoard bbNewOppTurns = calcAllOppTurns(opp, cb);
            BitBoard bbKingInCheck = cb.m_bb[piece.player][King] & bbNewOppTurns;

            // Zug wieder rueckgaengig machen!
            BIT_CLEAR(cb.m_bb[piece.player][piece.type], to);
            BIT_SET(cb.m_bb[piece.player][piece.type], from);
            if (captured) BIT_SET(cb.m_bb[opp][capturedPieceType], to);
            cb.updateBitBoards();

            // Falls King im Schach, Zug nicht aufnehmen
            if (bbKingInCheck == cb.m_bb[piece.player][King]) {
                continue;
            }
        }



        if ((rankFor(to) == Eight || rankFor(to) == One) && piece.type == Pawn) {
            turnsOut.push_back(Turn::promotionQueen(piece, from, to));
            turnsOut.push_back(Turn::promotionBishop(piece, from, to));
            turnsOut.push_back(Turn::promotionRook(piece, from, to));
            turnsOut.push_back(Turn::promotionKnight(piece, from, to));

        } else {
            turnsOut.push_back(Turn::move(piece, from, to));
        }
    }
}

BitBoard TurnGenerator::calcMoveTurns(Piece piece,
                                      BitBoard bbPiece,
                                      BitBoard bbAllOppTurns,
                                      const ChessBoard& cb) {
    PlayerColor opp = (piece.player == White) ? Black : White;

    switch (piece.type) {
    case King:   return calcKingTurns  (bbPiece,
                                        cb.m_bb[piece.player][AllPieces],
                                        bbAllOppTurns);
    case Queen:  return calcQueenTurns (bbPiece,
                                        cb.m_bb[opp][AllPieces],
                                        cb.m_bb[piece.player][AllPieces] |
                                        cb.m_bb[opp][AllPieces]);
    case Bishop: return calcBishopTurns(bbPiece,
                                        cb.m_bb[opp][AllPieces],
                                        cb.m_bb[piece.player][AllPieces] |
                                        cb.m_bb[opp][AllPieces]);
    case Knight: return calcKnightTurns(bbPiece,
                                        cb.m_bb[piece.player][AllPieces]);
    case Rook:   return calcRookTurns  (bbPiece,
                                        cb.m_bb[opp][AllPieces],
                                        cb.m_bb[piece.player][AllPieces] |
                                        cb.m_bb[opp][AllPieces]);
    case Pawn:   return calcPawnTurns  (bbPiece,
                                        cb.m_bb[opp][AllPieces],
                                        cb.m_bb[piece.player][AllPieces] |
                                        cb.m_bb[opp][AllPieces],
                                        piece.player,
                                        cb.m_enPassantSquare);
    default:     return 0;
    }
}

// Berechnet die Felder, zu denen eine der eigenen Figuren ziehen muss
// um den King aus dem Schach zu holen
BitBoard TurnGenerator::calcUnCheckFields(PlayerColor opp,
                                          const ChessBoard& cb) {
    Piece piece;
    piece.player = opp;
    Field curPiecePos;
    PlayerColor player = togglePlayerColor(opp);
    BitBoard bbCurPieceType, bbCurPiece, bbTurns, bbKingInCheck;
    BitBoard bbUnCheckFields = 0;

    for (int pieceType = King; pieceType <= Pawn ; pieceType++) {
        piece.type     = (PieceType) pieceType;
        bbCurPieceType = cb.m_bb[opp][pieceType];

        while (bbCurPieceType != 0) {
            bbCurPiece  = 0;
            curPiecePos = BB_SCAN(bbCurPieceType);
            BIT_CLEAR(bbCurPieceType, curPiecePos);
            BIT_SET  (bbCurPiece,     curPiecePos);

            bbTurns = calcMoveTurns(piece, bbCurPiece, 0, cb);
            bbKingInCheck = cb.m_bb[player][King] & bbTurns;

            // Zunaechst herausfinden, welche gegnerische Figur den King
            // ins Schach setzt
            if (bbKingInCheck == cb.m_bb[player][King]) {
                if (pieceType == Pawn || pieceType == Knight || pieceType == King) {
                    // Bei den nonsliding pieces ist das gesuchte "unCheckField"
                    // nur die Position der gegnerischen Figur
                    BIT_SET(bbUnCheckFields, curPiecePos);
                    return bbUnCheckFields;

                } else {
                    // Bei den sliding pieces kommen alle Felder in Frage, die der
                    // gegnerischen Figur, die den King in Schach setzt, den Weg
                    // "abschneiden" koennen.


                    // TODO: Die Pruefung kann eigentlich entfallen, da die
                    // King-BitBoards immer != 0 sind. Nur aufgrund der Testfaelle
                    // vorhanden.
                    Field kingPos = ERR;
                    if (cb.m_bb[player][King] != 0) {
                        kingPos = BB_SCAN(cb.m_bb[player][King]);
                    }


                    Rank kingRank = rankFor(kingPos);
                    File kingFile = fileFor(kingPos);
                    Rank pieceRank = rankFor(curPiecePos);
                    File pieceFile = fileFor(curPiecePos);

                    int dist = -10;
                    int rankSign = -10;
                    int fileSign = -10;

                    if (kingRank < pieceRank) {
                        // SW, S, SE
                        dist = pieceRank - kingRank - 1;
                        rankSign = -1;

                        if (kingFile < pieceFile) {
                            // SW
                            fileSign = -1;
                        } else if (kingFile == pieceFile) {
                            // S
                            fileSign = 0;
                        } else if (kingFile > pieceFile) {
                            // SE
                            fileSign = 1;
                        }

                    } else if (kingRank == pieceRank) {
                        // W, E
                        rankSign = 0;

                        if (kingFile < pieceFile) {
                            // W
                            dist = pieceFile - kingFile - 1;
                            fileSign = -1;

                        } else if (kingFile > pieceFile) {
                            // E
                            dist = kingFile - pieceFile - 1;
                            fileSign = 1;
                        }

                    } else if (kingRank > pieceRank) {
                        // NW, N, NE
                        dist = kingRank - pieceRank - 1;
                        rankSign = 1;

                        if (kingFile < pieceFile) {
                            // NW
                            fileSign = -1;
                        } else if (kingFile == pieceFile) {
                            // N
                            fileSign = 0;
                        } else if (kingFile > pieceFile) {
                            // NE
                            fileSign = 1;
                        }
                    }

                    int field = 0;
                    for (int i = 1; i <= dist; i++) {
                        field = (pieceRank + (i*rankSign)) * 8 +
                                (pieceFile + (i*fileSign));
                        BIT_SET(bbUnCheckFields, field);
                    }

                    // Und die Position der gegnerischen Figur
                    BIT_SET(bbUnCheckFields, curPiecePos);

                    return bbUnCheckFields;
                }
            }
        }
    }

    return 0;
}

// Berechnet alle Felder, die der Gegner momentan ANGREIFEN koennte, d.h.:
// * Bei den Pawns werden alle potenziellen Angriffszuege benoetigt; die
//   einfachen Move-Turns der Pawns gehoeren nicht dazu!
// * Bei den sliding pieces muessen auch die Felder berechnet werden,
//   die "hinter" dem King liegen
BitBoard TurnGenerator::calcAllOppTurns(PlayerColor opp,
                                        const ChessBoard& cb) {
    Piece piece;
    Field curPiecePos;
    PlayerColor player = togglePlayerColor(opp);

    BitBoard bbAllOppTurns = 0;
    BitBoard bbAllPieces = cb.m_bb[White][AllPieces] | cb.m_bb[Black][AllPieces];
    //BitBoard bbAllOppPiecesWhitoutKing = cb.m_bb[player][AllPieces] ^ cb.m_bb[player][King];
    BitBoard bbAllPiecesWhitoutKing = bbAllPieces ^ cb.m_bb[player][King];

    BitBoard bbCurPieceType, bbCurPiece;


    // short castle
    if (cb.m_shortCastleRight[opp]) {
        bbAllOppTurns |= calcShortCastleTurns(opp, bbAllPieces, 0);
    }
    // long castle
    if (cb.m_longCastleRight[opp]) {
        bbAllOppTurns |= calcLongCastleTurns(opp, bbAllPieces, 0);
    }

    // move turns
    for (int pieceType = King; pieceType <= Pawn ; pieceType++) {
        piece.type   = (PieceType) pieceType;
        piece.player = opp;

        bbCurPieceType = cb.m_bb[opp][pieceType];


        if (pieceType == Pawn) {
            // get all potential attacks of the pawns
            bbAllOppTurns |= calcPawnAttackTurns(bbCurPieceType,
                                                 0xFFFFFFFFFFFFFFFF,
                                                 opp,
                                                 cb.m_enPassantSquare);

        } else if (pieceType == King) {
            bbAllOppTurns |= calcKingTurns(bbCurPieceType, 0, 0);

        } else if (pieceType == Knight) {
            bbAllOppTurns |= calcKnightTurns(bbCurPieceType, 0);

        } else {
            // sliding pieces
            while (bbCurPieceType != 0) {
                bbCurPiece  = 0;
                curPiecePos = BB_SCAN(bbCurPieceType);
                BIT_CLEAR(bbCurPieceType, curPiecePos);
                BIT_SET  (bbCurPiece,     curPiecePos);

                if (pieceType == Rook) {
                    bbAllOppTurns |= calcRookTurns(bbCurPiece,
                                                   bbAllPiecesWhitoutKing, //bbAllOppPiecesWhitoutKing,
                                                   bbAllPiecesWhitoutKing);
                } else if (pieceType == Queen) {
                    bbAllOppTurns |= calcQueenTurns(bbCurPiece,
                                                    bbAllPiecesWhitoutKing, //bbAllOppPiecesWhitoutKing,
                                                    bbAllPiecesWhitoutKing);
                } else if (pieceType == Bishop) {
                    bbAllOppTurns |= calcBishopTurns(bbCurPiece,
                                                     bbAllPiecesWhitoutKing, //bbAllOppPiecesWhitoutKing,
                                                     bbAllPiecesWhitoutKing);
                }
            }
        }
    }

    return bbAllOppTurns;
}

BitBoard TurnGenerator::calcShortCastleTurns(PlayerColor player,
                                             BitBoard bbAllPieces,
                                             BitBoard bbAllOppTurns) {
    BitBoard bbShortCastleKingTurn = 0;

    if (player == White) {
        if (((bbAllOppTurns & generateBitBoard(E1, F1, G1, ERR)) == 0) &&
                !BIT_ISSET(bbAllPieces, G1) &&
                !BIT_ISSET(bbAllPieces, F1)) {
            BIT_SET(bbShortCastleKingTurn, G1);
        }
    } else {
        if (((bbAllOppTurns & generateBitBoard(E8, F8, G8, ERR)) == 0) &&
                !BIT_ISSET(bbAllPieces, G8) &&
                !BIT_ISSET(bbAllPieces, F8)) {
            BIT_SET(bbShortCastleKingTurn, G8);
        }
    }

    return bbShortCastleKingTurn;
}

BitBoard TurnGenerator::calcLongCastleTurns(PlayerColor player,
                                            BitBoard bbAllPieces,
                                            BitBoard bbAllOppTurns) {
    BitBoard bbLongCastleKingTurn = 0;

    if (player == White) {
        if (((bbAllOppTurns & generateBitBoard(E1, D1, C1, ERR)) == 0) &&
                !BIT_ISSET(bbAllPieces, D1) &&
                !BIT_ISSET(bbAllPieces, C1) &&
                !BIT_ISSET(bbAllPieces, B1)) {
            BIT_SET(bbLongCastleKingTurn, C1);
        }
    } else {
        if (((bbAllOppTurns & generateBitBoard(E8, D8, C8, ERR)) == 0) &&
                !BIT_ISSET(bbAllPieces, D8) &&
                !BIT_ISSET(bbAllPieces, C8) &&
                !BIT_ISSET(bbAllPieces, B8)) {
            BIT_SET(bbLongCastleKingTurn, C8);
        }
    }

    return bbLongCastleKingTurn;
}

BitBoard TurnGenerator::calcKingTurns(BitBoard king,
                                      BitBoard allOwnPieces,
                                      BitBoard allOppTurns) const {
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

    // Der King darf auf keine Felder ziehen, auf denen er im Schach
    // stehen wuerde
    kingTurns ^= (allOppTurns & kingTurns);

    return kingTurns;
}

BitBoard TurnGenerator::calcKnightTurns(BitBoard knights,
                                        BitBoard allOwnPieces) const {
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

BitBoard TurnGenerator::calcPawnTurns(BitBoard pawns,
                                      BitBoard allOppPieces,
                                      BitBoard allPieces,
                                      PlayerColor player,
                                      Field enPassantSquare) const {
    return (calcPawnMoveTurns(pawns, allPieces, player) |
            calcPawnAttackTurns(pawns, allOppPieces, player, enPassantSquare));
}

BitBoard TurnGenerator::calcPawnMoveTurns(BitBoard pawns,
                                          BitBoard allPieces,
                                          PlayerColor player) const {
    // Pawn Moves
    BitBoard oneStep  = 0;
    BitBoard twoSteps = 0;
    if (player == White) {
        oneStep  = (pawns                       << 8) & ~allPieces;
        twoSteps = ((oneStep & maskRank(Three)) << 8) & ~allPieces;
    } else if (player == Black) {
        oneStep  = (pawns                       >> 8) & ~allPieces;
        twoSteps = ((oneStep & maskRank(Six))   >> 8) & ~allPieces;
    }
    BitBoard pawnMoves = oneStep | twoSteps;

    return pawnMoves;
}

BitBoard TurnGenerator::calcPawnAttackTurns(BitBoard pawns,
                                            BitBoard allOppPieces,
                                            PlayerColor player,
                                            Field enPassantSquare) const {
    // Pawn Attacks
    BitBoard leftAttacks  = 0;
    BitBoard rightAttacks = 0;
    if (player == White) {
        leftAttacks  = (pawns & clearFile(A)) << 7;
        rightAttacks = (pawns & clearFile(H)) << 9;
    } else if (player == Black) {
        leftAttacks  = (pawns & clearFile(A)) >> 9;
        rightAttacks = (pawns & clearFile(H)) >> 7;
    }

    // en passant
    if (enPassantSquare != ERR) {
        BIT_SET(allOppPieces, enPassantSquare);
    }

    BitBoard pawnAttacks = (leftAttacks | rightAttacks) & allOppPieces;

    return pawnAttacks;
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
    BitBoard eastMoves = bbE & allPieces;
    eastMoves = (eastMoves << 1) | (eastMoves << 2) | (eastMoves << 3) |
                (eastMoves << 4) | (eastMoves << 5) | (eastMoves << 6) |
                (eastMoves << 7);
    //rightMoves = 0xFE00000000000000 >> (56-LSB von rightMoves);
    eastMoves &= bbE;
    eastMoves ^= bbE;
    eastMoves &= (allOppPieces | ~(allPieces));

    // west moves
    BitBoard westMoves = bbW & allPieces;
    westMoves = (westMoves >> 1) | (westMoves >> 2) | (westMoves >> 3) |
                (westMoves >> 4) | (westMoves >> 5) | (westMoves >> 6) |
                (westMoves >> 7);
    //westMoves = 0xFE00000000000000 >> (64-BB_SCAN(westMoves));
    westMoves &= bbW;
    westMoves ^= bbW;
    westMoves &= (allOppPieces | ~(allPieces));

    // north moves
    BitBoard northMoves = bbN & allPieces;
    northMoves = (northMoves <<  8) | (northMoves << 16) | (northMoves << 24) |
                (northMoves << 32) | (northMoves << 40) | (northMoves << 48) |
                (northMoves << 56);
    //upMoves = 0x0101010101010100 << LSB;
    northMoves &= bbN;
    northMoves ^= bbN;
    northMoves &= (allOppPieces | ~(allPieces));


    // south moves
    BitBoard southMoves = bbS & allPieces;
    southMoves = (southMoves >> 8) | (southMoves >> 16) | (southMoves >> 24) |
                (southMoves >> 32) | (southMoves >> 40) | (southMoves >> 48) |
                (southMoves >> 56);
    //downMoves = 0x0101010101010100 >> (64-BB_SCAN(downMoves));
    southMoves &= bbS;
    southMoves ^= bbS;
    southMoves &= (allOppPieces | ~(allPieces));

    return eastMoves | westMoves | northMoves | southMoves;
}
