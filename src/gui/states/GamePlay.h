#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include "logic/GameLogic.h"
#include "gui/GuiObserver.h"
#include "gui/interface/AbstractState.h"
#include "gui/Menu2D.h"
#include "gui/ChessSet.h"
#include "core/Logging.h"

#include <vector>
#include <array>
#include <deque>

class StateMachine;
class ObserverDispatcherProxy;
using ObserverDispatcherProxyPtr = std::shared_ptr<ObserverDispatcherProxy>;

class GamePlay : public AbstractState {
public:
	enum GameMode {
		AI_VS_AI,
		PLAYER_VS_AI
	};

	GamePlay(GameMode mode, PlayerColor firstPlayerColor);

	// interface methods
	void enter() override;
	AbstractState* run() override;
	void exit() override;

	// draw method
	void draw();

	// trigger methods
	void startShowText(std::string text);
	void switchToPlayerColor(PlayerColor color);

	/**
	* @brief Method for setting the new chess state.
	* This method is non-blocking.
	*/
	void setState(std::array<Piece, 64> state, PlayerColor lastPlayer, Turn lastTurn);
	void setState(std::array<Piece, 64> state);

	/**
	* @brief Method for setting the new turn, which changed the chess state.
	* @note Be sure to first call this and *after* call setState.
	* This method is non-blocking.
	*/
	//void addTurn(PlayerColor who, Turn turn);
	void setCapturedPiecesList(std::vector<Piece> piecesList);

	// events and callbacks
	void onBeforeLoadNextResource(std::string resourceName);	// callback method for ChessSet
	void onResumeGame();
	void onSaveGame();
	void onLeaveGame();
	void onBackToMenu();

private:
	StateMachine& m_fsm;
	int m_rotateFrom, m_rotateTo;
	GameMode m_gameMode;
	PlayerColor m_firstPlayerColor;
	bool m_lockCamera;

	std::array<Piece, 64> m_chessBoardState;

	// @todo -> own class
	struct CapturedPieces {
		std::array<int, 6> countBlack;
		std::array<int, 6> countWhite;

		GLuint blackBar;
		GLuint whiteBar;
	} m_capturedPieces;

	string getPieceName(int pieceNumber);

	enum States {
		KEEP_CURRENT,

		BACK_TO_MENU
	} m_nextState;

	enum InternalState {
		NOT_PAUSED,
		PAUSED,
		SAVE_GAME
	} m_internalState;

	struct PlayerTurn {
		PlayerColor who;
		Turn turn;
	};

	std::deque<PlayerTurn> m_playerTurns;
	Turn m_lastTurn;
	PlayerColor m_lastPlayer;
	bool m_firstTurn;

	// smart pointers
	AnimationHelperPtr m_animationHelperCamera, m_animationHelperBackground;
	ChessSetPtr m_chessSet;
	Menu2DPtr m_pauseMenu;

	// @todo: -> own class
	struct MessageBox {
		unsigned int width;
		unsigned int height;
		unsigned int padding;
		unsigned int showDuration;
		std::string text;

		unsigned int shownSince;
		unsigned int windowPosX;
		unsigned int windowPosY;

		GLuint displayList;
	} m_messageBox;

	int m_resourcesTotal;
	int m_resourcesLoaded;
	GLfloat m_lightPos0[3];
	GLfloat m_lightPos1[3];

	AbstractPlayerPtr m_firstPlayer, m_secondPlayer;
	AbstractGameLogicPtr m_gameLogic;
	GuiObserverPtr m_observer;
	ObserverDispatcherProxyPtr m_observerProxy;
	
	int posNumber;

    Logging::Logger m_log;
	
	// methods
	void initLighting();
	void initChessSet();
	void initAnimationHelpers();
	void initMenuPause();
	void initMenuSaveGame();
	void initPlayers();
	void initGameLogic();
	void initMessageBox();
	void initCamera();
	void initCapturedPieces();
	void resetCapturedPieces();
	void fadeBackgroundForOneTime();
	void rotateCamera();
	void setCameraPosition(float degree);
	void drawMessageBox();
	void drawLastTurns();
	void drawCapturedPieces();
	void drawPauseMenu();
	void enableLighting();
	void disableLighting();
	void handleEvents();
	void startCameraRotation();
};

using GamePlayPtr = std::shared_ptr<GamePlay>;

#endif // GAMEPLAY_H