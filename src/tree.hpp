#pragma once

#include <algorithm>
#include <functional>
#include <type_traits>
#include <vector>

namespace algo {
template <typename T>
  requires std::is_integral_v<T>
class tree {
private:
  int n;
  std::vector<std::vector<int>> adj;
  T valid_node;

public:
  tree(int n) : n(n), adj(n) {}

  std::size_t size() const { return n; }

  void add_edge(const T &u, const T &v) {
    valid_node = u;
    adj[u].push_back(v);
    adj[v].push_back(u);
  }

  template <typename enter, typename child_before, typename child_after, typename exit>
  void dfs(enter &&on_enter, child_before &&on_before, child_after &&on_after, exit &&on_exit, const T &start) const {
    auto dfs = [&](auto &&self, const T &u, const T &p) -> void {
      if constexpr (!std::is_same_v<enter, std::nullptr_t>) {
        on_enter(u, p);
      }
      for (const T &i : adj[u]) {
        if (i == p) {
          continue;
        }
        if constexpr (!std::is_same_v<child_before, std::nullptr_t>) {
          on_before(u, i);
        }
        self(self, i, u);
        if constexpr (!std::is_same_v<child_after, std::nullptr_t>) {
          on_after(u, i);
        }
      }
      if constexpr (!std::is_same_v<exit, std::nullptr_t>) {
        on_exit(u, p);
      }
    };
    dfs(dfs, start, -1);
  }

  /// @brief Depth-first search with the `child_after` hook.
  /// @tparam child
  /// @param child_after
  /// @param start
  template <typename child>
  void dfs(child &&child_after, const T &start) const {
    dfs(nullptr, nullptr, child_after, nullptr, start);
  }

  /// @brief Computes the parent of each node when rooting the tree at @p root.
  ///
  /// @param root The root of the tree.
  /// @return A vector where `par[u]` is the parent of node `u`. The root’s parent is itself.
  std::vector<T> par(const T &root) const {
    std::vector<T> ans(n);
    ans[root] = root;
    dfs([&](const T &u, const T &i) { ans[i] = u; }, root);
    return ans;
  }

  /// @brief Computes the depth of each node from a given root.
  ///
  /// @param root The root of the tree.
  /// @return A vector where `depth[u]` is the distance in edges from @p root to `u`. The depth of the root is 0.
  std::vector<T> dep(const T &root) const {
    std::vector<T> ans(n);
    dfs(nullptr, [&](const T &u, const T &i) { ans[i] = ans[u] + 1; }, nullptr, nullptr, root);
    return ans;
  }

  /// @brief Computes the diameter of the tree by calling `depth()` twice.
  /// @return The length, in edges, of the tree's diameter.
  int diameter() const {
    auto d1 = depth(valid_node);
    auto d2 = depth(std::max_element(d1.begin(), d1.end()) - d1.begin());
    return *std::max_element(d2.begin(), d2.end());
  }

  template <int binary_lift_size>
  class lift_view {
  private:
    const tree<T> &g;
    std::vector<std::array<int, binary_lift_size>> lift;
    std::vector<int> depth;

  public:
    lift_view(const tree<T> &g, const T &root) : g(g), depth(g.size()), lift(g.size()) {
      lift[root][0] = root;
      g.dfs(nullptr, [&](const T &u, const T &i) { depth[i] = depth[u] + 1, lift[i][0] = u; }, nullptr, nullptr, root);
      for (int bt = 1; bt < binary_lift_size; ++bt) {
        for (int i = 0; i < g.n; ++i) {
          lift[i][bt] = lift[lift[i][bt - 1]][bt - 1];
        }
      }
    }

    std::size_t size() const { return g.size(); }

    /// @brief Finds the `k`-th ancestor of `u`.
    /// @param u The node to find the ancestor of.
    /// @param k Which ancestor of `u` should be found.
    /// @return The `k`-th ancestor of `u`.
    T ancestor(T u, int k) const {
      for (int bt = 0; bt < binary_lift_size; ++bt) {
        if (k & 1 << bt) {
          u = lift[u][bt];
        }
      }
      return u;
    }

    /// @brief Finds the lowest common ancestor of nodes `u` and `v`.
    /// @param u The first node.
    /// @param v The second node.
    /// @return The lowest common ancestor of `u` and `v`.
    T lca(T u, T v) const {
      if (depth[u] < depth[v]) {
        std::swap(u, v);
      }
      if ((u = ancestor(u, depth[u] - depth[v])) == v) {
        return u;
      }
      for (int bt = binary_lift_size - 1; bt >= 0; --bt) {
        if (lift[u][bt] != lift[v][bt]) {
          u = lift[u][bt], v = lift[v][bt];
        }
      }
      return lift[u][0];
    }

    /// @brief Finds the distance (number of edges) between two nodes.
    /// @param u The first node.
    /// @param v The second node.
    /// @return The distance in edges between `u` and `v`.
    int distance(const T &u, const T &v) const {
      return depth[u] + depth[v] - 2 * depth[lca(u, v)];
    }

    template <typename M>
    class augmented {
    private:
      const lift_view &base;
      std::vector<std::array<M, binary_lift_size>> agg;

    public:
      augmented(const lift_view &base, const std::vector<M> &vals) : base(base), agg(base.size()) {
        for (std::size_t i = 0; i < base.size(); ++i) {
          agg[i][0] = vals[i];
        }
        for (int bt = 1; bt < binary_lift_size; ++bt) {
          for (int i = 0; i < base.size(); ++i) {
            agg[i][bt] = agg[i][bt - 1] + agg[base.lift[i][bt - 1]][bt - 1];
          }
        }
      }

      /// @brief Accumulates values along the `k` edges going upward from node `u`.
      /// @param u The starting node.
      /// @param k The number of edges to traverse upward.
      /// @return The aggregated value under monoid `M` along the path of `k` edges
      /// from `u` toward its ancestor at depth `depth[u] - k`.
      M accum(T u, int k) const {
        M ans = {};
        for (int bt = 0; bt < binary_lift_size; ++bt) {
          if (k & 1 << bt) {
            ans = ans + agg[u][bt];
            u = base.lift[u][bt];
          }
        }
        return ans;
      }

      /// @brief Returns the aggregated value along all edges on the path between `u` and `v`.
      /// @param u The first node.
      /// @param v The second node.
      /// @return The aggregated value under monoid `M` over the edges on the unique path from `u` to `v`.
      M query_edges(const T &u, const T &v) const {
        T lca = base.lca(u, v);
        return accum(u, base.depth[u] - base.depth[lca]) + accum(v, base.depth[v] - base.depth[lca]);
      }

      /// @brief Returns the aggregated value across all nodes on the path between `u` and `v`.
      /// @param u The first node.
      /// @param v The second node.
      /// @return The aggregated value under monoid `M` over all nodes on the unique path from `u` to `v`,
      /// including both endpoints and their lowest common ancestor.
      M query_nodes(const T &u, const T &v) const {
        T lca = base.lca(u, v);
        return accum(u, base.depth[u] - base.depth[lca] + 1) + accum(v, base.depth[v] - base.depth[lca]);
      }
    };

    /// @brief Creates an augmented lift view with monoid accumulation support.
    /// @tparam M The monoid type used for aggregation (must define operator+ and an identity element).
    /// @param vals A vector of initial values (one per node), used as the base case for building the aggregation table.
    /// @return A `with<M>` object that supports accumulation and path queries under the monoid `M`.
    template <typename M>
    augmented<M> with(const std::vector<M> &vals) {
      return augmented{*this, vals};
    }
  };

  /// @brief Returns a `lift_view` that includes the functions `ancestor()`, `lca()` and `distance()` by precomputing information required.
  /// @param root The node to root the tree at.
  template <int binary_lift_size = 20>
  lift_view<binary_lift_size> lift(const T &root) {
    return lift_view<binary_lift_size>(*this, root);
  }
};
}; // namespace algo