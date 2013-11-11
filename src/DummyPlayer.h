#ifndef DUMMYPLAYER_H
#define DUMMYPLAYER_H

#include "AbstractPlayer.h"
#include "ServiceDispatcherThread.h"
#include "helper.h"

#include <random>
#include <thread>
#include <chrono>
#include <iostream>

class DummyPlayer : public AbstractPlayer, public ServiceDispatcherThread {
public:
	DummyPlayer()
		: m_rng()
		, m_msDist(0, 7) {}

	virtual ~DummyPlayer() {
		stop(true);
	}

	virtual std::future<Turn> doMakeTurn(State state) override {
		// Always pass after a random amount of time
		// Obviously can't be aborted
		return postPromise([=]() {
			int duration = m_msDist(m_rng);
			std::cout << "Dummy will sleep for " << duration << " seconds" << std::endl;
			std::this_thread::sleep_for(std::chrono::seconds(duration));
			
			return Turn();
		});
	}
private:
	std::mt19937 m_rng;
	std::uniform_int_distribution<int> m_msDist;
};

using DummyPlayerPtr = std::shared_ptr<DummyPlayer>;

#endif // DUMMYPLAYER_H
