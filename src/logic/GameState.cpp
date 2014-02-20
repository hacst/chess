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
    m_turnGen.initFlags(m_chessBoard);
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
    return m_chessBoard.isGameOver();
}

bool GameState::isDrawDueTo50MovesRule() const {
    return m_chessBoard.isDrawDueTo50MovesRule();
}

PlayerColor GameState::getWinner() const {
    return m_chessBoard.getWinner();
}

Score GameState::getScore(size_t depth) const {
    return m_chessBoard.getScore(m_chessBoard.getNextPlayer(), depth);
}

Piece GameState::getLastCapturedPiece() const {
    return m_chessBoard.getLastCapturedPiece();
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
