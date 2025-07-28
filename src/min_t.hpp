#pragma once

#include <algorithm>
#include <limits>
#include <type_traits>

namespace algo {
/// @brief A wrapper type that defines a monoid under the `min` operation. The
/// identity element is `std::numeric_limits<T>::max()`.
/// @tparam T A numeric type that supports comparison and default construction
/// (e.g., `int`, `float`).
template <typename T>
  requires std::is_arithmetic_v<T>
struct min_t {
  T x;
  constexpr min_t() : x(std::numeric_limits<T>::max()) {}
  constexpr min_t(const T &x) : x(x) {}
  constexpr min_t operator+(const min_t &other) const {
    return std::min(x, other.x);
  }
  constexpr operator T() const { return x; }
  constexpr bool operator==(const min_t &other) const { return x == other.x; }
  constexpr bool operator!=(const min_t &other) const { return x != other.x; }
};
} // namespace algo