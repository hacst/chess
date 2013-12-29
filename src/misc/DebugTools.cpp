#include "DebugTools.h"

#include <sstream>

#include "logic/ChessBoard.h"

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
