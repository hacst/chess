#include "Evaluators.h"

#include "GameState.h"
#include <array>

Score MaterialEvaluator::getScore(const GameState& gameState) const {
    return getMaterialWorth(gameState.getNextPlayer(), gameState.getChessBoard());
}

Score MaterialEvaluator::getMaterialWorth(const PlayerColor player, const ChessBoard& board) const {
    // Uses fixed scoring for pieces. Kings are set
    // at a value higher than all other pieces combined to make
    // their loss unacceptable
    Score score = 0;

    const static std::array<Score, 13> pieceToScore = {
        2000,  // king
        88,    // queen
        33,    // bishop
        32,    // knight
        51,    // rook
        10,    // pawn
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
