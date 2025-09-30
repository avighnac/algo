#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace algo {
/// @brief Combinatorial utilities for types that support addition, subtraction, multiplication, and division.
/// @tparam T Type that defines +, -, *, and / (closed under division).
/// @tparam size Maximum value for computations.
template <typename T, std::size_t size = 200000>
class combi {
private:
  std::array<T, size> fact, inv_fact;

public:
  combi() {
    fact[0] = 1;
    for (int64_t i = 1; i < size; ++i) {
      fact[i] = fact[i - 1] * T{i};
    }
    inv_fact[size - 1] = T{1} / fact[size - 1];
    for (int64_t i = size - 1; i > 0; --i) {
      inv_fact[i - 1] = inv_fact[i] * T{i};
    }
  }

  /// @brief Returns `n!` for the given input.
  /// @param n Non-negative integer.
  /// @return The factorial of `n`.
  const T &factorial(const int &n) const { return fact[n]; }

  T perm(const int &n, const int &k) const {
    return fact[n] * inv_fact[n - k];
  }

  T choose(const int &n, const int &k) const {
    return fact[n] * inv_fact[k] * inv_fact[n - k];
  }

  T catalan(const int &n) const { return choose(2 * n, n) / (n + 1); }
};
} // namespace algo