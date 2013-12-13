#include <algorithm>

#include "GameLogic.h"

using namespace std;
using namespace std::chrono;

GameLogic::GameLogic(AbstractPlayerPtr white, AbstractPlayerPtr black, GameConfigurationPtr config)
    : m_tickLength(500)
    , m_abort(false)
    , m_white(white)
    , m_black(black)
    , m_config(config){
    assert(white != black);

    addObserver(white);
    addObserver(black);
}

GameLogic::~GameLogic() {
    m_abort = true;

    if (m_thread.joinable()) {
        m_thread.join();
    }
}

AbstractPlayerPtr GameLogic::getWhite() const {
    return m_white;
}

AbstractPlayerPtr GameLogic::getBlack() const {
    return m_black;
}

void GameLogic::addObserver(AbstractGameObserverPtr observer) {
    m_observers.push_back(observer);
}

GameConfigurationPtr GameLogic::getConfiguration() const {
    return m_config;
}

void GameLogic::stop() {
    LOG(debug) << "Asked to stop game logic";
    m_abort = true;
}

void GameLogic::run() {
    m_white->onSetColor(White);
    m_black->onSetColor(Black);

    assert(m_gameState.getNextPlayer() == PlayerColor::White);

    LOG(info) << "Game start";
    notify([&](AbstractGameObserverPtr& obs) {
        obs->onGameStart(m_gameState, *m_config);
    });

    while (!isGameOver()) {
        auto& currentPlayer = getCurrentPlayer();
        const PlayerColor currentColor = m_gameState.getNextPlayer();

        LOG(debug) << currentColor << "'s turn";

        notify([&](AbstractGameObserverPtr& obs) {
            obs->onTurnStart(currentColor);
        });

        LOG(trace) << "Asking for turn";
        auto futureTurn = currentPlayer->doMakeTurn(m_gameState);

        Turn turn; // Default pass turn

        const seconds maximumTurnTime(m_config->maximumTurnTimeInSeconds);

        if (!wait_for(futureTurn, maximumTurnTime)) {
            if (isGameOver()) {
                // Game aborted
                LOG(info) << "Game aborted";
                currentPlayer->doAbortTurn();
                break;
            }

            LOG(debug) << "Turn timed out";
            notify([&](AbstractGameObserverPtr& obs) {
                obs->onTurnTimeout(currentColor, maximumTurnTime);
            });

            currentPlayer->doAbortTurn();
        }
        else {
            turn = futureTurn.get();
        }

        LOG(trace) << currentColor << "'s turn: " << turn;

        m_gameState.applyTurn(turn);

        notify([&](AbstractGameObserverPtr& obs) {
            obs->onTurnEnd(currentColor, turn, m_gameState);
        });

        LOG(debug) << currentColor << " ended its turn";

        // Add in a delay between turns
        wait(seconds(m_config->timeBetweenTurnsInSeconds));
    }

    LOG(info) << "Game over";
    notify([&](AbstractGameObserverPtr& obs) {
        obs->onGameOver(m_gameState, getWinner());
    });
}

void GameLogic::wait(std::chrono::milliseconds waitInMs) const {
    std::chrono::milliseconds waited(0);
    while (waited < waitInMs && !m_abort) {
        const auto waitLeft = waitInMs - waited;
        const std::chrono::milliseconds iterTime = std::min(waitLeft, m_tickLength);

        this_thread::sleep_for(iterTime);

        waited += iterTime;
    }
}

bool GameLogic::isGameOver() const {
    return false || m_abort;
}

PlayerColor GameLogic::getWinner() const {
    return PlayerColor::NoPlayer;
}

AbstractPlayerPtr& GameLogic::getCurrentPlayer() {
    return (m_gameState.getNextPlayer() == White) ? m_black : m_white;
}

