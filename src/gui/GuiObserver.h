#ifndef GAMEOBSERVER_H
#define GAMEOBSERVER_H

#include <memory>

#include "logic/interface/AbstractGameObserver.h"

#include "gui/ChessSet.h"

class GamePlay;

class GuiObserver : public AbstractGameObserver {
public:
	GuiObserver(ChessSetPtr chessSetPtr, GamePlay& gamePlayState);

	void onGameStart(GameState state, GameConfiguration config) override;
	void onTurnStart(PlayerColor who) override;
	void onTurnEnd(PlayerColor who, Turn turn, GameState newState) override;
	void onTurnTimeout(PlayerColor who, std::chrono::seconds timeout) override;
	void onGameOver(GameState state, PlayerColor winner) override;

private:
	ChessSetPtr m_chessSetPtr;
	GamePlay& m_gamePlayState;
	PlayerColor m_currentPlayerColor;
};

using GuiObserverPtr = std::shared_ptr<GuiObserver>;

#endif // GAMEOBSERVER_H
