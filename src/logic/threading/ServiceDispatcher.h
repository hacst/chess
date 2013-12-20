#ifndef SERVICE_DISPATCHER_H
#define SERVICE_DISPATCHER_H

#include <future>
#include <boost/asio/io_service.hpp>

/**
 * @brief Provides functionality for safely running operations in a thread
 */
class ServiceDispatcher {
public:
	ServiceDispatcher()
		: m_service()
		, m_work(new boost::asio::io_service::work(m_service)) {}

	virtual ~ServiceDispatcher() {
		// Nothing
	}

	void poll() {
		m_service.poll();
	}

protected:
	template <typename Function>
	void post(Function&& function) {
		m_service.post(function);
	}

	template <typename Function>
	auto postPromise(Function&& f) -> decltype(std::promise<typename std::result_of<Function()>::type>().get_future()){
		auto p(std::make_shared<std::promise<decltype(f())>>());
		auto future = p->get_future();

		m_service.post([f, p]() mutable {
			p->set_value(f());
		});

		return future;
	}
	
	void run() {
		m_service.run();
	}
	
	void resetWork() {
		m_work.reset();
	}
	
	void stopService() {
		m_service.stop();
	}

private:
	
	/// Object providing event loop queuing and dispatching functionality
	boost::asio::io_service m_service;
	/// Work object preventing service run loop from exiting on dry queue
	std::unique_ptr<boost::asio::io_service::work> m_work;
};

#endif // SERVICE_DISPATCHER_H
