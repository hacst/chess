#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <memory>
#include "GameState.h"
#include "Turn.h"

#define BIT_SET   (BitBoard, field) (BitBoard |=   1 << (field))
#define BIT_CLEAR (BitBoard, field) (BitBoard &= ~(1 << (field)))
#define BIT_TOGGLE(BitBoard, field) (BitBoard ^=   1 << (field))
#define BIT_ISSET (BitBoard, field) (BitBoard & (  1 << (field)))

enum Field {
    A1, A2, A3, A4, A5, A6, A7, A8,
    B1, B2, B3, B4, B5, B6, B7, B8,
    C1, C2, C3, C4, C5, C6, C7, C8,
    D1, D2, D3, D4, D5, D6, D7, D8,
    E1, E2, E3, E4, E5, E6, E7, E8,
    F1, F2, F3, F4, F5, F6, F7, F8,
    G1, G2, G3, G4, G5, G6, G7, G8,
    H1, H2, H3, H4, H5, H6, H7, H8,
    ERR
};

enum Piece {
    WhiteKing, WhiteQueen, WhiteBishop, WhiteKnight, WhiteRook, WhitePawn,
    BlackKing, BlackQueen, BlackBishop, BlackKnight, BlackRook, BlackPawn,
    Empty
};



enum File {
    A, B, C, D, E, F, G, H
};

enum Rank {
    One, Two, Three, Four, Five, Six, Seven, Eight
};





class ChessBoard {
public:
    ChessBoard() : m_board({{
        {{ WhiteRook, WhiteKnight, WhiteBishop, WhiteKing, WhiteQueen, WhiteBishop, WhiteKnight, WhiteRook }},
        {{ WhitePawn, WhitePawn, WhitePawn, WhitePawn, WhitePawn, WhitePawn, WhitePawn, WhitePawn }},
        {{ Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty }},
        {{ Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty }},
        {{ Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty }},
        {{ Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty }},
        {{ BlackPawn, BlackPawn, BlackPawn, BlackPawn, BlackPawn, BlackPawn, BlackPawn, BlackPawn }},
        {{ BlackRook, BlackKnight, BlackBishop, BlackKing, BlackQueen, BlackBishop, BlackKnight, BlackRook }}
    }}) { initBitBoards(); }

    virtual void applyTurn(const Turn &t);

private:
    // for external purpose / board representation
    std::array<std::array<Piece, 8>, 8> m_board;

    // for internal turn calculation
    // we need at least 12 bit boards for a complete board representation
    BitBoard m_pawns[2];
    BitBoard m_rooks[2];
    BitBoard m_knights[2];
    BitBoard m_bishops[2];
    BitBoard m_queens[2];
    BitBoard m_king[2];
    // some additional bit boards for turn calculation
    BitBoard m_allPieces[3];

    virtual void initBitBoards();
    virtual void updateBitBoards();

    /*
    BitBoard m_allPieces;
    //BitBoard EmptyFields;

    BitBoard m_wAllPieces;
    BitBoard m_wPawns;
    BitBoard m_wRooks;
    BitBoard m_wKnights;
    BitBoard m_wBishops;
    BitBoard m_wQueens;
    BitBoard m_wKing;

    BitBoard m_bAllPieces;
    BitBoard m_bPawns;
    BitBoard m_bRooks;
    BitBoard m_bKnights;
    BitBoard m_bBishops;
    BitBoard m_bQueens;
    BitBoard m_bKing;
    */

public:

    static BitBoard maskRank(File file);
    static BitBoard maskFile(File file);
    static BitBoard clearRank(File file);
    static BitBoard clearFile(File file);
    //static std::vector<Turn> convert(BitBoard);

};

using ChessBoardPtr = std::shared_ptr<ChessBoard>;
using BitBoard = unsigned long;

#endif // CHESSBOARD_H
