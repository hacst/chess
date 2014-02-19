/*
    Copyright (c) 2013-2014, Stefan Hacker <dd0t@users.sourceforge.net>

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
#include "DebugTools.h"

#include <sstream>

using namespace std;
namespace DebugTools {

string toInitializerList(const std::array<Piece, 64>& board) {
    stringstream ss;
    ss << endl << "{" << endl;
    int field = -1;
    for (const Piece& piece : board) {
        ++field;

        if (piece.player == NoPlayer || piece.type == NoType) {
            continue;
        }

        ss << "    PoF(Piece(" << piece.player << ", ";

        switch (piece.type) {
        case King: ss << "King"; break;
        case Queen: ss << "Queen"; break;
        case Bishop: ss << "Bishop"; break;
        case Knight: ss << "Knight"; break;
        case Rook: ss << "Rook"; break;
        case Pawn: ss << "Pawn"; break;
        default: ss << "???"; break;
        }

        ss << "Pawn), " << static_cast<Field>(field) << ")," << endl;
    }
    ss << endl << "}" << endl;

    return ss.str();
}

} // namespace DebugTools
