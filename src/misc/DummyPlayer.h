#ifndef DUMMYPLAYER_H
#define DUMMYPLAYER_H

#include "logic/interface/AbstractPlayer.h"
#include "logic/threading/ServiceDispatcherThread.h"
#include "misc/helper.h"
#include "core/Logging.h"

#include <random>
#include <thread>
#include <chrono>
#include <iostream>

/**
 * @brief Player implementation which takes random turns after
 *        random amounts of time.
 * @warning Does not react to doAbortTurn events.
 */
class DummyPlayer : public AbstractPlayer, public ServiceDispatcherThread {
public:
    DummyPlayer()
        : m_rng()
        , m_msDist(0, 5)
        , m_log(Logging::initLogger("DummyPlayer")) {}
    
    virtual ~DummyPlayer() {
        stop(true);
    }
    
    virtual void onSetColor(PlayerColor color) override {
        LOG(Logging::info) << "Will be playing " << color;
    }
    
    virtual void doAbortTurn() override {
        LOG(Logging::warning) << "Asked to abort turn. This is not implemented";
    }
    
    virtual std::future<Turn> doMakeTurn(GameState state) override {
        return postPromise([=]() mutable {
            // Always choose a random turn after a random amount of time
            // Obviously can't be aborted
            auto turns = state.getTurnList();
            auto turnIt = random_selection(turns, m_rng);
            Turn turn = (turnIt == std::end(turns)) ? Turn() : (*turnIt);
            int duration = m_msDist(m_rng);

            LOG(Logging::debug) << "Will select " << turn << " after " << duration << " seconds sleep";
            std::this_thread::sleep_for(std::chrono::seconds(duration));
            return turn;
        });
    }
private:
    std::mt19937 m_rng;
    std::uniform_int_distribution<int> m_msDist;
    Logging::Logger m_log;
};

using DummyPlayerPtr = std::shared_ptr<DummyPlayer>;

#endif // DUMMYPLAYER_H
