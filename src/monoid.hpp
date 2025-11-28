#pragma once

#include <functional>

namespace algo {
template <typename T, typename f, typename = void>
struct monoid_identity;

template <typename T> // a + b
struct monoid_identity<T, std::plus<>, void> {
  static constexpr T x = T{};
};

template <typename T> // min(a, b)
struct monoid_identity<
    T,
    std::less<>,
    std::enable_if_t<std::numeric_limits<T>::is_specialized>> {
  static constexpr T x = std::numeric_limits<T>::max();
};

template <typename T> // max(a, b) 
struct monoid_identity<
    T,
    std::greater<>,
    std::enable_if_t<std::numeric_limits<T>::is_specialized>> {
  static constexpr T x = std::numeric_limits<T>::min();
};
} // namespace algo