#include "GameState.h"

void GameState::init() {
    m_board = new ChessBoard();
}

std::vector<Turn> GameState::getTurnList() {
    // TODO: muss ich hier "this" in "GameStatePtr" casten?
    return m_turnGenerator->generateTurns(this);
}

void GameState::applyTurn(const Turn &t) {
    m_board->applyTurn(t);
    m_next = (m_next == White) ? Black : White;
}
