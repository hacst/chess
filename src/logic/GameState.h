#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "ChessTypes.h"
#include "ChessBoard.h"
#include "TurnGenerator.h"
#include "Evaluator.h"


class GameState {
public:
    GameState();
    virtual void init();
    //virtual void init(SaveGame* sg);

    virtual std::vector<Turn>   getTurnList();
    virtual void                applyTurn(const Turn& t);
    virtual PlayerColor getNextPlayer() const;
    virtual const ChessBoard& getChessBoard() const;
    //virtual TurnGeneratorPtr    getTurnGenerator() const;

    bool isGameOver();
    bool operator==(const GameState& other) const;
    bool operator!=(const GameState& other) const;
    std::string toString() const;

private:
    PlayerColor     m_nextPlayer;
    ChessBoard      m_chessBoard;
    TurnGenerator   m_turnGen; // TODO: replace with AbstractTurnGenerator
    //AbstractTurnGenerator m_turnGen;
    //EvaluatorPtr     m_evalualter; // TODO: do we need this here?
};

using GameStatePtr = std::shared_ptr<GameState>;


#endif // GAMESTATE_H
