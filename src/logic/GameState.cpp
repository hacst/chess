#include "GameState.h"
#include <algorithm>

GameState::GameState() {
    init();
}

GameState::GameState(const ChessBoard &chessBoard)
    : m_chessBoard(chessBoard) {
    init();
}

void GameState::init() {
    m_turnGen.generateTurns(getNextPlayer(),
                            m_chessBoard);
}

std::vector<Turn> GameState::getTurnList() const {
    return m_turnGen.getTurnList();
}

void GameState::applyTurn(const Turn& turn) {
    m_chessBoard.applyTurn(turn);
    m_turnGen.generateTurns(getNextPlayer(),
                            m_chessBoard);
}

PlayerColor GameState::getNextPlayer() const {
    return m_chessBoard.getNextPlayer();
}

const ChessBoard& GameState::getChessBoard() const {
    return m_chessBoard;
}

bool GameState::isGameOver() const {
    if (isDrawDueTo50MovesRule()) {
        return true;
    }

    // When debugging we might want to work with boards without a king. Alternatively
    // check for all pieces of a player being gone.
    if (!(m_chessBoard.hasBlackPieces() && m_chessBoard.hasWhitePieces())) {
        return true;
    }

    // Assume game is over once king is captured.
    // FIXME: Figure out whether it's ok for the state to have the king captured. Sure would make the AI's life easier. Also this is horribly inefficient
    const auto captures = m_chessBoard.getCapturedPieces();
    auto hit = std::find_if(begin(captures), end(captures), [](const Piece& piece) {
        return piece.type == King;
    });

    if (hit != end(captures)) {
        return true;
    }

    return getTurnList().size() == 0;
}

bool GameState::isDrawDueTo50MovesRule() const {
    return m_chessBoard.getHalfMoveClock() >= 50 * 2;
}

PlayerColor GameState::getWinner() const {
    if (isDrawDueTo50MovesRule()) return NoPlayer;

    //FIXME: Same as isGameOver. Assumes king capture or eradication.
    if (!m_chessBoard.hasBlackPieces()) return White;
    else if (!m_chessBoard.hasWhitePieces()) return Black;

    // If we find a captured king the corresponding color has lost
    const auto captures = m_chessBoard.getCapturedPieces();
    auto hit = std::find_if(begin(captures), end(captures), [](const Piece& piece) {
        return piece.type == King;
    });

    return togglePlayerColor(hit->player);
}

Score GameState::getScore() const {
    return m_chessBoard.getScore(m_chessBoard.getNextPlayer());
}

bool GameState::operator==(const GameState& other) const {
    return m_chessBoard == other.getChessBoard();
}

bool GameState::operator!=(const GameState& other) const {
    return !(*this == other);
}

Hash GameState::getHash() const {
    return m_chessBoard.getHash();
}

std::string GameState::toString() const {
    return m_chessBoard.toString();
}
