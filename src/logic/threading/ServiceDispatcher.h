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
