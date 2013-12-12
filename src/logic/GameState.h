#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "ChessTypes.h"
#include "ChessBoard.h"
#include "TurnGenerator.h"


class GameState {
public:
    GameState();
    virtual void init();
    //virtual void init(SaveGame* sg);

    virtual std::vector<Turn>   getTurnList();
    virtual void                applyTurn(const Turn& turn);
    virtual PlayerColor         getNextPlayer() const;
    virtual const ChessBoard&   getChessBoard() const;

    bool isGameOver();
    bool operator==(const GameState& other) const;
    bool operator!=(const GameState& other) const;
    std::string toString() const;

private:
    PlayerColor     m_nextPlayer;
    ChessBoard      m_chessBoard;
    TurnGenerator   m_turnGen;
    // TODO: replace with AbstractTurnGenerator
    //AbstractTurnGenerator m_turnGen;
};

using GameStatePtr = std::shared_ptr<GameState>;


#endif // GAMESTATE_H
