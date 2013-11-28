#include "GameState.h"

GameState::GameState() {
    init();
}

void GameState::init() {
    m_nextPlayer = PlayerColor::White;
}



std::vector<Turn> GameState::getTurnList() {
    return m_turnGen.generateTurns(m_nextPlayer, m_chessBoard);
}

void GameState::applyTurn(const Turn& t) {
    // TODO: hier auch die bitboards im TurnGen updaten?

    m_chessBoard.applyTurn(t);
    m_nextPlayer = (m_nextPlayer == White) ? Black : White;
}

PlayerColor GameState::getNextPlayer() const {
    return m_nextPlayer;
}

const ChessBoard& GameState::getChessBoard() const {
    return m_chessBoard;
}

bool GameState::operator==(const GameState& other) const {
    return m_nextPlayer == other.getNextPlayer()
            && m_chessBoard == other.getChessBoard();
}

bool GameState::operator!=(const GameState& other) const {
    return !(*this == other);
}

std::string GameState::toString() const {
    return m_chessBoard.toString();
}
