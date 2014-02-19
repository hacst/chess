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
#ifndef SERVICE_DISPATCHER_H
#define SERVICE_DISPATCHER_H

#include <future>
#include <boost/asio/io_service.hpp>

/**
 * @brief Provides functionality for safely running operations in a thread
 * 
 * For components running on different threads it is essential to safely transport
 * events between them. Without any additional precautions functions will execute
 * on the thread they are called.
 * 
 * This dispatcher can store functions in a thread-safe way and replay them once
 * its poll method is called in the customers thread.
 */
class ServiceDispatcher {
public:
    ServiceDispatcher()
        : m_service()
        , m_work(new boost::asio::io_service::work(m_service)) { /* Nothing */ }
    
    virtual ~ServiceDispatcher() { /* Nothing */ }
    
    /**
     * @brief Replays all posted functions in the calling thread.
     * @see post
     * @see postPromise
     */
    void poll() {
        m_service.poll();
    }
    
protected:
    /**
     * @brief Store a given function.
     * @param function Function to store and later replay.
     */
    template <typename Function>
    void post(Function&& function) {
        m_service.post(function);
    }
    
    /**
     * @brief Stores a given function and returns a future on its return value.
     * @param function Function to store and later replay.
     * @return Future on result of given function.
     */
    template <typename Function>
    auto postPromise(Function&& function) -> decltype(std::promise<typename std::result_of<Function()>::type>().get_future()){
        auto promise(std::make_shared<std::promise<decltype(function())>>());
        auto future = promise->get_future();
        
        m_service.post([function, promise]() mutable {
            promise->set_value(function());
        });
        
        return future;
    }
    
    /** 
     * @brief Runs underlying boost asio io_service
     * @note Will block until work is completed.
     */
    void run() {
        m_service.run();
    }
    
    //! Drops queued functions
    void resetWork() {
        m_work.reset();
    }
    
    //! Stops underlying service.
    void stopService() {
        m_service.stop();
    }
    
private:
    
    //! Object providing event loop queuing and dispatching functionality
    boost::asio::io_service m_service;
    //! Work object preventing service run loop from exiting on dry queue
    std::unique_ptr<boost::asio::io_service::work> m_work;
};

#endif // SERVICE_DISPATCHER_H
