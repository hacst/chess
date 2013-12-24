#include "GameState.h"

GameState::GameState() {
	init();
}

GameState::GameState(const ChessBoard &chessBoard, PlayerColor nextPlayer)
	: m_nextPlayer(nextPlayer)
	, m_chessBoard(chessBoard) {
	// Empty
}

void GameState::init() {
	m_nextPlayer = PlayerColor::White;
	// TODO: init ChessBoard from savegame
}

std::vector<Turn> GameState::getTurnList() {
	return m_turnGen.generateTurns(getNextPlayer(), getChessBoard());
}

void GameState::applyTurn(const Turn& turn) {
    m_chessBoard.applyTurn(turn);
    m_nextPlayer = togglePlayerColor(m_nextPlayer);
}

PlayerColor GameState::getNextPlayer() const {
	return m_nextPlayer;
}

const ChessBoard& GameState::getChessBoard() const {
	return m_chessBoard;
}

bool GameState::isGameOver() {
	//TODO: Impl. me
	return getTurnList().empty();
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
