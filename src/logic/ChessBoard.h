#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <stdarg.h>
#include <array>

//#include "ChessTypes.h"
#include "Turn.h"

#define BB_SCAN(bb)    static_cast<Field>(static_cast<int>(log2((double)bb))) /* returns the field of MS1B */
#define BB_SET( field) (pow(2, (int)field))    /* returns the value 2^field */

#define BIT_SET(   bb, field) (bb |=   (BitBoard)1 << (field))
#define BIT_CLEAR( bb, field) (bb &= ~((BitBoard)1 << (field)))
#define BIT_TOGGLE(bb, field) (bb ^=   (BitBoard)1 << (field))
#define BIT_ISSET( bb, field) (bb & (  (BitBoard)1 << (field)))

using BitBoard = uint64_t;
std::string bitBoardToString(BitBoard b);
BitBoard    generateBitBoard(Field f1, ...);



struct PoF {
    Piece piece;
    Field field;

    PoF(Piece piece, Field field)
        : piece(piece), field(field) {}
};

std::array<Piece, 64> generateChessBoard(PoF pof1, ...);







class ChessBoard {
    friend class TurnGenerator;

public:
    ChessBoard();
    ChessBoard(std::array<Piece, 64> board);

    virtual void                  applyTurn(const Turn& t);
    virtual std::array<Piece, 64> getBoard()          const;
    virtual std::vector<Piece>    getCapturedPieces() const;

    bool operator==(const ChessBoard& other) const;
    bool operator!=(const ChessBoard& other) const;
    std::string toString() const;

protected:
    // for internal turn calculation bit boards are used
    // at least 12 bit boards are needed for a complete board
    // representation + some additional bit boards for turn
    // faster calculation
    std::array<std::array<BitBoard,NUM_PIECETYPES+1>, NUM_PLAYERS> bb;

private:
    virtual void initBitBoards(std::array<Piece, 64> board);
    virtual void updateBitBoards();

    std::vector<Piece> m_capturedPieces;
};

using ChessBoardPtr = std::shared_ptr<ChessBoard>;

#endif // CHESSBOARD_H
