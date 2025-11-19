#pragma once

#include "traits.hpp"
#include <numeric>

namespace algo {
template <typename T>
  requires std::is_arithmetic_v<T>
struct gcd_t {
  T x;
  constexpr gcd_t() : x(0) {}
  constexpr gcd_t(const T &x) : x(x) {}
  constexpr gcd_t operator+(const gcd_t &other) const {
    return std::gcd(x, other.x);
  }
  constexpr operator T() const { return x; }
  constexpr bool operator==(const gcd_t &other) const { return x == other.x; }
  constexpr bool operator!=(const gcd_t &other) const { return x != other.x; }
};

template <typename T>
struct is_idempotent<gcd_t<T>> : std::true_type {};
}; // namespace algo