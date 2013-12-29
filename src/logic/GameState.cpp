#include "GameState.h"
#include <algorithm>

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

bool GameState::isGameOver() const {
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

    return (hit != end(captures));
}

PlayerColor GameState::getWinner() const {
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
