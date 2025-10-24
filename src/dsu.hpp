#pragma once

#include <numeric>
#include <vector>

namespace algo {
/// @brief A disjoint set union (`dsu`) data structure.
/// @tparam T Element type.
/// @tparam Serializer An optional serializer mapping `T` bijectively to `int`.
template <typename T, typename Serializer = void>
class dsu {
private:
  int n;
  std::vector<int> par;
  [[no_unique_address]] Serializer serializer;

  int _root(int u) { return u == par[u] ? u : par[u] = _root(par[u]); }

public:
  dsu(int n, Serializer s = {}) : n(n), par(n), serializer(std::move(s)) {
    std::iota(par.begin(), par.end(), 0);
  }

  /// @brief Returns the root of the component `u` is part of.
  T root(T u) {
    if constexpr (std::is_void_v<Serializer>) {
      return _root(u);
    }
    return serializer(_root(serializer(u)));
  }

  /// @brief Merges the components `u` and `v` are part of.
  /// @return Returns `true` if a merge occured and `false` if `u` and `v` were already part
  /// of the same component.
  bool merge(T u, T v) {
    int _u = _root(serializer(u)), _v = _root(serializer(v));
    if (_u == _v) {
      return false;
    }
    par[_v] = _u;
    return true;
  }
};
} // namespace algo