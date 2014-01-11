#ifndef AIPLAYER_H
#define AIPLAYER_H

#include <mutex>
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
     * @param seed Seed to use for random operations for the player.
     * @note Don't forget to start() it.
     */
    AIPlayer(int seed = 5253);
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
        PREPARATION, // Game preparation phase
        PONDERING, // Pondering during enemies turn
        PLAYING, // Playing own turn
        STOPPED
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

    bool tryFindPromisedTurnInOpeningBook();
    void selectPromisedTurnBySearch();
    
    //! Holds the promise during fulfillment (@see play).
    std::promise<Turn> m_promisedTurn;

    /**
     * @brief State of the AI.
     * @warning Protected by m_stateMutex.
     */
    volatile States m_playerState;
    //! Mutex for m_playerState
    std::mutex m_stateMutex;

    //! Last notion of game state for the AI
    GameState m_gameState;
    //! Game configuration the AI works with.
    GameConfiguration m_gameConfig;

    //! Color the AI is playing as.
    PlayerColor m_color;
    
    //! Algorithm used for search.
    Negamax<> m_algorithm;
    //! Thread the AI is run on.
    std::thread m_thread;
    
    //! Opening book (potentially unititialized)
    PolyglotBook m_openingBook;
    //! Indicates that we had a miss on the book and no longer use it
    bool m_outOfBook;

    Logging::Logger m_log;
};

namespace std {
std::ostream& operator <<(std::ostream& stream, const AIPlayer::States s);
}

using AIPlayerPtr = std::shared_ptr<AIPlayer>;

#endif // AIPLAYER_H
