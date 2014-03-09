/*
    Copyright (c) 2013-2014, Max Stark <max.stark88@googlemail.com>

    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

    3. Neither the name of the copyright holder nor the names of its
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.
*/
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
        case A1: return r("a1");
        case B1: return r("b1");
        case C1: return r("c1");
        case D1: return r("d1");
        case E1: return r("e1");
        case F1: return r("f1");
        case G1: return r("g1");
        case H1: return r("h1");

        case A2: return r("a2");
        case B2: return r("b2");
        case C2: return r("c2");
        case D2: return r("d2");
        case E2: return r("e2");
        case F2: return r("f2");
        case G2: return r("g2");
        case H2: return r("h2");

        case A3: return r("a3");
        case B3: return r("b3");
        case C3: return r("c3");
        case D3: return r("d3");
        case E3: return r("e3");
        case F3: return r("f3");
        case G3: return r("g3");
        case H3: return r("h3");

        case A4: return r("a4");
        case B4: return r("b4");
        case C4: return r("c4");
        case D4: return r("d4");
        case E4: return r("e4");
        case F4: return r("f4");
        case G4: return r("g4");
        case H4: return r("h4");

        case A5: return r("a5");
        case B5: return r("b5");
        case C5: return r("c5");
        case D5: return r("d5");
        case E5: return r("e5");
        case F5: return r("f5");
        case G5: return r("g5");
        case H5: return r("h5");

        case A6: return r("a6");
        case B6: return r("b6");
        case C6: return r("c6");
        case D6: return r("d6");
        case E6: return r("e6");
        case F6: return r("f6");
        case G6: return r("g6");
        case H6: return r("h6");

        case A7: return r("a7");
        case B7: return r("b7");
        case C7: return r("c7");
        case D7: return r("d7");
        case E7: return r("e7");
        case F7: return r("f7");
        case G7: return r("g7");
        case H7: return r("h7");

        case A8: return r("a8");
        case B8: return r("b8");
        case C8: return r("c8");
        case D8: return r("d8");
        case E8: return r("e8");
        case F8: return r("f8");
        case G8: return r("g8");
        case H8: return r("h8");

        default: return r("-");
        }
    }

    std::ostream& operator <<(std::ostream& stream, const Rank r) {
        if (r == NoRank) stream << "NoRank";
        else stream << static_cast<int>(r);

        return stream;
    }

    std::ostream& operator <<(std::ostream& stream, const File f) {
        if (f == NoFile) stream << "NoFile";
        else stream << ('a' + static_cast<int>(f));

        return stream;
    }
}

bool Piece::operator==(const Piece& other) const {
    return type == other.type
            && player == other.player;
}
