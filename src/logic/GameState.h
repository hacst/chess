#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "ChessTypes.h"
#include "ChessBoard.h"
#include "TurnGenerator.h"


class GameState {
public:
	GameState();
	explicit GameState(const ChessBoard& chessBoard);

	virtual void init();
	//virtual void init(SaveGame* sg);

	virtual std::vector<Turn>   getTurnList() const;
	virtual void                applyTurn(const Turn& turn);
	virtual PlayerColor         getNextPlayer() const;
	virtual const ChessBoard&   getChessBoard() const;

	bool isGameOver() const;
    PlayerColor getWinner() const;
    //! Returns current score estimate from next players POV.
    Score getScore() const;

	bool operator==(const GameState& other) const;
	bool operator!=(const GameState& other) const;
	std::string toString() const;

private:
	ChessBoard      m_chessBoard;
	TurnGenerator   m_turnGen;
	// TODO: replace with AbstractTurnGenerator
	//AbstractTurnGenerator m_turnGen;
};

using GameStatePtr = std::shared_ptr<GameState>;


#endif // GAMESTATE_H
