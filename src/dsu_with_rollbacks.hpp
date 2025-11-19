#pragma once

#include <numeric>
#include <type_traits>
#include <vector>

namespace algo {
template <typename T>
  requires std::is_integral_v<T>
class dsu_with_rollbacks {
private:
  int n;
  std::vector<int> par, m_size;
  std::vector<std::pair<int, int>> rollbacks;

public:
  dsu_with_rollbacks(int n) : n(n), par(n), m_size(n, 1) {
    std::iota(par.begin(), par.end(), 0);
  }

  int root(T u) const { return u == par[u] ? u : root(par[u]); }

  bool merge(T u, T v) {
    u = root(u), v = root(v);
    if (u == v) {
      return false;
    }
    if (m_size[u] < m_size[v]) {
      std::swap(u, v);
    }
    rollbacks.push_back({v, par[v]});
    rollbacks.push_back({u, m_size[u]});
    par[v] = u;
    m_size[u] += m_size[v];
    return true;
  }

  void undo() {
    auto [u, size] = rollbacks.back();
    rollbacks.pop_back();
    m_size[u] = size;
    auto [v, p] = rollbacks.back();
    rollbacks.pop_back();
    par[v] = p;
  }
};
}; // namespace algo