#include "GameState.h"

GameState::GameState() {
    init();
}

void GameState::init() {
    m_actPlayer = PlayerColor::White;
    // TODO: init ChessBoard from savegame
}



std::vector<Turn> GameState::getTurnList() {
    return m_turnGen.generateTurns(getActPlayer(), getChessBoard());
}

void GameState::applyTurn(const Turn& turn) {
    m_chessBoard.applyTurn(turn);
    m_actPlayer = (m_actPlayer == White) ? Black : White;
}

PlayerColor GameState::getActPlayer() const {
    return m_actPlayer;
}

const ChessBoard& GameState::getChessBoard() const {
    return m_chessBoard;
}

bool GameState::operator==(const GameState& other) const {
    return m_actPlayer == other.getActPlayer()
            && m_chessBoard == other.getChessBoard();
}

bool GameState::operator!=(const GameState& other) const {
    return !(*this == other);
}

std::string GameState::toString() const {
    return m_chessBoard.toString();
}
