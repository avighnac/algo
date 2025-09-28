#pragma once

#include <algorithm>
#include <queue>
#include <type_traits>
#include <vector>

namespace algo {
/// @brief An optimized unweighted graph with a fixed size that's optimized for use in contests.
// This requires the vertex type `T` to be indexable.
/// @tparam T The vertex type.
template <typename T>
  requires std::is_integral_v<T>
class graph {
private:
  int n;
  std::vector<std::vector<int>> adj;

public:
  graph(int n) : n(n), adj(n) {}

  void add_edge(const T &u, const T &v) {
    adj[u].push_back(v);
  }

  struct bfs_result {
    std::vector<int> dist;
    std::vector<T> par;
    bfs_result(int n) : dist(n), par(n) {}
  };

  /// @brief Performs a breadth-first search from the given starting node; computes the shortest
  /// distance (in number of edges) from the start node to all reachable nodes
  /// in the graph.
  /// @param start The starting node for the BFS traversal.
  /// @return A struct `bfs_result` object containing two `std::vector`s
  bfs_result bfs(const T &start) const {
    std::queue<T> q;
    std::vector<bool> vis(n);
    bfs_result ans(n);
    ans.dist.assign(n, std::numeric_limits<int>::max());
    ans.par.assign(n, -1);
    q.push(start);
    ans.dist[start] = 0, ans.par[start] = start;
    while (!q.empty()) {
      T u = q.front();
      q.pop();
      for (const T &v : adj[u]) {
        if (!vis[v]) {
          vis[v] = true;
          ans.par[v] = u, ans.dist[v] = ans.dist[u] + 1;
          q.push(v);
        }
      }
    }
    return ans;
  }

  /// @brief Computes the shortest path between two nodes.
  ///
  /// This function internally performs a breadth-first search starting from
  /// the `from` node, then reconstructs the shortest path to the `to` node.
  /// If the target node is unreachable, the returned vector will be empty.
  ///
  /// @param from The starting node of the path.
  /// @param to The destination node of the path.
  /// @return A `std::vector<T>` containing the nodes along the shortest path from
  ///         `from` to `to`, including both endpoints. If no path exists, returns an empty vector.
  std::vector<T> path(const T &from, const T &to) const {
    auto [dist, par] = bfs(from);
    std::vector<T> ans;
    if (par[to] == -1) {
      return ans;
    }
    for (T u = to; u != from; u = par[u]) {
      ans.push_back(u);
    }
    ans.push_back(from);
    std::reverse(ans.begin(), ans.end());
    return ans;
  }
};
} // namespace algo