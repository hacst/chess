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
#ifndef HELPER_H
#define HELPER_H

#include <string>
#include <memory>
#include <type_traits>
#include <ostream>
#include <random>

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
