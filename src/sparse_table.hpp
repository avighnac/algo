#pragma once

#include "traits.hpp"
#include <bit>
#include <vector>

namespace algo {
template <typename T>
  requires is_idempotent_v<T>
class sparse_table {
private:
  std::size_t n;
  int w;
  std::vector<std::vector<T>> bin;

public:
  template <typename M>
  sparse_table(const std::vector<M> &vals) : n(vals.size()), w(std::bit_width(n)), bin(n, std::vector<T>(w)) {
    for (int i = 0; i < n; ++i) {
      bin[i][0] = vals[i];
    }
    for (int bt = 0; bt < w - 1; ++bt) {
      for (int i = 0; i + (1 << bt) < n; ++i) {
        bin[i][bt + 1] = bin[i][bt] + bin[i + (1 << bt)][bt];
      }
    }
  }

  T query(std::size_t l, std::size_t r) const {
    if (l == r) {
      return bin[l][0];
    }
    int k = std::bit_width(r - l + 1) - 1;
    return bin[l][k] + bin[r - (1 << k) + 1][k];
  }
};
} // namespace algo