#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <stdarg.h>
#include <array>
#include <cmath>

//#include "ChessTypes.h"
#include "Turn.h"
#include "Evaluators.h"

using BitBoard = uint64_t;

#ifdef _MSC_VER
#include <intrin.h>

#ifdef _WIN64
#pragma intrinsic(_BitScanReverse64)
inline Field getFirstOccupiedField(BitBoard bb) {
    assert(bb != 0);
    assert(sizeof(Field) == sizeof(unsigned long));
    unsigned long result;
    _BitScanReverse64(&result, bb);
    return static_cast<Field>(result);
}

#else //_WIN32
#pragma intrinsic(_BitScanReverse)
inline Field getFirstOccupiedField(BitBoard bb) {
    assert(bb != 0);
    assert(sizeof(Field) == sizeof(long));
    unsigned long result;
    if (_BitScanReverse(&result, bb >> 32) == 0) {
        _BitScanReverse(&result, bb & 0xFFFFFFFF);
        return static_cast<Field>(result);
    }
    return static_cast<Field>(result + 32);
}

#endif //_WIN64
#elif defined(__GNUC__) || defined(__clang__)
inline Field getFirstOccupiedField(BitBoard bb) {
    assert(bb != 0);
    assert(sizeof(Field) == sizeof(int));
    return static_cast<Field>(63 - __builtin_clzll(bb));
}
#else
// This shouldn't be used really. Too slow.
inline Field getFirstOccupiedField(BitBoard bb) {
    return static_cast<Field>(static_cast<int>(std::log2((double)bb)));
}
#endif

#define BB_SCAN(bb)    getFirstOccupiedField(bb) /* returns the field of MS1B */
#define BB_SET( field) static_cast<BitBoard>(std::pow(2, (int)field))    /* returns the value 2^field */

#define BIT_SET(   bb, field) (bb |=   (BitBoard)1 << (field))
#define BIT_CLEAR( bb, field) (bb &= ~((BitBoard)1 << (field)))
#define BIT_TOGGLE(bb, field) (bb ^=   (BitBoard)1 << (field))
#define BIT_ISSET( bb, field) (bb & (  (BitBoard)1 << (field)))

std::string bitBoardToString(BitBoard b);
BitBoard    generateBitBoard(Field f1, ...);



struct PoF {
    Piece piece;
    Field field;

    PoF(Piece piece, Field field)
        : piece(piece), field(field) {}
};

std::array<Piece, 64> generateChessBoard(std::vector<PoF> pieces);

class ChessBoard {
    friend class TurnGenerator;

public:
    ChessBoard();
    ChessBoard(std::array<Piece, 64> board);

    void                  applyTurn(const Turn& t);
    std::array<Piece, 64> getBoard()          const;
    std::vector<Piece>    getCapturedPieces() const;

    //! Returns true if black pieces are on the board.
    bool hasBlackPieces() const;
    //! Returns true if white pieces are on the board.
    bool hasWhitePieces() const;

    //! Returns the current estimated score according to the internal estimator.
    Score getScore(PlayerColor color) const;

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
    void initBitBoards(std::array<Piece, 64> board);
    void updateBitBoards();

    std::vector<Piece> m_capturedPieces;
    IncrementalBoardEvaluator m_evaluator;
};

using ChessBoardPtr = std::shared_ptr<ChessBoard>;

#endif // CHESSBOARD_H
