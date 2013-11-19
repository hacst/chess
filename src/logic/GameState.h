#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <memory>
#include "ChessBoard.h"
#include "TurnGenerator.h"
#include "Evaluater.h"

enum PlayerColor {
    White, Black, Both
};

class GameState {
public:
    virtual void init();
    //virtual void init(SaveGame* sg);

    virtual ChessBoardPtr getChessBoard();
    virtual std::vector<Turn> getTurnList();
    virtual void applyTurn(const Turn& t);

private:
    EvaluaterPtr     m_evaluater; // TODO: do we need this here?
    TurnGeneratorPtr m_turnGenerator;
    ChessBoardPtr    m_board;
    PlayerColor      m_next;
};

using GameStatePtr = std::shared_ptr<GameState>;

#endif // GAMESTATE_H
