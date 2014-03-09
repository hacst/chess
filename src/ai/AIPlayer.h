/*
    Copyright (c) 2013-2014, Stefan Hacker <dd0t@users.sourceforge.net>

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
#ifndef AIPLAYER_H
#define AIPLAYER_H

#include <mutex>
#include <atomic>
#include <chrono>
#include "logic/interface/AbstractPlayer.h"
#include "ai/Negamax.h"
#include "ai/PolyglotBook.h"
#include "core/Logging.h"

/**
 * @brief Artificial intelligence player implementation.
 */
class AIPlayer: public AbstractPlayer {
public:
    /**
     * @brief Creates a new AIPlayer.
     * @param AIConfiguration configuration to use for AI
     * @param name Logger channel name to use
     * @param seed Seed to use for random operations for the player.
     * @note Don't forget to start() it.
     */
    AIPlayer(
        const AIConfiguration& config,
        const std::string& name = "AIPlayer",
        int seed = 5253);

    ~AIPlayer();

    /**
     * @brief Starts the AIPlayer thread.
     */
    void start();

    // See AbstractPlayer
    virtual void onSetColor(PlayerColor color) override;
    virtual void onGameStart(GameState state, GameConfiguration config) override;
    virtual std::future<Turn> doMakeTurn(GameState state) override;
    virtual void doAbortTurn() override;
    virtual void onGameOver(GameState, PlayerColor) override;

    //! States for AIPlayer
    enum States {
        PREPARATION, //!< Game preparation phase
        PONDERING, //!< Pondering during enemies turn
        PLAYING, //!< Playing own turn
        STOPPED //!< Stopped operations
    };

    /**
     * @return Return current state.
     */
    States getState() const;

private:
    /**
     * @brief Executes AIPlayer state machine choosing to play, ponder or stop.
     */
    void run();

    /**
     * @brief State used when asked to make a turn.
     * Employs Negamax search to return a reasonable turn to the game logic.
     */
    void play();
    
    /**
     * @brief State between turns.
     * Can be used for additional processing (e.g. iterative deepening).
     */
    void ponder();

    /**
     * @brief Changes AIPlayer state.
     * @note A STOPPED AI cannot be restarted.
     * @param newState New state to adopt.
     */
    void changeState(States newState);

    //! Search opening book and fulfill promise if possible. If not return false.
    bool tryFindPromisedTurnInOpeningBook();
    //! Use negamax to iteratively search for the turn an fulfill with best found.
    void searchForPromisedTurn();
    //! Use negamax while discarding its results to fill transposition table.
    void performIterativeDeepening();
    //! Complete the promise and prepare the AI for pondering.
    void completePromiseWith(const Turn& turn);

    /**
     * @brief Performs an abortable negamax search up to the given depth.
     * @param depth Depth to search to.
     * @param state State to search from.
     * @param aiState Current ai state for abortion checks
     * @return Turn if depth was reached. None otherwise.
     */
    boost::optional<Turn> performSearchIteration(size_t depth, GameState& state, States aiState);

    //! Returns false if a time limit expired or the current state must be left.
    bool canStayInState(States currentState);
    //! Sets a time limit that can be checked with @see canStayInState
    void setTimeLimit(std::chrono::milliseconds limit);
    
    //! Holds the promise during fulfillment (@see play).
    std::promise<Turn> m_promisedTurn;

    /**
     * @brief State of the AI.
     * @warning Protected by m_stateMutex.
     */
    std::atomic<States> m_playerState;
    //! Mutex for m_playerState
    std::mutex m_stateMutex;

    //! Last notion of game state for the AI
    GameState m_gameState;
    //! State for the AI to ponder on between turns
    GameState m_ponderGameState;

    //! Game configuration the AI works with.
    GameConfiguration m_gameConfig;

    //! Color the AI is playing as.
    PlayerColor m_color;
    
    //! Algorithm used for search.
    Negamax<GameState, true, true, true> m_negamax;
    //! Thread the AI is run on.
    std::thread m_thread;
    
    //! Opening book (potentially unititialized)
    PolyglotBook m_openingBook;
    //! Indicates that we had a miss on the book and no longer use it
    bool m_outOfBook;

    //! Depth limit for iterative deepening
    size_t m_maxIterationDepth;
    //! Maximum time usable for turn
    std::chrono::seconds m_maxTimeForTurn;

    //! Timeout timer (@see setTimeLimit @see canStayInState)
    std::chrono::high_resolution_clock::time_point m_timeoutExpirationTime;

    //! AI configuration
    const AIConfiguration m_config;

    //! True if the AI has found a way to win.
    bool m_hasWinningMove;
    
    Logging::Logger m_log;
};

namespace std {
std::ostream& operator <<(std::ostream& stream, const AIPlayer::States s);
}

using AIPlayerPtr = std::shared_ptr<AIPlayer>;

#endif // AIPLAYER_H
