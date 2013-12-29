#include "ChessBoard.h"

using namespace std;

ChessBoard::ChessBoard() {
    std::array<Piece, 64> board = {
        { Piece(White, Rook), Piece(White, Knight), Piece(White, Bishop), Piece(White, King), Piece(White, Queen), Piece(White, Bishop), Piece(White, Knight), Piece(White, Rook),
          Piece(White, Pawn), Piece(White, Pawn), Piece(White, Pawn), Piece(White, Pawn), Piece(White, Pawn), Piece(White, Pawn), Piece(White, Pawn), Piece(White, Pawn),
          Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType),
          Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType),
          Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType),
          Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType),
          Piece(Black, Pawn), Piece(Black, Pawn), Piece(Black, Pawn), Piece(Black, Pawn), Piece(Black, Pawn), Piece(Black, Pawn), Piece(Black, Pawn), Piece(Black, Pawn),
          Piece(Black, Rook), Piece(Black, Knight), Piece(Black, Bishop), Piece(Black, King), Piece(Black, Queen), Piece(Black, Bishop), Piece(Black, Knight), Piece(Black, Rook) }
    };

    /*
white_pawns = 0x000000000000ff00
black_pawns = 0x00ff000000000000
white_knights = 0x000000000000042
black_knights = 0x4200000000000000
white_bishops =0x000000000000024
black_bishops = 0x2400000000000000
white_rooks = 0x000000000000081
black_rooks = 0x8100000000000000
white_queens = 0x0000000000000008
black_queens = 0x0800000000000000
white_king = 0x0000000000000010
black_king = 0x1000000000000000
*/

    initBitBoards(board);
}

ChessBoard::ChessBoard(std::array<Piece, 64> board) {
    initBitBoards(board);
}

void ChessBoard::initBitBoards(std::array<Piece, 64> board) {
    // init all bit boards with 0
    for (int player = White; player < NUM_PLAYERS; player++) {
        for (int pieceType = King; pieceType < NUM_PIECETYPES; pieceType++) {
            bb[player][pieceType] = 0;
        }
    }

    for (int field = 0; field < NUM_FIELDS; field++) {
        Piece piece = board[field];
        if (piece.player != NoPlayer) {
            BIT_SET(bb[piece.player][piece.type], field);
        }
    }

    updateBitBoards();
}

void ChessBoard::updateBitBoards() {
    bb[White][AllPieces] = bb[White][King]   | bb[White][Queen] | bb[White][Bishop] |
                           bb[White][Knight] | bb[White][Rook]  | bb[White][Pawn];
    bb[Black][AllPieces] = bb[Black][King]   | bb[Black][Queen] | bb[Black][Bishop] |
                           bb[Black][Knight] | bb[Black][Rook]  | bb[Black][Pawn];
}



void ChessBoard::applyTurn(const Turn& turn) {

    if (turn.action == Turn::Action::Move) {
        BIT_CLEAR(bb[turn.piece.player][turn.piece.type], turn.from);
        BIT_SET  (bb[turn.piece.player][turn.piece.type], turn.to);

        // TODO: en passant?
        PlayerColor opp = (turn.piece.player == White) ? Black : White;
        if (BIT_ISSET(bb[opp][AllPieces], turn.to)) {
            for (int pieceType = King; pieceType < NUM_PIECETYPES; pieceType++) {
                if (BIT_ISSET(bb[opp][pieceType], turn.to)) {
                    BIT_CLEAR(bb[opp][pieceType], turn.to);
                    m_capturedPieces.push_back(Piece(opp, (PieceType)pieceType));

                    pieceType = NUM_PIECETYPES; // break?
                }
            }
        }

    } else if (turn.action == Turn::Action::Forfeit) {
        //TODO: Do something
    } else {
        // Assume passed
    }

    updateBitBoards();
}

std::array<Piece, 64> ChessBoard::getBoard() const {
    std::array<Piece, 64> board;
    BitBoard allPieces = bb[White][AllPieces] | bb[Black][AllPieces];

    for (int field = 0; field < NUM_FIELDS; field++) {
        if (BIT_ISSET(allPieces, field)) {
            for (int player = White; player < NUM_PLAYERS; player++) {
                for (int pieceType = King; pieceType < NUM_PIECETYPES; pieceType++) {
                    if (BIT_ISSET(bb[player][pieceType], field)) {
                        board[field] = Piece((PlayerColor)player, (PieceType)pieceType);

                        pieceType = NUM_PIECETYPES;
                        player = NUM_PLAYERS;
                    }
                }
            }
        } else {
            board[field] = Piece(NoPlayer, NoType);
        }
    }

    return board;
}

std::vector<Piece> ChessBoard::getCapturedPieces() const {
    return m_capturedPieces;
}



/* for test and debug purposes */

bool ChessBoard::operator==(const ChessBoard& other) const {
    return getBoard() == other.getBoard();
}

bool ChessBoard::operator!=(const ChessBoard& other) const {
    return !(*this == other);
}

std::string ChessBoard::toString() const {
    std::array<Piece, 64> board = getBoard();
    stringstream ss;

    ss << endl;
    for (int row = NUM_FIELDS; row > 0; row -= 8) {
        //if (!(row % 8)) ss << endl;
        ss << endl;
        for (int col = 0; col < 8; col++) {
            ss << board[row + col - 8] << ' ';
        }
    }
    ss << endl;

    /*int i = 0;
    for (auto& piece: board) {
        if (!(i++ % 8)) ss << endl;
        ss << piece << ' ';
    }*/

    return ss.str();
}

std::string bitBoardToString(BitBoard b) {
    stringstream ss;
    int i, j;
    int bits = sizeof(b) * 8; // always 64

    ss << endl;
    //ss << "decimal: " << b;
    for (i = bits; i > 0; i -= 8) {
        //if (!(i % 8)) ss << endl;
        ss << endl;
        for (j = 0; j < 8; j++) {
            if (BIT_ISSET(b, j+(i-8))) {
                ss << 1;
            } else {
                ss << "-";
            }
        }
    }
    ss << endl;
    return ss.str();
}

BitBoard generateBitBoard(Field f1, ...) {
    va_list fields;
    va_start(fields, f1);

    BitBoard bb = 0;
    Field f = f1;
    while(f != ERR) {
        BIT_SET(bb, f);
        f = (Field)va_arg(fields, int);
    }

    va_end(fields);
    return bb;
}

std::array<Piece, 64> generateChessBoard(std::vector<PoF> pieces) {
    
    std::array<Piece, 64> board;
    for (int i = 0; i < NUM_FIELDS; i++) {
        board[i] = Piece(NoPlayer, NoType);
    }

    for (auto& pof: pieces) {
        board[pof.field] = pof.piece;
    }

    return board;
}