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
#include "gui/ResourceInitializer.h"

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
    //! The possible game modes to chose.
    enum GameMode {
        AI_VS_AI,
        PLAYER_VS_AI
    };

    /**
     * @brief Creates a new game.
     * @param mode The GameMode (*AI vs. AI* or *Player vs. AI*).
     * @param firstPlayerColor The color of the player which takes the first turn.
     * @param initialFen If set overrides the configured initial FEN
     */
    GamePlay(GameMode mode, PlayerColor humanPlayerColor, std::string initialFen = "");

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
     * @brief Draws all relevant and state related stuff on the screen.
     */
    void draw();

    /**
     * @brief Call this function to draw text on the right bottom side of the viewport.
     * @param text The string to show.
     */
    void startShowText(std::string text);

    /**
     * @brief When the other player is on turn, this method switched the camera position
     * and shows a small text message which player is on turn.
     * @param color The PlayerColor of the current player, which is on turn.
     * @note This should always be called to show a message to the user.
     */
    void switchToPlayerColor(PlayerColor color);

    /**
     * @brief Method for setting the new chess state. This method is non-blocking.
     * @param state The current chess board state to set.
     * @param lastPlayer The player which was last on turn.
     * @param lastTurn The last turn which was made.
     */
    void setState(std::array<Piece, 64> state, PlayerColor lastPlayer, Turn lastTurn);

    /**
     * @brief Method for setting the new chess state. This method is non-blocking.
     * @param state The current chess board state to set.
     * @note This should only be called one time, when the game starts.
     */
    void setState(std::array<Piece, 64> state);

    /**
     * @brief Sets the game state to the given one.
     * @param gameState The new gameState to set.
     */
    void setGameState(const GameState& gameState);

    /**
     * @brief This method changed the internal state to interact with a human player
     * if the game mode is PLAYER_VS_AI.
     * @param who The color of the player who's on turn.
     * @note This method should be called on each turn start. Otherwise the GUI can't react
     * to keyboard input for a human player.
     */
    void onPlayerIsOnTurn(PlayerColor who);

    /**
     * @brief If the human player aborts his current turn, this method switches to
     * the AI player.
     * @note This method currently only works for PLAYER_VS_AI. If there are in the
     * future other game modes, this must be corrected.
     */
    void onPlayerAbortTurn();

    /**
     * @brief Tells the human to make a turn.
     * @return The Turn as a future.
     */
    std::future<Turn> doMakePlayerTurn();

private:
    //! The state machine to access window functions.
    StateMachine& m_fsm;

    //! Number in degree (0-360 deg) to rotate from/to.
    int m_rotateFrom, m_rotateTo;

    //! The game mode, one of: PLAYER_VS_AI or AI_VS_AI.
    GameMode m_gameMode;

    //! The color of the human player.
    PlayerColor m_humanPlayerColor;

    //! Flag to lock the camera rotation on each turn change.
    bool m_lockCamera;

    //! Smart pointer to the keyboard navigation handler to manage human field navigation.
    ArrowNavigationHandlerPtr m_arrowNavHandler;

    //! Smart pointer to the resource initializer.
    ResourceInitializerPtr m_resourceInitializer;

    //! All possible turns to take for a selected figure/model.
    std::vector<Turn> m_possibleTurns;

    //! Holds the promise during fulfillment.
    std::promise<Turn> m_promisedPlayerTurn;

    //! To stop triggering keys too often, this counter helps
    //! to delay each key stroke.
    struct KeyboardCounter {
        std::chrono::time_point<std::chrono::system_clock> keyR;
        std::chrono::time_point<std::chrono::system_clock> keyReturn;
    } m_kCounter;

    //! Holds the 64 field chess board state with models, types, colors, ...
    std::array<Piece, 64> m_chessBoardState;

    //! Holds the whole game state.
    GameState m_gameState;

    //! Struct that contains all captured pieces for the black and white
    //! player as also the OpenGL display lists for the black/white bars.
    struct CapturedPieces {
        std::array<int, 6> countBlack;
        std::array<int, 6> countWhite;

        GLuint blackBar;
        GLuint whiteBar;
    } m_capturedPieces;

    //! The main states for the StateMachine.
    enum States {
        KEEP_CURRENT,

        BACK_TO_MENU
    } m_nextState;

        //! Internal state in the GamePlay class.
        enum InternalState {
            AI_ON_TURN,
            PLAYER_ON_TURN,
            PAUSE,
            SAVE_GAME
        } m_internalState, m_lastInternalState;

            //! Internal player state (only available if the game mode is PLAYER_VS_AI).
            enum PlayerState {
                NONE,
                CHOOSE_PROMOTION_TURN
            } m_playerState;
    
    //! Holds the possible promotion turns for the human player.
    Turn m_promotionTurns[4];

    //! Struct which represents a players turn.
    struct PlayerTurn {
        PlayerColor who;
        Turn turn;
    };

    //! Holds all the human player made turns.
    std::deque<PlayerTurn> m_playerTurns;

    //! The last player who was on turn.
    PlayerColor m_lastPlayer;

    //! The last turn which was made.
    Turn m_lastTurn;

    //! Flag, if the first turn was made. For camera rotation.
    bool m_firstTurn;

    //! Animation helpers to handle (time dependent) animations.
    AnimationHelperPtr m_animationHelperCamera, m_animationHelperBackground;

    //! Smart pointer to the chess set.
    ChessSetPtr m_chessSet;

    //! Smart pointer to pause and save menu.
    Menu2DPtr m_pauseMenuMain, m_pauseMenuSave;

    //! Struct that represents the message box on the top. If this box gets more fancy
    //! or even more complex this should be moved to a separate class.
    struct MessageBox {
        //! width of the box
        unsigned int width;
        //! height of the box
        unsigned int height;
        //! inner padding to the edges
        unsigned int padding;
        //! currently not used
        unsigned int showDuration;
        //! text to show on viewport
        std::string text;

        //! currently not used
        unsigned int shownSince;
        //! viewport x coordinate
        unsigned int windowPosX;
        //! viewport y coordinate
        unsigned int windowPosY;

        //! OpenGL display list which stores the OpenGL background part.
        GLuint displayList;
    } m_messageBox;

    //! The first light position.
    GLfloat m_lightPos0[3];

    //! The second light position.
    GLfloat m_lightPos1[3];

    //! Smart pointer for the player.
    AbstractPlayerPtr m_firstPlayer, m_secondPlayer;

    //! Smart pointer for the game logic.
    AbstractGameLogicPtr m_gameLogic;

    //! Smart pointer for the observer
    GuiObserverPtr m_observer;

    //! Smart pointer for the observer proxy
    ObserverDispatcherProxyPtr m_observerProxy;

    //! Smart pointer for the player proxy
    PlayerDispatcherProxyPtr m_playerProxy;

    //! The initial FEN notation string to load the game state from.
    std::string m_initialFen;

    //! boost::log Logger for universal info, debug and error logging.
    Logging::Logger m_log;
    
    /**
     * @brief The functions returns the name of the piece/model for the given number.
     * @param pieceNumber A number between 0 and 6 corresponding to the PieceType.
     * @return The name of the piece.
     */
    string getPieceName(int pieceNumber);

    /**
     * @brief Initializes the OpenGL lightning for the scene.
     */
    void initLighting();

    /**
     * @brief Initializes the whole chess set (all models and chess board).
     */
    void initChessSet();

    /**
     * @brief Initializes all the animation helpers.
     */
    void initAnimationHelpers();

    /**
     * @brief Initializes the main pause menu with all the buttons.
     */
    void initMenuPause();

    /**
     * @brief Initializes the save pause menu with all the buttons.
     */
    void initMenuSaveGame();

    /**
     * @brief Initializes the players depending on the game mode.
     */
    void initPlayers();

    /**
     * @brief Initializes the game logic depending on the human player color
     * and given FEN game state.
     */
    void initGameLogic();

    /**
     * @brief Initializes the message box to show text to the human player.
     */
    void initMessageBox();

    /**
     * @brief Initializes the camera rotation and initial position.
     */
    void initCamera();

    /**
     * @brief Initializes the widget for the captures pieces to show on the
     * HUD for the human player.
     */
    void initCapturedPieces();

    /**
     * @brief Initializes the counters for the stricken models.
     * @param initialGameState The initial game state to init the game from.
     */
    void initPieceCounters(GameState& initialGameState);

    /**
     * @brief Faded the background for exactly one time, when the game starts.
     */
    void fadeBackgroundForOneTime();

    /**
     * @brief Sets the number of captured pieces to zero for each color.
     */
    void resetCapturedPieces();

    /**
     * @brief Rotates the camera with an animation helper if necessary and
     * only if the animation time is not over.
     */
    void rotateCamera();

    /**
     * @brief Calculates the new camera coordinates from a given degree between 0-360 deg and
     * positions the camera in the world space with respect to the animated rotation around
     * the Y-axis.
     * @param degree The angle in degree between 0 - 360.
     */
    void setCameraPosition(float degree);

    /**
     * @brief Does all the 2D drawing action like menu, widgets and text rendering.
     */
    void draw2D();

    /**
     * @brief Does all the 3D drawing action like chess board, models and so on.
     */
    void draw3D();

    /**
     * @brief Draws the message box at the initialized position and the set text.
     */
    void drawMessageBox();

    /**
     * @brief Draws the last fice turns on the left bottom side of the viewport.
     */
    void drawLastTurns();

    /**
     * @brief Draws the info string on the right bottom side of the viewport to
     * show some extra information.
     */
    void drawInfoBox(string msg);
    
    /**
     * @brief Draws all the captured pieces statistic for the black and white
     * player on the left top and right top side of the viewport.
     */
    void drawCapturedPieces();

    /**
     * @brief Draws the pause menu.
     */
    void drawPauseMenu();

    /**
     * @brief Draws the human-interaction things like colorized tiles
     * when a model is selected or the cursor-highlight-tile to choose
     * a model.
     */
    void drawPlayerActions();

    /**
     * @brief Enables lighting.
     */
    void enableLighting();

    /**
     * @brief Disables lighting.
     */
    void disableLighting();

    /**
     * @brief Handles all the human player keyboard and mouse events.
     */
    void handleEvents();

    /**
     * @brief Starts the camera rotation and updates the start and end
     * position of the camera.
     */
    void startCameraRotation();

    /**
     * @brief If the internal game state changed to PAUSE, this method
     * is called.
     */
    void onPauseGame();

    /**
     * @brief If the internal game state was PAUSE and is now AI_ON_TURN or PLAYER_ON_TURN.
     */
    void onResumeGame();

    /**
     * @brief If the player wants to save the game, the internal game state changes to
     * SAVE_GAME.
     */
    void onSaveGame();

    /**
     * @brief This must be called when saving the current game to game slot 1.
     */
    void onSaveSlot1();

    /**
     * @brief This must be called when saving the current game to game slot 2.
     */
    void onSaveSlot2();

    /**
     * @brief This must be called when saving the current game to game slot 3.
     */
    void onSaveSlot3();

    /**
     * @brief Saves the game at a given slot.
     * @param slot The slot number between 0-2.
     */
    void saveGameToSlot(unsigned int slot);

    /**
     * @brief Changes the internal state to PAUSE.
     */
    void onMenuSaveBack();

    /**
     * @brief Calls the players to abort the current turn and leaves the game.
     */
    void onLeaveGame();
};

using GamePlayPtr = std::shared_ptr<GamePlay>;

#endif // GAMEPLAY_H
