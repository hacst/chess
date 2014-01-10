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

class PlayerDispatcherProxy;
using PlayerDispatcherProxyPtr = std::shared_ptr<PlayerDispatcherProxy>;

/**
 * @brief Class which holds the state GamePlay. This state is the essential part of
 * all states. The whole game play is hold in this state.
 *
 * @note To run() a state, first enter() the state.
 */
class GamePlay : public AbstractState {
public:
	//! The possible game modes.
	enum GameMode {
		AI_VS_AI,
		PLAYER_VS_AI
	};

	/**
	 * @brief Creates a new game.
	 * @param mode The GameMode (*AI vs. AI* or *Player vs. AI*).
	 * @param firstPlayerColor The color of the player which takes the first turn.
	 */
	GamePlay(GameMode mode, PlayerColor firstPlayerColor);

	/**
	 * @brief Enters the state for the first time. This will setup all the state related stuff.
	 * @note To run() the current state, first enter() it.
	 */
	void enter() override;

	/**
	 * @brief Runs the current state and does all the work.
	 * @return AbstractState* the state which should be run after this state.
	 * A nullptr if the game should be exited.
	 */
	AbstractState* run() override;

	/**
	 * @brief Exits the current state and cleans up all allocated resources.
	 * @note This is the last method to call, before the object is deleted.
	 */
	void exit() override;

	/**
	 * @brief Draws something state related stuff on the screen.
	 */
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
	PlayerDispatcherProxyPtr m_playerProxy;
	
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