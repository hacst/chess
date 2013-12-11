#include "Evaluator.h"

#include "GameState.h"
#include <array>

Score Evaluator::getScore(const GameState& gameState) const {
	return getMaterialWorth(gameState.getNextPlayer(), gameState.getChessBoard());
}

Score Evaluator::getMaterialWorth(PlayerColor player, const ChessBoard& board) const {
	// Uses the 1/3/3/5/9 scoring for pieces. Kings are set
	// at a value higher than all other pieces combined to make
	// their loss unacceptable
	Score score = 0;

	const static std::array<Score, 13> pieceToScore = {
		200,  // White king
		9,    // White queen
		3,    // White bishop
		3,    // White knight
		5,    // White rook
		1,    // White pawn
		0,    // WTF
		0,    // Empty
	};

	// Calculate from white's perspective
	for (Piece& piece: board.getBoard()) {
		if (piece.player == player) {
			score += pieceToScore[piece.type];
		} else {
			score -= pieceToScore[piece.type];
		}
	}

	return score;
}
