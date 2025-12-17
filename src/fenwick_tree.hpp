#pragma once

#include <vector>

namespace algo {
template <typename T, int d>
class fenwick_tree;

/// @brief A Fenwick tree supporting prefix accumulation.
/// @tparam T Any associative and commutative type with an identity element and a defined `operator+`.
template <typename T>
class fenwick_tree<T, 1> {
private:
  std::size_t n;
  std::vector<T> f;

public:
  fenwick_tree(std::size_t n) : n(n), f(n + 1) {}

  /// @brief Applies `operator+` on each internal Fenwick node, essentially setting `a[i]` to `a[i]` + `x`
  /// @param i Index of the element to update.
  /// @param x The value to combine with `a[i]` using the monoid's associative `operator+`.
  void apply(std::size_t i, T x) {
    for (++i; i <= n; i += i & -i) {
      f[i] = f[i] + x;
    }
  }

  /// @brief Computes accumulated prefixes.
  /// @param i The index where the prefix should end.
  /// @return The accumulated prefix from index `0` to index `i`.
  T query(std::size_t i) const {
    T ans = T{};
    for (++i; i > 0; i &= i - 1) {
      ans = ans + f[i];
    }
    return ans;
  }

  /// @brief Computes the accumulated value over the range [l,r]. Only present when
  /// `operator-` is defined, that is, `T` forms an albelian group.
  /// @param l The starting index of the range.
  /// @param r The ending index of the range.
  /// @return The accumulated result over the range [l, r].
  T query(std::size_t l, std::size_t r) const
    requires requires(T a, T b) { a - b; }
  {
    return l == 0 ? query(r) : query(r) - query(l - 1);
  }
};
} // namespace algo