#ifndef SERVICE_DISPATCHER_THREAD_H
#define SERVICE_DISPATCHER_THREAD_H

#include <thread>
#include "ServiceDispatcher.h"

/**
 * @brief Provides functionality for safely running operations in a thread.
 * 
 * Uses ServiceDispatcher to move function calls into its own thread and
 * execute them.
 */
class ServiceDispatcherThread : public ServiceDispatcher {
public:
    /**
     * @brief Creates a ServiceDispatcherThread.
     * @note Don't forget to start() it.
     */
    ServiceDispatcherThread() {}
    
    /**
     * @brief Destroy dispatcher.
     * Stops internal thread and discards all remaining calls.
     */
    virtual ~ServiceDispatcherThread() {
        stop(true);
    }
    
    //! Start the dispatcher thread.
    virtual void start() {
        m_thread = std::thread([this](){
            run();
        });
    }
    
    /**
     * @brief Stops the execution of this tread.
     * @param force If true remaining calls are dropped. Otherwise shutdown
     *              is deferred until all calls currently in the queue are
     *              processed.
     */
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
