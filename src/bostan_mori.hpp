#pragma once

#include "convolute.hpp"
#include "mint.hpp"

namespace algo {
/// @brief Computes [x^m] p(x) / q(x), assuming q(0) != 0 and deg(p) < deg(q).
/// @return The coefficient [x^m] p(x) / q(x).
template <int64_t mod>
mint<mod> bostan_mori(std::vector<mint<mod>> p, std::vector<mint<mod>> q, int64_t m) {
  while (m > 0) {
    std::vector<mint<mod>> minus_q = q;
    for (int i = 1; i < minus_q.size(); i += 2) {
      minus_q[i] = -minus_q[i];
    }
    std::vector<mint<mod>> F = convolute(p, minus_q);
    std::vector<mint<mod>> G = convolute(q, minus_q);
    q.resize((G.size() + 1) / 2);
    for (int i = 0; i < (G.size() + 1) / 2; ++i) {
      q[i] = G[2 * i];
    }
    if (m & 1) {
      p.resize(F.size() / 2);
      for (int i = 0; i < F.size() / 2; ++i) {
        p[i] = F[2 * i + 1];
      }
    } else {
      p.resize((F.size() + 1) / 2);
      for (int i = 0; i < (F.size() + 1) / 2; ++i) {
        p[i] = F[2 * i];
      }
    }
    m >>= 1;
  }
  return p[0] / q[0];
}
} // namespace algo