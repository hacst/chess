#include "ChessBoard.h"

ChessBoard::ChessBoard()
    : m_board({
            { WhiteRook, WhiteKnight, WhiteBishop, WhiteKing, WhiteQueen, WhiteBishop, WhiteKnight, WhiteRook,
              WhitePawn, WhitePawn, WhitePawn, WhitePawn, WhitePawn, WhitePawn, WhitePawn, WhitePawn,
              Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty,
              Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty,
              Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty,
              Empty, Empty, Empty, Empty, Empty, Empty, Empty, Empty,
              BlackPawn, BlackPawn, BlackPawn, BlackPawn, BlackPawn, BlackPawn, BlackPawn, BlackPawn,
              BlackRook, BlackKnight, BlackBishop, BlackKing, BlackQueen, BlackBishop, BlackKnight, BlackRook }
        }) {
}



void ChessBoard::applyTurn(const Turn& t) {

    if (t.action == Turn::Action::Move) {
        m_board[t.from] = Empty;
        m_board[t.to]   = t.piece;

    } else if (t.action == Turn::Action::Forfeit) {
        //TODO: Do something
    } else {
        // Assume passed
    }

    //LOG(trace) << toString();

    //updateBitBoards();
}




std::array<Piece, 64> ChessBoard::getBoard() const {
    return m_board;
}

bool ChessBoard::operator==(const ChessBoard& other) const {
    return m_board == other.getBoard();
}

bool ChessBoard::operator !=(const ChessBoard& other) const {
    return !(*this == other);
}

std::string ChessBoard::toString() const {
    stringstream ss;
    int i = 0;

    ss << endl;
    for (auto& piece: m_board) {
        if (!(i++ % 8)) ss << endl;
        ss << piece << ' ';
    }
    ss << endl;

    return ss.str();
}
