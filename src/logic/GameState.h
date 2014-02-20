#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "ChessTypes.h"
#include "ChessBoard.h"
#include "TurnGenerator.h"

class GameState {
public:
    GameState();
    explicit GameState(const ChessBoard& chessBoard);

    //! Returns a list with all possible and legal turns.
    std::vector<Turn> getTurnList() const;
    //! Applies the given turn on current chessboard.
    void applyTurn(const Turn& turn);

    //! Return next player to make a turn.
    PlayerColor getNextPlayer() const;
    //! Provides access to the chessboard.
    const ChessBoard& getChessBoard() const;
    /**
     * @brief Returns the captured piece from the last turn or
     * Piece(NoPlayer, NoType) if no piece was captured
     */
    Piece getLastCapturedPiece() const;

    //! Returns true if the game is over
    bool isGameOver() const;
    /**
    * @brief Returns the winner of the game.
    * Returns Player color or NoPlayer on draw.
    */
    PlayerColor getWinner() const;

    //! Returns true if the game is draw due to the 50 moves rule
    bool isDrawDueTo50MovesRule() const;

    //! Returns current score estimate from next players POV.
    Score getScore(size_t depth = 0) const;
    //! Returns hash for current position
    Hash getHash() const;

    bool operator==(const GameState& other) const;
    bool operator!=(const GameState& other) const;
    std::string toString() const;

private:
    //! Initialize the turn generator with the given chessboard.
    void init();
    //! Chessboard representation and logic.
    ChessBoard m_chessBoard;
    //! Turn generator and gameover detection.
    TurnGenerator m_turnGen;
};


#endif // GAMESTATE_H
