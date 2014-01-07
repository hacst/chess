#ifndef HELPER_H
#define HELPER_H

#include <string>
#include <memory>
#include <type_traits>
#include <ostream>
#include <random>

// As MSVC 2013 isn't fully C++11 compliant alignas cannot be used to
// set data structure alignment. Instead compiler specific approaches
// are wrapped in this PACK macro.
#if defined(__GNUC__) || defined(__clang__)
#define PACK( __Declaration__ ) __Declaration__ __attribute__((__packed__))
#elif defined(_MSC_VER)
#define PACK( __Declaration__ ) __pragma( pack(push, 1) ) __Declaration__ __pragma( pack(pop) )
#endif

/**
 * @brief Selects a random element from a given container.
 * @param container Container to choose from.
 * @param rng Random number generator to use (e.g. a std::mt19937).
 * @return Iterator to the chosen element. end(container) if the container is empty.
 */
template<typename Container, typename Rng>
auto random_selection(const Container& container, Rng& rng) -> decltype(std::begin(container)) {
    auto cur = std::begin(container);
    auto end = std::end(container);

    if (cur == end) {
        return end;
    }
    std::uniform_int_distribution<size_t> distribution(0, std::distance(cur, end) - 1);
    std::advance(cur, distribution(rng));
    return cur;
}

// Not for public consumption. Contains template metaprogramming which for a
// given type T makes has_toString<T>::value statically (during compile time)
// evaluate to true or false depending on whether the given type (e.g. a class)
// has a toString method. Used by the ostream operator<< overload below.
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
// Not for public consumption. Uses has_toString to enable on ostream operator<<
// overload for classes which implement a toString function. For these functions
// using this operator (e.g. cout << myclass) will call the toString function on
// a class to get a stringified version of the class. This is a convinience class
// which makes C++ behave a bit closer to what Java programmers are used to.
template<typename T>
typename std::enable_if<has_toString<T>::value, std::ostream&>::type
operator<<(std::ostream& stream, const T& t) {
    stream << t.toString();
    return stream;
}
}


#endif // HELPER_H
