#ifndef GAMELOGIC_H
#define GAMELOGIC_H

#include <vector>
#include <chrono>
#include <future>

#include "GameState.h"
#include "core/GameConfiguration.h"
#include "logic/interface/AbstractGameLogic.h"
#include "misc/helper.h"

class GameLogic : public AbstractGameLogic {
public:
    GameLogic(AbstractPlayerPtr white, AbstractPlayerPtr black, GameConfigurationPtr config);
    ~GameLogic();

    virtual AbstractPlayerPtr getWhite() const override;
    virtual AbstractPlayerPtr getBlack() const override;

    virtual void addObserver(AbstractGameObserverPtr observer) override;

    virtual bool isGameOver() const override;
    virtual PlayerColor getWinner() const override;
    virtual GameConfigurationPtr getConfiguration() const override;

    virtual void stop() override;

private:
    /// Contains actual functionality of GameLogic
    virtual void run() override;

    /// Wraps std::future::wait_for so the wait can be aborted every m_tickLength time.
    template <typename Future>
    bool wait_for(Future&& fut, std::chrono::milliseconds waitInMs) {
        std::chrono::milliseconds waited(0);
        while (waited < waitInMs && !m_abort) {
            const auto waitLeft = waitInMs - waited;
            const std::chrono::milliseconds iterTime = std::min(waitLeft, m_tickLength);

            if (fut.wait_for(iterTime) == std::future_status::ready) {
                return true;
            }

            waited += iterTime;
        }
        // Future couldn't provide result till timeout
        return false;
    }

    /**
     * @brief Abortable wait
     */
    void wait(std::chrono::milliseconds waitInMs) const;

    /// Function to call a given function on all attached observers.
    ///
    /// Usage:	notify([&](AbstractGameObserverPtr& obs) {
    ///			    obs->someObserverFunction(parameters);
    ///			});
    ///
    template <typename Function>
    void notify(Function&& f) {
        for (auto& observer : m_observers) {
            f(observer);
        }
    }

    AbstractPlayerPtr& getCurrentPlayer();

    // Interval in which the GameLogic should check for
    std::chrono::milliseconds m_tickLength;
    // If true the running game is aborted
    bool m_abort;

    std::vector<AbstractGameObserverPtr> m_observers;
    AbstractPlayerPtr m_white;
    AbstractPlayerPtr m_black;
    GameState m_gameState;
    GameConfigurationPtr m_config;
};

#endif // GAMELOGIC_H
