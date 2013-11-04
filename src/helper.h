#ifndef HELPER_H
#define HELPER_H

#include <thread>
#include <memory>
#include <future>
#include <type_traits>
#include <ostream>
#include <boost/asio/io_service.hpp>

/**
 * @brief Provides functionality for safely running operations in a thread
 */
class ServiceDispatcherThread {
public:
	ServiceDispatcherThread()
		: m_service()
		, m_work(new boost::asio::io_service::work(m_service)) {}

	virtual ~ServiceDispatcherThread() {
		stop(true);
	}

	virtual void start() {
		m_thread = std::thread([this](){
			m_service.run();
		});
	}

	virtual void stop(bool force = false) {
		if (!m_thread.joinable()) {
			// We already stopped or never started
			return;
		}

		if (force) {
			// Drop queued posts to exit asap
			m_service.stop();
		} else {
			// Reset on work wouldn't be threadsafe so defer it
			m_service.post([this]() { deferredStopOp(); });
		}
		m_thread.join();
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

		m_service.post([f, p](){
			p->set_value(f());
		});

		return future;
	}

private:

	void deferredStopOp() {
		m_work.reset();
	}

	/// Thread this object is running its event loop on after start
	std::thread m_thread;
	/// Object providing event loop queuing and dispatching functionality
	boost::asio::io_service m_service;
	/// Work object preventing service run loop from exiting on dry queue
	std::unique_ptr<boost::asio::io_service::work> m_work;
};

template<typename T>
struct has_toString
{
private:
	typedef std::true_type yes;
	typedef std::false_type no;

	template<typename U> static auto test(int) -> decltype(std::declval<U>().toString() == "", yes());
	template<typename> static no test(...);

public:

	enum { value = std::is_same<decltype(test<T>(0)), yes>::value };
};

namespace std {
template<typename T>
typename std::enable_if<has_toString<T>::value, std::ostream&>::type
operator<<(std::ostream& stream, const T& t) {
	stream << t.toString();
	return stream;
}
}


#endif // HELPER_H
