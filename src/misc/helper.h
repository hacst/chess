#ifndef HELPER_H
#define HELPER_H

#include <string>
#include <memory>
#include <type_traits>
#include <ostream>

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
