/*
    Copyright (c) 2013-2014, Patrick Hillert <silent@gmx.biz>

    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice,
    this list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in the
    documentation and/or other materials provided with the distribution.

    3. Neither the name of the copyright holder nor the names of its
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
    ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
    LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
    CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
    SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
    INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
    CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
    ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
    POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include "logic/GameLogic.h"
#include "gui/GuiObserver.h"
#include "gui/interface/AbstractState.h"
#include "gui/Menu2D.h"
#include "gui/ChessSet.h"
#include "core/Logging.h"
#include "gui/ArrowNavigationHandler.h"

#include <vector>
#include <array>
#include <deque>
#include <chrono>

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
    GamePlay(GameMode mode, PlayerColor humanPlayerColor);

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

    void setGameState(const GameState& gameState);

    /**
    * @brief Method for setting the new turn, which changed the chess state.
    * @note Be sure to first call this and *after* call setState.
    * This method is non-blocking.
    */
    //void addTurn(PlayerColor who, Turn turn);
    //void setCapturedPiecesList(std::vector<Piece> piecesList);

    // events and callbacks
    void onBeforeLoadNextResource(std::string resourceName);	// callback method for ChessSet
    void onResumeGame();
    void onSaveGame();
    void onLeaveGame();
    void onBackToMenu();
    void onPlayerIsOnTurn(PlayerColor who);
    void onPlayerAbortTurn();
    std::future<Turn> doMakePlayerTurn();

private:
    StateMachine& m_fsm;
    int m_rotateFrom, m_rotateTo;
    GameMode m_gameMode;
    PlayerColor m_humanPlayerColor;
    bool m_lockCamera;
    ArrowNavigationHandlerPtr m_arrowNavHandler;

    std::vector<Turn> m_possibleTurns;
    Turn m_userChosenPromotionTurn;
    bool m_promotionTurnAvailable;

    //! Holds the promise during fulfillment.
    std::promise<Turn> m_promisedPlayerTurn;

    struct KeyboardCounter {
        std::chrono::time_point<std::chrono::system_clock> keyR;
        std::chrono::time_point<std::chrono::system_clock> keyReturn;
    } m_kCounter;

    std::array<Piece, 64> m_chessBoardState;
    GameState m_gameState;

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

    Logging::Logger m_log;

    // States are organized as followed:
    // 
    // States
    //   |-- InternalState
    //      |-- PlayerState
    enum InternalState {
        AI_ON_TURN,
        PLAYER_ON_TURN,
        PAUSE,
        SAVE_GAME
    } m_internalState, m_lastInternalState;

    enum PlayerState {
        NONE,
        CHOOSE_PROMOTION_TURN
    } m_playerState;

    Turn m_promotionTurns[4];

    struct PlayerTurn {
        PlayerColor who;
        Turn turn;
    };

    std::deque<PlayerTurn> m_playerTurns;
    PlayerColor m_lastPlayer;
    Turn m_lastTurn;
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
    void draw2D();
    void draw3D();
    void drawMessageBox();
    void drawLastTurns();
    void drawInfoBox(string msg);
    void drawCapturedPieces();
    void drawPauseMenu();
    void drawPlayerActions();
    void enableLighting();
    void disableLighting();
    void handleEvents();
    void startCameraRotation();
    void onPauseGame();
};

using GamePlayPtr = std::shared_ptr<GamePlay>;

#endif // GAMEPLAY_H
