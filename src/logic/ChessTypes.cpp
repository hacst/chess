#include "ChessTypes.h"

namespace std {
    std::ostream& operator <<(std::ostream& stream, const PlayerColor c) {
        switch(c) {
        case None:
            stream << "None";
            break;
        case White:
            stream << "White";
            break;
        case Black:
            stream << "Black";
            break;
        default:
            assert(false);
            stream << "Unknown color";
            break;
        }

        return stream;
    }

    std::ostream& operator <<(std::ostream& stream, const Piece p) {
        auto r = [&](std::string s) -> std::ostream& {
            stream << s;
            return stream;
        };

        //TODO: Change this to proper chess notation
        switch(p) {
        case Piece::Empty: return r(" E ");
        case Piece::WhiteKing: return r("wK ");
        case Piece::WhiteQueen: return r("wQ ");
        case Piece::WhiteRook: return r("wR ");
        case Piece::WhiteBishop: return r("wB ");
        case Piece::WhiteKnight: return r("wKN");
        case Piece::WhitePawn: return r("wP ");
        case Piece::BlackKing: return r("bK ");
        case Piece::BlackQueen: return r("bQ ");
        case Piece::BlackRook: return r("bR ");
        case Piece::BlackBishop: return r("bB ");
        case Piece::BlackKnight: return r("bKN");
        case Piece::BlackPawn: return r("bP ");
        default: return r("???");
        }
    }

    std::ostream& operator <<(std::ostream& stream, const Field f) {
        auto r = [&](std::string s) -> std::ostream& {
            stream << s;
            return stream;
        };

        switch(f) {
        case Field::A1: return r("A1");
        case Field::B1: return r("B1");
        default: return r("unknown field");
        }
    }
}
