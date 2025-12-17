#pragma once

#include <functional>
#include <limits>

namespace algo {

template <typename T, typename F, typename = void>
struct monoid_identity;

// a + b
template <typename T>
struct monoid_identity<T, std::plus<>, void> {
  T operator()() const { return T{}; }
};

// min(a, b)
template <typename T>
struct monoid_identity<
    T,
    std::less<>,
    std::enable_if_t<std::numeric_limits<T>::is_specialized>> {
  T operator()() const { return std::numeric_limits<T>::max(); }
};

// max(a, b)
template <typename T>
struct monoid_identity<
    T,
    std::greater<>,
    std::enable_if_t<std::numeric_limits<T>::is_specialized>> {
  T operator()() const { return std::numeric_limits<T>::min(); }
};
} // namespace algo