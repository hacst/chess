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
