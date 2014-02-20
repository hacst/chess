#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "ChessTypes.h"
#include "ChessBoard.h"
#include "TurnGenerator.h"


class GameState {
public:
    GameState();
    explicit GameState(const ChessBoard& chessBoard);

    void init();
    //virtual void init(SaveGame* sg);

    std::vector<Turn>   getTurnList() const;
    void                applyTurn(const Turn& turn);
    PlayerColor         getNextPlayer() const;
    const ChessBoard&   getChessBoard() const;

    //! Returns true if the game is over
    bool isGameOver() const;

    /**
    * @brief Returns the winner of the game.
    * Returns Player color or NoPlayer on draw.
    * @warning Only valid is isGameOver
    */
    PlayerColor getWinner() const;

    //! Returns true if the game is draw due to the 50 moves rule
    bool isDrawDueTo50MovesRule() const;

    //! Returns current score estimate from next players POV.
    Score getScore() const;
    //! Returns hash for current position
    Hash getHash() const;

    bool operator==(const GameState& other) const;
    bool operator!=(const GameState& other) const;
    std::string toString() const;

private:
    ChessBoard      m_chessBoard;
    TurnGenerator   m_turnGen;
};

using GameStatePtr = std::shared_ptr<GameState>;


#endif // GAMESTATE_H
