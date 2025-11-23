#pragma once

#include "fenwick_tree.hpp"

namespace algo {
template <typename T, int d>
class fenwick_tree {
private:
  std::size_t n;
  std::vector<fenwick_tree<T, d - 1>> f;

public:
  template <typename... ints>
  fenwick_tree(std::size_t n, ints... dims) : n(n), f(n + 1, fenwick_tree<T, d - 1>{dims...}) {}

  template <typename... ints>
  void apply(std::size_t i, ints... next) {
    for (++i; i <= n; i += i & -i) {
      f[i].apply(next...);
    }
  }

  template <typename... ints>
    requires(sizeof...(ints) == d - 1)
  T query(std::size_t i, ints... dims) const {
    T ans = T{};
    for (++i; i > 0; i -= i & -i) {
      ans = ans + f[i].query(dims...);
    }
    return ans;
  }

  template <typename... ints>
  T query(std::size_t l, std::size_t r, ints... dims) const
    requires requires(T a, T b) { a - b; }
  {
    T ans = T{};
    for (++r; r > 0; r -= r & -r) {
      ans = ans + f[r].query(dims...);
    }
    for (; l > 0; l -= l & -l) {
      ans = ans - f[l].query(dims...);
    }
    return ans;
  }
};
}; // namespace algo