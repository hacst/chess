#include "ChessBoard.h"
#include "misc/helper.h"
#include <iostream>
using namespace std;

ChessBoard::ChessBoard()
    : ChessBoard({
{ Piece(White, Rook), Piece(White, Knight), Piece(White, Bishop), Piece(White, Queen), Piece(White, King), Piece(White, Bishop), Piece(White, Knight), Piece(White, Rook),
  Piece(White, Pawn), Piece(White, Pawn), Piece(White, Pawn), Piece(White, Pawn), Piece(White, Pawn), Piece(White, Pawn), Piece(White, Pawn), Piece(White, Pawn),
  Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType),
  Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType),
  Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType),
  Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType), Piece(NoPlayer, NoType),
  Piece(Black, Pawn), Piece(Black, Pawn), Piece(Black, Pawn), Piece(Black, Pawn), Piece(Black, Pawn), Piece(Black, Pawn), Piece(Black, Pawn), Piece(Black, Pawn),
  Piece(Black, Rook), Piece(Black, Knight), Piece(Black, Bishop), Piece(Black, Queen), Piece(Black, King), Piece(Black, Bishop), Piece(Black, Knight), Piece(Black, Rook) }
                 }, White, {{true, true}}, {{true,true}}, ERR, 0, 1)
{
    m_kingInCheck[White] = false;
    m_kingInCheck[Black] = false;
    m_stalemate = false;
    m_checkmate[White] = false;
    m_checkmate[Black] = false;
}

ChessBoard::ChessBoard(std::array<Piece, 64> board,
                       PlayerColor nextPlayer,
                       std::array<bool, NUM_PLAYERS> shortCastleRight,
                       std::array<bool, NUM_PLAYERS> longCastleRight,
                       Field enPassantSquare,
                       int halfMoveClock,
                       int fullMoveClock)
    : m_shortCastleRight(shortCastleRight)
    , m_longCastleRight(longCastleRight)
    , m_enPassantSquare(enPassantSquare)
    , m_halfMoveClock(halfMoveClock)
    , m_fullMoveClock(fullMoveClock)
    , m_nextPlayer(nextPlayer)
    , m_evaluator(board)
    , m_hasher() {

    m_kingInCheck[White] = false;
    m_kingInCheck[Black] = false;
    m_stalemate = false;
    m_checkmate[White] = false;
    m_checkmate[Black] = false;
    initBitBoards(board);
}

void ChessBoard::initBitBoards(std::array<Piece, 64> board) {

    // init all bit boards with 0
    for (int player = White; player < NUM_PLAYERS; player++) {
        for (int pieceType = King; pieceType < NUM_PIECETYPES; pieceType++) {
            m_bb[player][pieceType] = 0;
        }
    }

    for (int field = 0; field < NUM_FIELDS; field++) {
        Piece piece = board[field];
        if (piece.player != NoPlayer) {
            BIT_SET(m_bb[piece.player][piece.type], field);
        }
    }

    updateBitBoards();
    
    m_hasher = IncrementalZobristHasher(*this);
}

void ChessBoard::updateBitBoards() {
    m_bb[White][AllPieces] = m_bb[White][King]   | m_bb[White][Queen] |
                             m_bb[White][Bishop] | m_bb[White][Knight] |
                             m_bb[White][Rook]   | m_bb[White][Pawn];
    m_bb[Black][AllPieces] = m_bb[Black][King]   | m_bb[Black][Queen] |
                             m_bb[Black][Bishop] | m_bb[Black][Knight] |
                             m_bb[Black][Rook]   | m_bb[Black][Pawn];
}

void ChessBoard::applyTurn(const Turn& turn) {
    ++m_halfMoveClock;

    if (turn.action == Turn::Action::Move) {
        applyMoveTurn(turn);
    } else if (turn.action == Turn::Action::Castle) {
        applyCastleTurn(turn);

    } else if (turn.action == Turn::Action::PromotionQueen) {
        applyPromotionTurn(turn, Queen);
    } else if (turn.action == Turn::Action::PromotionBishop) {
        applyPromotionTurn(turn, Bishop);
    } else if (turn.action == Turn::Action::PromotionKnight) {
        applyPromotionTurn(turn, Knight);
    } else if (turn.action == Turn::Action::PromotionRook) {
        applyPromotionTurn(turn, Rook);
    } else if (turn.action == Turn::Action::Forfeit) {
        //TODO: add a Action::forfeit turn to each generated turn list?
    } else {
        // Assume passed
    }

    updateEnPassantSquare(turn);
    updateBitBoards();

    // select next player
    if (m_nextPlayer == White) {
        m_nextPlayer = Black;
    } else {
        ++m_fullMoveClock;
        m_nextPlayer = White;
    }
    m_hasher.turnAppliedIncrement();
}

void ChessBoard::applyMoveTurn(const Turn& turn) {
    const PlayerColor opp = togglePlayerColor(turn.piece.player);
    updateCastlingRights(turn);

    BIT_CLEAR(m_bb[turn.piece.player][turn.piece.type], turn.from);
    BIT_SET  (m_bb[turn.piece.player][turn.piece.type], turn.to);

    m_evaluator.moveIncrement(turn);
    m_hasher.moveIncrement(turn);

    if (BIT_ISSET(m_bb[opp][AllPieces], turn.to)) {
        capturePiece(turn);

    } else if (m_enPassantSquare == turn.to) {
        const Rank rank = rankFor(m_enPassantSquare);
        const Piece capturedPiece(opp, Pawn);
        Field field;
        if (opp == White) {
            field = fieldFor(fileFor(m_enPassantSquare), nextRank(rank));
        } else {
            field = fieldFor(fileFor(m_enPassantSquare), prevRank(rank));
        }

        addCapturedPiece(capturedPiece, field);
    }
}

void ChessBoard::applyCastleTurn(const Turn& turn) {
    m_halfMoveClock = 0;
    updateCastlingRights(turn);

    Field from, to;

    assert(turn.piece.type == King);

    BIT_CLEAR(m_bb[turn.piece.player][King], turn.from);
    BIT_SET  (m_bb[turn.piece.player][King], turn.to);
    
    m_hasher.moveIncrement(turn);
    m_evaluator.moveIncrement(turn);

    if (turn.to == G1) { // short castle, white
        from = H1;
        to = F1;
    } else if (turn.to == G8) { // short castle, black
        from = H8;
        to = F8;
    } else if (turn.to == C1) { // long castle, white
        from = A1;
        to = D1;
    } else if (turn.to == C8) { // long castle, black
        from = A8;
        to = D8;
    } else {
        assert(false);
    }

    BIT_CLEAR(m_bb[turn.piece.player][Rook], from);
    BIT_SET  (m_bb[turn.piece.player][Rook], to);

    Turn rookTurn = Turn::move(Piece(turn.piece.player, Rook), from, to);
    m_hasher.moveIncrement(rookTurn);
    m_evaluator.moveIncrement(rookTurn);

    assert(!BIT_ISSET(m_bb[togglePlayerColor(turn.piece.player)][AllPieces], turn.to));
    assert(!BIT_ISSET(m_bb[togglePlayerColor(turn.piece.player)][AllPieces], to));
}

void ChessBoard::applyPromotionTurn(const Turn& turn, const
                                    PieceType pieceType) {
    m_halfMoveClock = 0;

    BIT_CLEAR(m_bb[turn.piece.player][turn.piece.type], turn.from);
    BIT_SET  (m_bb[turn.piece.player][pieceType],       turn.to);

    m_evaluator.promotionIncrement(turn, pieceType);
    m_hasher.promotionIncrement(turn, pieceType);

    // TODO: update m_evaluator, m_hasher?
}

void ChessBoard::capturePiece(const Turn& turn) {
    const PlayerColor opp = togglePlayerColor(turn.piece.player);

    for (int pieceType = King; pieceType < NUM_PIECETYPES; pieceType++) {
        if (BIT_ISSET(m_bb[opp][pieceType], turn.to)) {
            const Piece capturedPiece(opp, (PieceType) pieceType);
            addCapturedPiece(capturedPiece, turn.to);
            break;
        }
    }
}

void ChessBoard::addCapturedPiece(const Piece capturedPiece, Field field) {
    m_halfMoveClock = 0;

    BIT_CLEAR(m_bb[capturedPiece.player][capturedPiece.type], field);
    m_capturedPieces.push_back(capturedPiece);

    m_evaluator.captureIncrement(field, capturedPiece);
    m_hasher.captureIncrement(field, capturedPiece);
}

void ChessBoard::updateEnPassantSquare(const Turn &turn) {
    if (m_enPassantSquare != ERR) {
        m_hasher.clearedEnPassantSquare(m_enPassantSquare);
        m_enPassantSquare = ERR; // Void en passant rights
    }

    const PlayerColor opp = togglePlayerColor(turn.piece.player);

    if (turn.piece.type == Pawn) {
        m_halfMoveClock = 0;
        // check possible en passant turn for next player
        const Rank fromRank = rankFor(turn.from);
        const Rank toRank =  rankFor(turn.to);

        if (fromRank == Two && toRank == Four) {
            m_enPassantSquare = fieldFor(fileFor(turn.from), nextRank(fromRank));
            m_hasher.newEnPassantPossibility(turn, m_bb[opp][Pawn]);
        } else if (fromRank == Seven && toRank == Five) {
            m_enPassantSquare = fieldFor(fileFor(turn.from), nextRank(toRank));
            m_hasher.newEnPassantPossibility(turn, m_bb[opp][Pawn]);
        }
    }
}

void ChessBoard::updateCastlingRights(const Turn& turn) {
    const auto prevLongCastleRight  = m_longCastleRight;
    const auto prevShortCastleRight = m_shortCastleRight;

    if (turn.piece == Piece(White, Rook)) {
        if      (turn.from == A1) m_longCastleRight[White]  = false;
        else if (turn.from == H1) m_shortCastleRight[White] = false;
    } else if (turn.piece == Piece(White, King)) {
        m_shortCastleRight[White] = false;
        m_longCastleRight[White]  = false;
    }

    if (turn.piece == Piece(Black, Rook)) {
        if      (turn.from == A8) m_longCastleRight[Black]  = false;
        else if (turn.from == H8) m_shortCastleRight[Black] = false;
    } else if (turn.piece == Piece(Black, King)) {
        m_shortCastleRight[Black] = false;
        m_longCastleRight[Black]  = false;
    }

    m_hasher.updateCastlingRights(
        prevShortCastleRight, prevLongCastleRight,
        m_shortCastleRight, m_longCastleRight
    );
}

std::array<Piece, 64> ChessBoard::getBoard() const {
    std::array<Piece, 64> board;
    BitBoard allPieces = m_bb[White][AllPieces] | m_bb[Black][AllPieces];

    for (int field = 0; field < NUM_FIELDS; field++) {
        if (BIT_ISSET(allPieces, field)) {
            for (int player = White; player < NUM_PLAYERS; player++) {
                for (int pieceType = King; pieceType < NUM_PIECETYPES; pieceType++) {
                    if (BIT_ISSET(m_bb[player][pieceType], field)) {
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

bool ChessBoard::hasBlackPieces() const {
    return m_bb[Black][AllPieces] != 0;
}

bool ChessBoard::hasWhitePieces() const {
    return m_bb[White][AllPieces] != 0;
}

PlayerColor ChessBoard::getNextPlayer() const {
    return m_nextPlayer;
}

//! Returns the current estimated score according to the internal estimator.
Score ChessBoard::getScore(PlayerColor color) const {
    return m_evaluator.getScore(color);
}

Hash ChessBoard::getHash() const {
    return m_hasher.getHash();
}

int ChessBoard::getHalfMoveClock() const {
    return m_halfMoveClock;
}

int ChessBoard::getFullMoveClock() const {
    return m_fullMoveClock;
}


/* for test and debug purposes */

bool ChessBoard::operator==(const ChessBoard& other) const {
    return m_bb == other.m_bb
        && m_shortCastleRight == other.m_shortCastleRight
        && m_longCastleRight == other.m_longCastleRight
        && m_enPassantSquare == other.m_enPassantSquare
        && m_nextPlayer == other.m_nextPlayer
    //    && m_capturedPieces == other.m_capturedPieces   // Exluded from comparision
        && m_evaluator == other.m_evaluator
        && m_hasher == other.m_hasher;
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
    ss << endl << endl;
    ss << toFEN() << endl;
    ss << std::hex << "Hash: " << m_hasher.getHash() << std::dec << endl;
    ss << "Score estimate       : " << m_evaluator.getScore(m_nextPlayer) << endl;

    return ss.str();
}

ChessBoard ChessBoard::fromFEN(const string &fen) {
    stringstream ss(fen);
    
    // Read back board
    std::array<Piece, 64> board{};
    Rank rank = Eight;
    File file = A;
    
    char c;
    while(ss.peek() != EOF && ss.peek() != ' ' && rank >= One) {
        bool nofeed = false;
        ss >> std::skipws >> c;

        const Field cur = fieldFor(file, rank);
        
        switch (c) {
        case 'k': board[cur] = Piece(Black, King); break;
        case 'q': board[cur] = Piece(Black, Queen); break;
        case 'b': board[cur] = Piece(Black, Bishop); break;
        case 'n': board[cur] = Piece(Black, Knight); break;
        case 'r': board[cur] = Piece(Black, Rook); break;
        case 'p': board[cur] = Piece(Black, Pawn); break;
            
        case 'K': board[cur] = Piece(White, King); break;
        case 'Q': board[cur] = Piece(White, Queen); break;
        case 'B': board[cur] = Piece(White, Bishop); break;
        case 'N': board[cur] = Piece(White, Knight); break;
        case 'R': board[cur] = Piece(White, Rook); break;
        case 'P': board[cur] = Piece(White, Pawn); break;
            
        case '/':
            nofeed = true;
            break;
        default:
            assert(c >= '1' && c <= '8');
            const int filesToSkip = c - '0';
            
            file = static_cast<File>(file + filesToSkip);
            nofeed = true;
            
            break;
        }
        
        if (!nofeed) {
            file = nextFile(file);
        }
        if (file > H) {
            file = A;
            rank = prevRank(rank);
        }
    }
    
    ss >> std::skipws >> c;

    // Read back active player
    assert(c == 'w' || c == 'b');
    PlayerColor nextPlayer = (c == 'w') ? White : Black;
    
    // Read back castling rights
    array<bool, NUM_PLAYERS> shortCastleRight {false, false};
    array<bool, NUM_PLAYERS> longCastleRight {false, false};
    while(ss.peek() == ' ') { ss.get(); };
    while(ss.peek() != EOF && ss.peek() != ' ') {
        ss >> std::skipws >> c;
        switch (c) {
        case 'K': shortCastleRight[White] = true;  break;
        case 'k': shortCastleRight[Black] = true; break;
        case 'Q': longCastleRight[White] = true; break;
        case 'q': longCastleRight[Black] = true; break;
        case '-': break;
        default: assert(false);
        }
    }
    
    // Read back en passant square
    Field enPassantSquare = ERR;
    
    ss >> std::skipws >> c;
    if (c != '-') {
        File f = static_cast<File>(c - 'a');
        assert(f >= A && f <= H);
        ss >> c;
        Rank r = static_cast<Rank>(c - '1');
        assert(r >= One && r <= Eight);
        
        enPassantSquare = fieldFor(f,r);
    }

    // Read back half-move counter
    int halfMoveClock;
    ss >> std::skipws >> halfMoveClock;
    
    // Read back full-move counter
    int fullMoveClock;
    ss >> std::skipws >> fullMoveClock;
    
    return ChessBoard(board,
                      nextPlayer,
                      shortCastleRight, longCastleRight,
                      enPassantSquare,
                      halfMoveClock, fullMoveClock);
}

string ChessBoard::toFEN() const {
    const auto board = getBoard();
    
    // Output board
    stringstream ss;
    for (Rank rank = Eight; rank >= One; rank = prevRank(rank)) {
        int empty = 0;
        for (File file = A; file <= H; file = nextFile(file)) {
            const Piece p = board[fieldFor(file, rank)];
            if (p.type == NoType) ++empty;
            else {
                if (empty > 0) {
                    ss << empty;
                    empty = 0;
                }
                ss << p;
            }
        }
        
        if (empty > 0) ss << empty;
        
        if (rank != One) ss << "/";
    }
    
    // Active color
    ss << " " << ((m_nextPlayer == White) ? "w":"b") << " ";
    
    // Castling availability
    if (!m_shortCastleRight[White] && !m_shortCastleRight[Black]
            && !m_longCastleRight[White] && !m_longCastleRight[Black]) {
        
        ss << "-";
    } else {
        if (m_shortCastleRight[White]) ss << "K";
        if (m_longCastleRight[White]) ss << "Q";
        if (m_shortCastleRight[Black]) ss << "k";
        if (m_longCastleRight[Black]) ss << "q";
    }
    
    // En passant
    ss << " " << m_enPassantSquare << " ";
    
    // Half-move clock
    ss << m_halfMoveClock << " ";
    
    // Full-move clock
    ss << m_fullMoveClock;
    
    return ss.str();
}

Field ChessBoard::getEnPassantSquare() const {
    return m_enPassantSquare;
}

std::array<bool, NUM_PLAYERS> ChessBoard::getKingInCheck() const {
    return m_kingInCheck;
}

void ChessBoard::setCheckmate(PlayerColor player, bool checkmate) {
    m_checkmate[player] = checkmate;
}

std::array<bool, NUM_PLAYERS> ChessBoard::getCheckmate() const {
    return m_checkmate;
}

void ChessBoard::setKingInCheck(PlayerColor player, bool kingInCheck) {
    m_kingInCheck[player] = kingInCheck;
}

bool ChessBoard::getStalemate() const {
    return m_stalemate;
}

void ChessBoard::setStalemate(bool stalemate) {
    m_stalemate = stalemate;
}

std::array<bool, NUM_PLAYERS> ChessBoard::getShortCastleRights() const {
    return m_shortCastleRight;
}

std::array<bool, NUM_PLAYERS> ChessBoard::getLongCastleRights() const {
    return m_longCastleRight;
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

/**
 * @brief Helper function for board generation.
 * @note: Guesses on castling rights and clocks. Prefer ChessBoard::fromFEN
 * @param pieces List of Piece+Field combinations.
 * @param nextPlayer Next player to draw.
 * @return Chess board.
 */
ChessBoard generateChessBoard(std::vector<PoF> pieces, PlayerColor nextPlayer) {
    std::array<Piece, 64> board;
    for (int i = 0; i < NUM_FIELDS; i++) {
        board[i] = Piece(NoPlayer, NoType);
    }

    for (auto& pof: pieces) {
        board[pof.field] = pof.piece;
    }
    
    // Guess castling rights
    std::array<bool, NUM_PLAYERS> shortCastleRight = {false, false};
    std::array<bool, NUM_PLAYERS> longCastleRight = {false, false};
    
    if (board[E1] == Piece(White, King)) {
        longCastleRight[White] = (board[A1] == Piece(White, Rook));
        shortCastleRight[White] = (board[H1] == Piece(White, Rook));
    }
    
    if (board[E8] == Piece(Black, King)) {
        longCastleRight[Black] = (board[A8] == Piece(Black, Rook));
        shortCastleRight[Black] = (board[H8] == Piece(Black, Rook));
    }
    
    Field enPassantSquare = ERR; // None
    
    return ChessBoard(board, nextPlayer,
                      shortCastleRight, longCastleRight,
                      enPassantSquare,
                      0, 1);
}
