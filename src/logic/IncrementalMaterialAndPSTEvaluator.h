#ifndef INCREMENTALMATERIALANDPSTEVALUATOR_H
#define INCREMENTALMATERIALANDPSTEVALUATOR_H

#include <array>
#include "logic/ChessTypes.h"

class ChessBoard;
class Turn;

/**
 * @brief Class for incrementally estimating game state using PST and Material.
 * Uses fixed piece square tables and a fixed material evaluation to
 * incrementally calculate a score for the current board position during
 * the game.
 * @see ChessBoard
 * @note Not valid once game is over.
 */
class IncrementalMaterialAndPSTEvaluator {
public:
    //! Initializes the evaluator for a prestine board.
    IncrementalMaterialAndPSTEvaluator();
    //! Initializes the evaluator for an already played board.
    explicit IncrementalMaterialAndPSTEvaluator(const std::array<Piece, 64> &board);

    //! Updates estimate for the moving of the piece in give turn.
    void moveIncrement(const Turn& turn);
    //! Updates estimate for a capture of the given piece on the given field.
    void captureIncrement(Field field, const Piece& piece);
    //! Updates estimate for the promotion of a piece
    void promotionIncrement(const Turn& turn, PieceType targetType);

    //! Gives a full estimate for the given board
    static Score estimateFullBoard(const std::array<Piece, 64> &board);

    //! Returns the score from the perspective of the given player color.
    Score getScore(PlayerColor color) const;

    bool operator==(const IncrementalMaterialAndPSTEvaluator& other) const;
private:
    //! Score estimation for white player
    Score m_estimatedScore;
};

#endif // INCREMENTALMATERIALANDPSTEVALUATOR_H
