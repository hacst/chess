#include "AIPlayer.h"

#include <boost/log/attributes/constant.hpp>
#include "misc/helper.h"
#include <memory>
#include <future>

using namespace std;
using namespace std::chrono;
using namespace Logging;

AIPlayer::AIPlayer(const AIConfiguration& config, const string& name, int seed)
    : m_promisedTurn()
    , m_playerState(STOPPED)
    , m_gameState()
    , m_gameConfig()
    , m_color(PlayerColor::NoPlayer)
    , m_negamax()
    , m_thread()
    , m_openingBook(seed)
    , m_outOfBook(true)
    , m_maxIterationDepth(numeric_limits<size_t>::max())
    , m_config(config)
    , m_log(initLogger(name)) {
    
    LOG(info) << "Using seed " << seed;
    LOG(info) << config;
    // Empty
}

void AIPlayer::start() {
    m_playerState = PREPARATION;

    m_thread = thread([this] {
        run();
    });
}

AIPlayer::~AIPlayer() {
    changeState(STOPPED);

    if (m_thread.joinable()) {
        m_thread.join();
    }
}

void AIPlayer::onSetColor(PlayerColor color) {
    LOG(info) << "Got color " << color;
    m_color = color;
}

void AIPlayer::onGameStart(GameState state, GameConfiguration gameConfig) {
    LOG(info) << "Game start";

    m_gameState = state;
    m_ponderGameState = state;

    // The time limits set for the game are important. Give us an extra second
    // to reply to those to make sure we don't time out on our moves.
    m_maxTimeForTurn = seconds(
        std::max(1, // Minimum of one second
        std::min(
            m_config.maximumTimeForTurnInSeconds,
            gameConfig.maximumTurnTimeInSeconds - 1)));

    LOG(info) << "Max turn time set to " << m_maxTimeForTurn.count() << "s";

    if (!m_config.openingBook.empty()) {
        if (!m_openingBook.open(m_config.openingBook)) {
            LOG(warning) << "Failed to load opening book. AI will play without one";
            m_outOfBook = true;
        } else {
            // Enable book
            LOG(info) << "Using opening book '" << m_config.openingBook
                << "' with " << m_openingBook.getNumberOfEntries() << " entries";

            m_outOfBook = false;
        }
    } else {
        LOG(info) << "Not using opening book";
        m_outOfBook = true;
    }

    changeState(PONDERING);
}

future<Turn> AIPlayer::doMakeTurn(GameState state) {
    LOG(info) << "Asked to make turn";
    assert(m_playerState != PLAYING);

    m_promisedTurn = promise<Turn>();
    m_gameState = state;

    changeState(PLAYING);
    return m_promisedTurn.get_future();
}

void AIPlayer::doAbortTurn() {
    changeState(PONDERING);
}

bool AIPlayer::tryFindPromisedTurnInOpeningBook() {
    if (m_outOfBook)
        return false;
    
    const Hash hash = m_gameState.getHash();
    auto entry = m_openingBook.getWeightedEntry(hash);
    if (entry) {
        LOG(info) << "Book entry found for " << std::hex << m_gameState.getHash() << std::dec <<": " << *entry;
        bool found = false;

        // Check turn against possible moves to detect collisions
        for (const auto& turn : m_gameState.getTurnList()) {
            if (entry->move.from == turn.from
                && entry->move.to == turn.to) {
                //TODO: Fix this for pawn promotion and castling once we support that. For now comparing source and target should do.
                completePromiseWith(turn);
                LOG(info) << "Performing turn from book: " << turn;
                found = true;
                break;
            }
        }
        
        if (!found) {
            LOG(error) << "Book proposed impossible turn. This could either be a turn generation bug or a hash collision";
            LOG(error) << m_gameState;
            m_outOfBook = true;
            return false;
        }

    } else {
        m_outOfBook = true;
        LOG(info) << "Now out of book";
        return false;
    }
    
    return true;
}

void AIPlayer::completePromiseWith(const Turn& turn) {
    m_ponderGameState = m_gameState;
    m_ponderGameState.applyTurn(turn);

    m_promisedTurn.set_value(turn);
}

void AIPlayer::searchForPromisedTurn() {
    boost::optional<Turn> turnWithFarthestHorizon;
    size_t iteration = 1;

    while (canStayInState() && iteration <= m_config.maximumDepth) {
        auto turn = performSearchIteration(iteration, m_gameState);
        if (!turn) break;

        turnWithFarthestHorizon = turn;
        ++iteration;
    }
    
    if (turnWithFarthestHorizon) {
        LOG(info) << "Reached " << iteration - 1 << " plies. Found " << *turnWithFarthestHorizon;
        completePromiseWith(*turnWithFarthestHorizon);
    } else {
        LOG(warning) << "No viable solution found in time. Breaking promise";
    }
}

void AIPlayer::play() {
    setTimeLimit(m_maxTimeForTurn);

    if(!tryFindPromisedTurnInOpeningBook()) {
        searchForPromisedTurn();
    }

    changeState(PONDERING);
}

boost::optional<Turn> AIPlayer::performSearchIteration(size_t depth, GameState& state) {
    LOG(info) << "Starting search of depth " << depth;
    
    future<NegamaxResult> result = async(launch::async, [&]{
        return m_negamax.search(state, depth);
    });

    while (canStayInState()) {
        future_status status = result.wait_for(milliseconds(50));
        if (status == future_status::ready) {
            return result.get().turn;
        }
    }

    m_negamax.abort();
    result.wait(); // Make sure we notice if our async thread hangs due to a bug.

    return boost::none;
}

bool AIPlayer::canStayInState() {
    return chrono::high_resolution_clock::now() < m_timeoutExpirationTime && !m_leaveCurrentState;
}

void AIPlayer::setTimeLimit(chrono::milliseconds limit) {
    m_timeoutExpirationTime = chrono::high_resolution_clock::now() + limit;
}

void AIPlayer::ponder() {
    if (m_config.ponderDuringOpposingPly) {
        setTimeLimit(chrono::hours(2)); // Practically no limit

        performIterativeDeepening();
    } else {
        while (canStayInState()) {
            this_thread::sleep_for(milliseconds(100));
        }
    }
}

void AIPlayer::performIterativeDeepening() {
    size_t iteration = 1;
    while (canStayInState()
        && iteration <= m_config.maximumDepth
        && performSearchIteration(iteration, m_gameState)) {

        LOG(info) << "Pondered " << iteration << " plies deep";
        ++iteration;
    }
}

void AIPlayer::run() {
    while (m_playerState != STOPPED) {
        m_leaveCurrentState = false;

        switch (m_playerState) {
        case PONDERING: ponder(); break;
        case PLAYING: play(); break;
        default: break;
        }
    }
    LOG(info) << "AIPlayer stopped";
}

void AIPlayer::onGameOver(GameState, PlayerColor) {
    changeState(STOPPED);
}

AIPlayer::States AIPlayer::getState() const {
    return m_playerState;
}

void AIPlayer::changeState(States newState) {
    lock_guard<mutex> lock(m_stateMutex);

    if (m_playerState != newState && m_playerState != STOPPED) {
        m_leaveCurrentState = true;
        LOG(info) << "Now " << newState;

        m_playerState = newState;
    }
}

namespace std {

std::ostream& operator <<(std::ostream& stream, const AIPlayer::States s) {
    switch(s) {
    case AIPlayer::PREPARATION:
        stream << "PREPARATION";
        break;
    case AIPlayer::PONDERING:
        stream << "PONDERING";
        break;
    case AIPlayer::PLAYING:
        stream << "PLAYING";
        break;
    case AIPlayer::STOPPED:
        stream << "STOPPED";
        break;
    default:
        stream << "<UNKNOWN>";
        break;
    }

    return stream;
}

}
