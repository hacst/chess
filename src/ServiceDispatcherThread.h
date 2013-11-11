#ifndef SERVICE_DISPATCHER_THREAD_H
#define SERVICE_DISPATCHER_THREAD_H

#include <thread>
#include "ServiceDispatcher.h"

/**
 * @brief Provides functionality for safely running operations in a thread
 */
class ServiceDispatcherThread : public ServiceDispatcher {
public:
	ServiceDispatcherThread() {}

	virtual ~ServiceDispatcherThread() {
		stop(true);
	}

	virtual void start() {
		m_thread = std::thread([this](){
			run();
		});
	}

	virtual void stop(bool force = false) {
		if (!m_thread.joinable()) {
			// We already stopped or never started
			return;
		}

		if (force) {
			// Drop queued posts to exit asap
			stopService();
		} else {
			// Reset on work wouldn't be threadsafe so defer it
			post([this]() { resetWork(); });
		}
		m_thread.join();
	}

	/// Thread this object is running its event loop on after start
	std::thread m_thread;
};

#endif // SERVICE_DISPATCHER_THREAD_H
