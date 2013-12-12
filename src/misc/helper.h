#ifndef HELPER_H
#define HELPER_H

#include <memory>
#include <type_traits>
#include <ostream>
#include <boost/log/trivial.hpp>

#define LOG	BOOST_LOG_TRIVIAL

template<typename T>
struct has_toString
{
private:
	typedef std::true_type yes;
	typedef std::false_type no;

	template<typename U> static auto test(int) -> decltype((void)(std::declval<U>().toString() == ""), yes());
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
