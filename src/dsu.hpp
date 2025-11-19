#pragma once

#include <numeric>
#include <type_traits>
#include <variant>
#include <vector>

namespace algo {
/// @brief A disjoint set union (`dsu`) data structure.
/// @tparam T Element type.
/// @tparam Serializer An optional serializer mapping `T` bijectively to `int`.
template <typename T, typename Serializer = void>
class dsu {
private:
  int n;
  std::vector<int> par, m_size;

  using serializer_t = std::conditional_t<std::is_void_v<Serializer>, std::monostate, Serializer>;
  [[no_unique_address]] serializer_t serializer;

  int _root(int u) { return u == par[u] ? u : par[u] = _root(par[u]); }

public:
  dsu(int n, serializer_t s = {}) : n(n), par(n), m_size(n, 1), serializer(std::move(s)) {
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
    int _u, _v;
    if constexpr (std::is_void_v<Serializer>) {
      _u = _root(u);
      _v = _root(v);
    } else {
      _u = _root(serializer(u));
      _v = _root(serializer(v));
    }
    if (_u == _v) {
      return false;
    }
    par[_v] = _u;
    m_size[_u] += m_size[_v];
    return true;
  }

  int size(T u) {
    int _u;
    if constexpr (std::is_void_v<Serializer>) {
      _u = _root(u);
    } else {
      _u = _root(serializer(u));
    }
    return m_size[_u];
  }
};
} // namespace algo