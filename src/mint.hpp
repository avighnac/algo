#pragma once

#include <cstdint>
#include <ostream>

namespace algo {
template <int64_t mod = 1000000007>
class mint {
private:
  int64_t x;

public:
  mint(const int64_t &x) : x((x % mod + mod) % mod) {}
  mint() : x(0) {}

  mint pow(int64_t b) const {
    mint ans = 1, p = x;
    while (b > 0) {
      if (b & 1) {
        ans *= p;
      }
      p *= p, b >>= 1;
    }
    return ans;
  }

  /// @brief Computes the modular multiplicative inverse of this value.
  /// @note Requires `mod` to be prime.
  mint inv() const { return pow(mod - 2); }

  mint &operator+=(const mint &other) { return x = (x + other.x) % mod, *this; }
  mint &operator-=(const mint &other) { return x = (x - other.x + mod) % mod, *this; }
  mint &operator*=(const mint &other) { return x = (x * other.x) % mod, *this; }
  mint &operator/=(const mint &other) { return *this *= other.inv(); }

  mint operator+(const mint &other) const { return mint{*this} += other; }
  mint operator-(const mint &other) const { return mint{*this} -= other; }
  mint operator*(const mint &other) const { return mint{*this} *= other; }
  mint operator/(const mint &other) const { return mint{*this} /= other; }

  friend std::ostream &operator<<(std::ostream &os, const mint &x) {
    return os << x.x;
  }

  bool operator==(const mint &other) const { return x == other.x; }
  bool operator!=(const mint &other) const { return !(*this == other); }

  operator int64_t() const { return x; }
};
} // namespace algo