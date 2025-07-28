#pragma once

#include <type_traits>

namespace algo {
template <typename T>
  requires std::is_integral_v<T>
struct xor_t {
  T x;
  constexpr xor_t() : x(0) {}
  constexpr xor_t(const T &x) : x(x) {}
  constexpr xor_t operator+(const xor_t &other) const {
    return x ^ other.x;
  }
  constexpr xor_t operator-(const xor_t &other) const {
    return x ^ other.x;
  }
  constexpr operator T() const { return x; }
  constexpr bool operator==(const xor_t &other) const { return x == other.x; }
  constexpr bool operator!=(const xor_t &other) const { return x != other.x; }
};
} // namespace algo