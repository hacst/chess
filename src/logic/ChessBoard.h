#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include "ChessTypes.h"
#include "Turn.h"


class ChessBoard {
public:
    ChessBoard();

    virtual void applyTurn(const Turn& t);
    //virtual std::vector<Piece> getCapturedPieces(PlayerColor player);
    virtual std::array<Piece, 64> getBoard() const;

    bool operator==(const ChessBoard& other) const;
    bool operator!=(const ChessBoard& other) const;
    std::string toString() const;

private:
    // for external board representation
    std::array<Piece, 64> m_board;
};

using ChessBoardPtr = std::shared_ptr<ChessBoard>;

#endif // CHESSBOARD_H
