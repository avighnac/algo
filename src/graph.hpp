#pragma once

#include "traits.hpp"
#include <functional>
#include <map>
#include <queue>
#include <set>
#include <type_traits>
#include <vector>

namespace algo {
template <typename T>
constexpr bool is_indexable_v = std::is_convertible_v<T, int>;

/// @brief A generic graph data structure supporting both numeric and custom
/// node types, with automatic storage optimization.
/// @tparam Tv The vertex type. If convertible to `int`, a vector is used.
/// If marked dense-serializable with a serializer, `algo::serialized_array` is
/// used. Otherwise, falls back to `std::map`.
/// @tparam Tw The edge weight type (e.g., `int`, `float`, `double`).
template <typename Tv, typename Tw> class graph {
private:
  std::optional<std::function<int(const Tv &)>> serializer;

  template <typename T>
  using indexable_set = std::conditional_t<
      is_indexable_v<T>, std::vector<char>,
      std::conditional_t<
          is_serializable_v<T>,
          serialized_array<T, char, std::function<int(const T &)>>,
          std::set<T>>>;

  template <typename K, typename V>
  using indexable_map = std::conditional_t<
      is_indexable_v<K>, std::vector<V>,
      std::conditional_t<is_serializable_v<K>,
                         serialized_array<K, V, std::function<int(const K &)>>,
                         std::map<K, V>>>;

  template <typename K, typename V>
  static auto make_map(std::optional<std::function<int(const K &)>> serializer,
                       std::size_t max) {
    if constexpr (is_indexable_v<K>) {
      return std::vector<V>(max + 1);
    } else if constexpr (is_serializable_v<K>) {
      return serialized_array<K, V, std::function<int(const K &)>>(max + 1,
                                                                   *serializer);
    } else {
      return std::map<K, V>();
    }
  }

  template <typename K>
  static auto make_set(std::optional<std::function<int(const K &)>> serializer,
                       std::size_t max) {
    if constexpr (is_indexable_v<K>) {
      return std::vector<char>(max + 1);
    } else if constexpr (is_serializable_v<K>) {
      return serialized_array<K, char, std::function<int(const K &)>>(
          max + 1, *serializer);
    } else {
      return std::set<K>();
    }
  }

  int max = -1;

  struct edge {
    Tv u, v;
    Tw w;
  };
  class adj_t {
  private:
    indexable_map<Tv, std::vector<edge>> data;

  public:
    adj_t() = default;
    adj_t(std::size_t n, std::function<int(const Tv &)> s)
      requires is_serializable_v<Tv>
        : data(n, std::move(s)) {}

    void add(const edge &e) { data[e.u].push_back(e); }

    const std::vector<edge> &operator[](const Tv &u) const {
      static const std::vector<edge> empty;
      if constexpr (is_indexable_v<Tv>) {
        return u < data.size() ? data[u] : empty;
      } else if constexpr (is_serializable_v<Tv>) {
        return data.at(u);
      } else {
        auto it = data.find(u);
        return it != data.end() ? it->second : empty;
      }
    }

    void resize_if_indexable(std::size_t n) {
      if constexpr (is_indexable_v<Tv>) {
        data.resize(n);
      }
    }

    void clear() { data.clear(); }
  };

public:
  std::vector<edge> edges;
  adj_t adj;

  graph() {
    if constexpr (is_serializable_v<Tv>) {
      throw std::logic_error(
          "Must provide a serializer for dense-serializable type");
    }
  }
  template <typename T = Tv, typename = std::enable_if_t<is_indexable_v<T>>>
  graph(std::size_t n) {
    if constexpr (is_serializable_v<T>) {
      throw std::logic_error(
          "Use the serializer constructor for dense-serializable type");
    }
    max = n - 1;
    adj.resize_if_indexable(n);
  }
  graph(std::size_t n, std::function<int(const Tv &)> s)
    requires is_serializable_v<Tv>
      : serializer(std::move(s)), adj(n, *serializer) {
    max = n - 1;
  }

  void add_edge(const Tv &u, const Tv &v, const Tw &w) {
    if constexpr (is_indexable_v<Tv>) {
      max = std::max({max, u, v});
    } else if constexpr (is_serializable_v<Tv>) {
      max = std::max({max, serializer ? (*serializer)(u) : -1,
                      serializer ? (*serializer)(v) : -1});
    }
    adj.add({u, v, w});
  }

  template <typename T = Tw,
            typename = std::enable_if_t<std::is_arithmetic_v<T>>>
  void add_edge(const Tv &u, const Tv &v) {
    add_edge(u, v, 1);
  }

  void clear() { adj.clear(); }

  struct bfs_result {
    indexable_map<Tv, Tw> dist;
    indexable_map<Tv, Tv> par;

    bfs_result(std::optional<std::function<int(const Tv &)>> serializer,
               std::size_t max)
        : dist(make_map<Tv, Tw>(serializer, max)),
          par(make_map<Tv, Tv>(serializer, max)) {}
  };

  /// @brief Performs a breadth-first search from the given starting node,
  /// assuming all edges have an implicit weight of 1; computes the shortest
  /// distance (in number of edges) from the start node to all reachable nodes
  /// in the graph.
  ///
  /// If the vertex type `Tv` is indexable (i.e., convertible to `int`), the
  /// returned distance map will be an `std::vector<Tw>`, where the index
  /// corresponds to the vertex. Otherwise, it will be an `std::map<Tv, Tw>`.
  ///
  /// @param start The starting node for the BFS traversal.
  /// @return A container mapping each reachable node to its distance from the
  /// start node (with -1 indicating unreachability). The type is
  /// `std::vector<Tw>` if `Tv` is indexable, or `std::map<Tv, Tw>` otherwise.
  bfs_result bfs(const Tv &start) const {
    std::queue<Tv> q;
    indexable_set<Tv> visited = make_set<Tv>(serializer, max);
    bfs_result ans(serializer, max);

    if constexpr (is_indexable_v<Tv>) {
      visited.resize(max + 1);
      ans.dist.resize(max + 1, invalid_value<Tw>());
      ans.par.resize(max + 1, invalid_value<Tv>());
    }
    if constexpr (is_serializable_v<Tv>) {
      ans.dist.fill(invalid_value<Tw>());
      ans.par.fill(invalid_value<Tv>());
    }

    auto mark_visited = [&](const Tv &v) {
      if constexpr (is_indexable_v<Tv> || is_serializable_v<Tv>) {
        visited[v] = 1;
      } else {
        visited.insert(v);
      }
    };

    auto is_visited = [&](const Tv &v) -> bool {
      if constexpr (is_indexable_v<Tv>) {
        return v < visited.size() && visited[v];
      } else if constexpr (is_serializable_v<Tv>) {
        return visited[v];
      } else {
        return visited.contains(v);
      }
    };

    q.push(start);
    mark_visited(start);
    ans.dist[start] = 0, ans.par[start] = start;
    while (!q.empty()) {
      Tv u = q.front();
      q.pop();
      for (const edge &e : adj[u]) {
        Tv v = e.v;
        if (!is_visited(v)) {
          mark_visited(v);
          ans.par[v] = u, ans.dist[v] = ans.dist[u] + 1;
          q.push(v);
        }
      }
    }
    return ans;
  }
};
} // namespace algo