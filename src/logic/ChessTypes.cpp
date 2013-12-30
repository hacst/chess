#include "ChessTypes.h"

namespace std {
    std::ostream& operator <<(std::ostream& stream, const PlayerColor c) {
        switch(c) {
        case NoPlayer:
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

        if (p.player == White) {
            switch(p.type) {
            case King:   return r("K");
            case Queen:  return r("Q");
            case Bishop: return r("B");
            case Knight: return r("N");
            case Rook:   return r("R");
            case Pawn:   return r("P");
            case NoType: return r("-");
            default:     return r("?");
            }

        } else if (p.player == Black) {
            switch(p.type) {
            case King:   return r("k");
            case Queen:  return r("q");
            case Bishop: return r("b");
            case Knight: return r("n");
            case Rook:   return r("r");
            case Pawn:   return r("p");
            case NoType: return r("-");
            default:     return r("?");
            }
        } else if (p.player == NoPlayer) {
            return r("-");
        } else {
            return r("?");
        }
    }

    std::ostream& operator <<(std::ostream& stream, const Field f) {
        auto r = [&](std::string s) -> std::ostream& {
            stream << s;
            return stream;
        };

        switch(f) {
        case A1: return r("A1");
        case B1: return r("B1");
        case C1: return r("C1");
        case D1: return r("D1");
        case E1: return r("E1");
        case F1: return r("F1");
        case G1: return r("G1");
        case H1: return r("H1");

        case A2: return r("A2");
        case B2: return r("B2");
        case C2: return r("C2");
        case D2: return r("D2");
        case E2: return r("E2");
        case F2: return r("F2");
        case G2: return r("G2");
        case H2: return r("H2");

        case A3: return r("A3");
        case B3: return r("B3");
        case C3: return r("C3");
        case D3: return r("D3");
        case E3: return r("E3");
        case F3: return r("F3");
        case G3: return r("G3");
        case H3: return r("H3");

        case A4: return r("A4");
        case B4: return r("B4");
        case C4: return r("C4");
        case D4: return r("D4");
        case E4: return r("E4");
        case F4: return r("F4");
        case G4: return r("G4");
        case H4: return r("H4");

        case A5: return r("A5");
        case B5: return r("B5");
        case C5: return r("C5");
        case D5: return r("D5");
        case E5: return r("E5");
        case F5: return r("F5");
        case G5: return r("G5");
        case H5: return r("H5");

        case A6: return r("A6");
        case B6: return r("B6");
        case C6: return r("C6");
        case D6: return r("D6");
        case E6: return r("E6");
        case F6: return r("F6");
        case G6: return r("G6");
        case H6: return r("H6");

        case A7: return r("A7");
        case B7: return r("B7");
        case C7: return r("C7");
        case D7: return r("D7");
        case E7: return r("E7");
        case F7: return r("F7");
        case G7: return r("G7");
        case H7: return r("H7");

        case A8: return r("A8");
        case B8: return r("B8");
        case C8: return r("C8");
        case D8: return r("D8");
        case E8: return r("E8");
        case F8: return r("F8");
        case G8: return r("G8");
        case H8: return r("H8");

        default: return r("unknown field");
        }
    }

    std::ostream& operator <<(std::ostream& stream, const Rank r) {
        stream << static_cast<int>(r);
        return stream;
    }

    std::ostream& operator <<(std::ostream& stream, const File f) {
        stream << ('A' + static_cast<int>(f));
        return stream;
    }
}

bool Piece::operator==(const Piece& other) const {
    return type == other.type
            && player == other.player;
}
