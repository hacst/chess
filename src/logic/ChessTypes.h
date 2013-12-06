#ifndef CHESSTYPES_H
#define CHESSTYPES_H

#include <stdint.h>
#include <limits.h>
#include <vector>
#include <string>
#include <sstream>
#include <memory>
#include "../misc/helper.h"

using namespace std;


const int NUM_FIELDS     = 64;
const int NUM_PLAYERS    = 2;
const int NUM_PIECETYPES = 6;

enum Field {
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8,
    ERR
};
enum PieceType {
    King, Queen, Bishop, Knight, Rook, Pawn, AllPieces, NoType
};
enum PlayerColor {
    White, Black, NoPlayer
};

struct Piece {
    PlayerColor player;
    PieceType   type;

    Piece()
        : player(), type() {}
    Piece(PlayerColor player, PieceType pieceType)
        : player(player), type(pieceType) {}

    bool operator==(const Piece& other) const;
};

namespace std {
    std::ostream& operator <<(std::ostream& stream, const PlayerColor c);
    std::ostream& operator <<(std::ostream& stream, const Piece p);
    std::ostream& operator <<(std::ostream& stream, const Field f);
}


#endif // CHESSTYPES_H
