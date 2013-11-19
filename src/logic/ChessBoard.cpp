#include "ChessBoard.h"

void ChessBoard::applyTurn(const Turn &t) {

    if (turn.action == Turn::Move) {
        m_board[t.from] = Empty;
        m_board[t.to] = t.piece;

        /*
        auto& from = board(turn.from.x, turn.from.y);
        auto& to = board(turn.to.x, turn.to.y);

        if (to != Board::Field::Empty) {
            LOG(trace) << from << " strikes " << to;
        }

        to = from;
        from = Board::Field::Empty;
        */


    } else if (turn.action == Turn::Forfeit) {
        //TODO: Do something
    } else {
        // Assume passed
    }

    updateBitBoards();
}



void ChessBoard::initBitBoards() {
    /* set BitBoards to start positions */

    BIT_SET(m_king[White], E1);
    BIT_SET(m_queens[White], D1);
    BIT_SET(m_bishops[White], C1); // TODO: do this in 1 step
    BIT_SET(m_bishops[White], F1);
    BIT_SET(m_knights[White], B1);
    BIT_SET(m_knights[White], G1);
    BIT_SET(m_pawns[White], A2);
    BIT_SET(m_pawns[White], B2);
    BIT_SET(m_pawns[White], C2);
    BIT_SET(m_pawns[White], D2);
    BIT_SET(m_pawns[White], E2);
    BIT_SET(m_pawns[White], F2);
    BIT_SET(m_pawns[White], G2);
    BIT_SET(m_pawns[White], H2);

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

    /*
    BIT_SET(m_wKing, Field.E1);
    BIT_SET(m_wQueens, Field.D1);
    BIT_SET(m_wBishops, Field.C1);
    BIT_SET(m_wBishops, Field.F1);
    BIT_SET(m_wKnights, Field.B1);
    BIT_SET(m_wKnights, Field.G1);
    BIT_SET(m_wRooks, Field.A1);
    BIT_SET(m_wRooks, Field.H1);
    BIT_SET(m_wPawns, Field.A2);
    BIT_SET(m_wPawns, Field.B2);
    BIT_SET(m_wPawns, Field.C2);
    BIT_SET(m_wPawns, Field.D2);
    BIT_SET(m_wPawns, Field.E2);
    BIT_SET(m_wPawns, Field.F2);
    BIT_SET(m_wPawns, Field.G2);
    BIT_SET(m_wPawns, Field.H2);

    BIT_SET(m_bKing, Field.E8);
    BIT_SET(m_bQueens, Field.D8);
    BIT_SET(m_bBishops, Field.C8);
    BIT_SET(m_bBishops, Field.F8);
    BIT_SET(m_bKnights, Field.B8);
    BIT_SET(m_bKnights, Field.G8);
    BIT_SET(m_bRooks, Field.A8);
    BIT_SET(m_bRooks, Field.H8);
    BIT_SET(m_bPawns, Field.A7);
    BIT_SET(m_bPawns, Field.B7);
    BIT_SET(m_bPawns, Field.C7);
    BIT_SET(m_bPawns, Field.D7);
    BIT_SET(m_bPawns, Field.E7);
    BIT_SET(m_bPawns, Field.F7);
    BIT_SET(m_bPawns, Field.G7);
    BIT_SET(m_bPawns, Field.H7);
    */

    updateBitBoards();
}

void ChessBoard::updateBitBoards() {
    //m_wAllPieces = m_wPawns | m_wRooks | m_wKnights | m_wBishops | m_wKing | m_wQueens;
    //m_bAllPieces = m_bPawns | m_bRooks | m_bKnights | m_bBishops | m_bKing | m_bQueens;
    //m_allPieces  = m_wAllPieces | m_bAllPieces;

    m_allPieces[White] = m_pawns[White] | m_rooks[White] | m_knights[White] |
                         m_bishops[White] | m_queens[White] | m_king[White];
    m_allPieces[Black] = m_pawns[Black] | m_rooks[Black] | m_knights[Black] |
                         m_bishops[Black] | m_queens[Black] | m_king[Black];
    m_allPieces[Both]  = m_allPieces[White] | m_allPieces[Black];
}

BitBoard ChessBoard::clearFile(File file) {
    BitBoard b;

    return b;
}
