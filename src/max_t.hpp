#pragma once

#include "traits.hpp"
#include <algorithm>
#include <limits>
#include <type_traits>

namespace algo {
/// @brief A wrapper type that defines a monoid under the `max` operation. The
/// identity element is `std::numeric_limits<T>::min()`.
/// @tparam T A numeric type that supports comparison and default construction
/// (e.g., `int`, `float`).
template <typename T>
  requires std::is_arithmetic_v<T>
struct max_t {
  T x;
  constexpr max_t() : x(std::numeric_limits<T>::min()) {}
  constexpr max_t(const T &x) : x(x) {}
  constexpr max_t operator+(const max_t &other) const {
    return std::max(x, other.x);
  }
  constexpr operator T() const { return x; }
  constexpr bool operator==(const max_t &other) const { return x == other.x; }
  constexpr bool operator!=(const max_t &other) const { return x != other.x; }
};

template <typename T>
struct is_idempotent<max_t<T>> : std::true_type {};
} // namespace algo