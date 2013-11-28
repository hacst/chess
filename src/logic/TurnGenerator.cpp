#include "TurnGenerator.h"

std::vector<Turn> TurnGenerator::generateTurns(PlayerColor player, const ChessBoard& chessBoard) {

    //TODO: updating bitboards or generating bitboards from chessboard
    // oder in eigener applyTurn Methode
    generateBitBoards(chessBoard);

    std::array<BitBoard, 6> bitBoards = calcTurns(player);
    std::vector<Turn> turns, allTurns;

    for (BitBoard b: bitBoards) {
        turns = bitBoardToTurns(b);
        allTurns.insert(allTurns.end(), turns.begin(), turns.end());
    }

    return allTurns;
}




/*

void TurnGenerator::initBitBoards() {
    // set BitBoards to start positions

    BIT_SET(m_king[White], 5);


    BIT_SET(m_king[White], Field::E1);
    BIT_SET(m_queens[White], Field::D1);
    BIT_SET(m_bishops[White], Field::C1); // TODO: do this in 1 step
    BIT_SET(m_bishops[White], Field::F1);
    BIT_SET(m_knights[White], Field::B1);
    BIT_SET(m_knights[White], Field::G1);
    BIT_SET(m_pawns[White], Field::A2);
    BIT_SET(m_pawns[White], Field::B2);
    BIT_SET(m_pawns[White], Field::C2);
    BIT_SET(m_pawns[White], Field::D2);
    BIT_SET(m_pawns[White], Field::E2);
    BIT_SET(m_pawns[White], Field::F2);
    BIT_SET(m_pawns[White], Field::G2);
    BIT_SET(m_pawns[White], Field::H2);

    BIT_SET(m_king[Black], E8);
    BIT_SET(m_queens[Black], D8);
    BIT_SET(m_bishops[Black], C8);
    BIT_SET(m_bishops[Black], F8);
    BIT_SET(m_knights[Black], B8);
    BIT_SET(m_knights[Black], G8);
    BIT_SET(m_pawns[Black], A7);
    BIT_SET(m_pawns[Black], B7);
    BIT_SET(m_pawns[Black], C7);
    BIT_SET(m_pawns[Black], D7);
    BIT_SET(m_pawns[Black], E7);
    BIT_SET(m_pawns[Black], F7);
    BIT_SET(m_pawns[Black], G7);
    BIT_SET(m_pawns[Black], H7);

    updateBitBoards();
}
*/

void TurnGenerator::generateBitBoards(const ChessBoard& chessBoard) {
    int i;
    std::array<Piece, 64> board = chessBoard.getBoard();

    for (i = 0; i < 2; i++) {
        m_pawns[i] = 0;
        m_rooks[i] = 0;
        m_knights[i] = 0;
        m_bishops[i] = 0;
        m_queens[i] = 0;
        m_king[i] = 0;
    }

    for (i = 0; i < 64; i++) {
        switch (board[i]) {
        case WhiteKing: BIT_SET(m_king[White], i); break;
        case BlackKing: BIT_SET(m_king[Black], i); break;
        case WhiteQueen: BIT_SET(m_queens[White], i); break;
        case BlackQueen: BIT_SET(m_queens[Black], i); break;
        case WhiteBishop: BIT_SET(m_bishops[White], i); break;
        case BlackBishop: BIT_SET(m_bishops[Black], i); break;
        case WhiteKnight: BIT_SET(m_knights[White], i); break;
        case BlackKnight: BIT_SET(m_knights[Black], i); break;
        case WhiteRook: BIT_SET(m_rooks[White], i); break;
        case BlackRook: BIT_SET(m_rooks[Black], i); break;
        case WhitePawn: BIT_SET(m_pawns[White], i); break;
        case BlackPawn: BIT_SET(m_pawns[Black], i); break;
        case Empty: break;
        default: break;
        }
    }

    m_allPieces[White] = m_pawns[White] | m_rooks[White] | m_knights[White] |
                         m_bishops[White] | m_queens[White] | m_king[White];
    m_allPieces[Black] = m_pawns[Black] | m_rooks[Black] | m_knights[Black] |
                         m_bishops[Black] | m_queens[Black] | m_king[Black];
    m_allPieces[None]  = m_allPieces[White] | m_allPieces[Black];
}





std::array<BitBoard, 6> TurnGenerator::calcTurns(PlayerColor player) {
    std::array<BitBoard, 6> bitBoards;

    bitBoards[0] = 0; // calcQueenTurns(m_queens[player], m_allPieces[player]);
    bitBoards[1] = 0; // calcBishopTurns(m_bishops[player], board->m_allPieces[player]);
    bitBoards[2] = 0; // calcRookTurns(m_rooks[player], m_allPieces[player]);
    bitBoards[3] = calcKingTurns(m_king[player], m_allPieces[player]);
    bitBoards[4] = 0; // calcKnightTurns(m_knights[player], m_allPieces[player]);
    bitBoards[5] = 0; // calcPawnTurns(m_pawns[player], m_allPieces[player]);

    return bitBoards;
}

BitBoard TurnGenerator::calcKingTurns(BitBoard king, BitBoard allOwnPieces) {
    //king = king & ChessBoard::clearFile(File.A);
    //king = king & ChessBoard::clearFile(File.H);

    BitBoard turn1 = king << 9;
    BitBoard turn2 = king << 8;
    BitBoard turn3 = king << 7;
    BitBoard turn4 = king << 1;
    BitBoard turn5 = king >> 1;
    BitBoard turn6 = king >> 7;
    BitBoard turn7 = king >> 8;
    BitBoard turn8 = king >> 9;

    BitBoard kingTurns = turn1 | turn2 | turn3 | turn4 |
                         turn5 | turn6 | turn7 | turn8;
    kingTurns &= ~allOwnPieces;

    // TODO: Rochade

    // TODO: schach pruefen
    // dazu alle möeglichen zuege des anderen spielers berechnen
    // (in einem bb) und dann verunden mit king bb. ergebniss bb
    // enthaelt felder, die der king nicht betreten darf, also
    // nochmal xor verknüpfen

    return kingTurns;
}

std::vector<Turn> TurnGenerator::bitBoardToTurns(BitBoard b) {
    std::vector<Turn> turns;

    //turns.push_back(Turn::);

    return turns;
}

std::string bitBoardToString(BitBoard b) {
    stringstream ss;
    int i, j;
    int bits = sizeof(b) * 8;

    ss << endl << "decimal: " << b;
    for (i = bits; i > 0; i -= 8) {
        if (!(i % 8)) ss << endl;
        for (j = 0; j < 8; j++) {
            if (BIT_ISSET(b, j+(i-8))) {
                ss << 1;
            } else {
                ss << 0;
            }
        }
    }
    ss << endl;
    return ss.str();
}
