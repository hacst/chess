#ifndef EVALUATORS_H
#define EVALUATORS_H

#include <array>

#include "interface/AbstractEvaluator.h"
#include "logic/ChessTypes.h"

class ChessBoard;
class Turn;

/**
 * @brief Class for incrementally estimating game state.
 */
class IncrementalBoardEvaluator {
public:
    //! Initializes the evaluator for a prestine board.
    IncrementalBoardEvaluator();
    //! Initializes the evaluator for an already played board.
    explicit IncrementalBoardEvaluator(const std::array<Piece, 64> &board);

    //! Updates estimate for the moving of the piece in give turn.
    void moveIncrement(const Turn& turn);
    //! Updates estimate for a capture of the given piece on the given field.
    void captureIncrement(Field field, const Piece& piece);

    //! Gives a full estimate for the given board
    static Score estimateFullBoard(const std::array<Piece, 64> &board);

    //! Returns the score from the perspective of the given player color.
    Score getScore(PlayerColor color) const;

    bool operator==(const IncrementalBoardEvaluator& other) const;
private:
    //! Score estimation for white player
    Score m_estimatedScore;
};

class MaterialEvaluator: public AbstractEvaluator {
public:
	virtual Score getScore(const GameState& gameState) const override;
	virtual Score getMaterialWorth(PlayerColor player, const ChessBoard& board) const;
};

using MaterialEvaluatorPtr = std::shared_ptr<MaterialEvaluator>;


#endif // EVALUATORS_H
