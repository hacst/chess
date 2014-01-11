#include "AIPlayer.h"

#include <boost/log/attributes/constant.hpp>
#include "misc/helper.h"
#include "logic/Evaluators.h"
#include <memory>
#include <chrono>

using namespace std;
using namespace std::chrono;
using namespace Logging;

AIPlayer::AIPlayer(int seed)
    : m_promisedTurn()
    , m_playerState(STOPPED)
    , m_gameState()
    , m_gameConfig()
    , m_color(PlayerColor::NoPlayer)
    , m_algorithm()
    , m_thread()
    , m_openingBook(seed)
    , m_outOfBook(true)
    , m_log(initLogger("AIPlayer")) {
    
    LOG(info) << "Using seed " << seed;
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

void AIPlayer::onGameStart(GameState state, GameConfiguration config) {
    LOG(info) << "Game start";

    m_gameState = state;
    m_gameConfig = config;

    if (!m_gameConfig.openingBook.empty()) {
        if (!m_openingBook.open(m_gameConfig.openingBook)) {
            LOG(warning) << "Failed to load opening book. AI will play without one";
            m_outOfBook = true;
        } else {
            // Enable book
            LOG(info) << "Using opening book '" << m_gameConfig.openingBook
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
                m_promisedTurn.set_value(turn);
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

void AIPlayer::selectPromisedTurnBySearch() {
    const size_t DEPTH = 4;
    LOG(info) << "Starting search of depth " << DEPTH;
    auto result = m_algorithm.search(m_gameState, DEPTH);

    // Pass on result for turn
    if (result.turn) {
        LOG(info) << "Completed search, best score " << result.score << " with turn " << result.turn.get();
        m_promisedTurn.set_value(result.turn.get());
    }
    else {
        LOG(info) << "Aborted search, no turn possible";
        m_promisedTurn.set_value(Turn());
    }    
}

void AIPlayer::play() {
    if(!tryFindPromisedTurnInOpeningBook()) {
        selectPromisedTurnBySearch();
    }

    changeState(PONDERING);
}

void AIPlayer::ponder() {
    // Not implemented yet, sleep instead
    this_thread::sleep_for(milliseconds(100));
}

void AIPlayer::run() {
    while (m_playerState != STOPPED) {
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
