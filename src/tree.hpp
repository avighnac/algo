#pragma once

#include "segment_tree.hpp"
#include <algorithm>
#include <functional>
#include <type_traits>
#include <vector>

namespace algo {
template <typename T>
  requires std::is_integral_v<T>
class tree {
private:
  std::size_t n;
  std::vector<std::vector<T>> adj;
  T valid_node = -1;

public:
  tree(std::size_t n) : n(n), adj(n) {}

  std::size_t size() const { return n; }

  void add_edge(const T &u, const T &v) {
    valid_node = u;
    adj[u].push_back(v);
    adj[v].push_back(u);
  }

  /// @brief Returns the adjacency list of the tree.
  const std::vector<std::vector<T>> &adjacency() const { return adj; }

  template <typename enter, typename child_before, typename child_after, typename exit>
  void dfs(enter &&on_enter, child_before &&on_before, child_after &&on_after, exit &&on_exit, const T &root) const {
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
    dfs(dfs, root, -1);
  }

  /// @brief Depth-first search with the `child_after` hook.
  /// @tparam child
  /// @param child_after
  /// @param start
  template <typename child>
  void dfs(child &&child_after, const T &start) const {
    dfs(nullptr, nullptr, std::forward<child>(child_after), nullptr, start);
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
    if (valid_node == -1) {
      return 0;
    }
    auto d1 = dep(valid_node);
    auto d2 = dep(std::max_element(d1.begin(), d1.end()) - d1.begin());
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
        for (std::size_t i = 0; i < g.n; ++i) {
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
          for (std::size_t i = 0; i < base.size(); ++i) {
            agg[i][bt] = agg[i][bt - 1] + agg[base.lift[i][bt - 1]][bt - 1];
          }
        }
      }

      /// @brief Accumulates values along the `k` edges going upward from node `u`.
      /// @param u The starting node.
      /// @param k The number of edges to traverse upward.
      /// @return The aggregated value under monoid `M` along the path of `k` edges
      /// from `u` toward its ancestor at depth `depth[u] - k`.
      M accum(const T &u, int k) const {
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
    /// @return A `with<M>` object that supports accumulation and static queries under the monoid `M`.
    template <typename M>
    augmented<M> with(const std::vector<M> &vals) {
      return augmented{*this, vals};
    }
  };

  /// @brief Returns a `lift_view` that includes the functions `ancestor()`, `lca()` and `distance()` by precomputing information required.
  /// @param root The node to root the tree at.
  template <int binary_lift_size = 20>
  lift_view<binary_lift_size> lift(const T &root) const {
    return lift_view<binary_lift_size>(*this, root);
  }

  template <typename M>
  class flatten_view {
  private:
    const tree<T> &g;
    std::vector<int> start, end;
    segment_tree<M> seg;

  public:
    flatten_view(const tree<T> &g, const T &root) : g(g), start(g.size()), end(g.size()), seg(g.size()) {
      int timer = 0;
      g.dfs([&](const T &u, const T &p) { start[u] = timer++; }, nullptr, nullptr, [&](const T &u, const T &p) { end[u] = timer - 1; }, root);
    }

    /// @brief Set the value at node `u`.
    void set(const T &u, const M &x) {
      seg.set(start[u], x);
    }

    /// @brief Bulk-assign values to all nodes.
    /// @param vals `std::vector` where `vals[u]` is the value for node `u`.
    void set(const std::vector<M> &vals) {
      std::vector<M> m(g.size());
      for (std::size_t i = 0; i < g.size(); ++i) {
        m[start[i]] = vals[i];
      }
      seg.set(m);
    }

    /// @brief Query the aggregate over the entire subtree rooted at `u`.
    M query(const T &u) const {
      return seg.query(start[u], end[u]);
    }

    /// @brief Proxy for convenient node access.
    struct accessor {
      flatten_view &fv;
      T u;
      accessor(flatten_view &fv, T u) : fv(fv), u(u) {}
      accessor &operator=(const M &x) {
        fv.set(u, x);
        return *this;
      }
      accessor &operator+=(const M &x) { return *this = M(*this) + x; }
      accessor &operator-=(const M &x) { return *this = M(*this) - x; }
      operator M() const { return fv.seg.at(fv.start[u]); }
      friend std::ostream &operator<<(std::ostream &os,
                                      const accessor &acc) {
        return os << M(acc);
      }
    };

    /// @brief Returns an accessor for node u.
    accessor operator[](const T &u) { return accessor(*this, u); }
  };

  /// @brief Creates a view that flattens the tree into an array via Euler tour.
  /// Each node `u` is assigned a contiguous segment `[start[u], end[u]]`.
  /// This allows subtree queries and point updates to be reduced to range queries.
  ///
  /// @tparam M The monoid type (must define `operator+` and have an identity).
  /// @param root The root of the tree.
  /// @return A `flatten_view<M>` object supporting subtree aggregation.
  template <typename M>
  flatten_view<M> flatten(const T &root) const {
    return flatten_view<M>{*this, root};
  }

  /// @brief Overload that also bulk-initializes node values.
  /// @param vals `std::vector` of values (size = number of nodes), where `vals[u]` is the value of node `u`.
  /// @param root The root of the tree.
  /// @return A `flatten_view<M>` initialized with `vals`.
  template <typename M>
  flatten_view<M> flatten(const std::vector<M> &vals, const T &root) const {
    flatten_view<M> fv{*this, root};
    fv.set(vals);
    return fv;
  }

  template <typename M>
  class hld_view {
  private:
    const tree<T> &g;
    std::vector<int> start, top, par, depth;
    segment_tree<M> seg;

    template <typename Fn>
    M query_path(T u, T v, Fn &&handle_last) const {
      M ans = {};
      while (top[u] != top[v]) {
        if (depth[top[u]] < depth[top[v]]) {
          std::swap(u, v);
        }
        ans = ans + seg.query(start[top[u]], start[u]);
        u = par[top[u]];
      }
      if (depth[u] > depth[v]) {
        std::swap(u, v);
      }
      ans = ans + handle_last(start[u], start[v]);
      return ans;
    }

  public:
    hld_view(const tree<T> &g, const T &root) : g(g), start(g.size()), top(g.size()), par(g.par(root)), depth(g.dep(root)), seg(g.size()) {
      std::vector<std::vector<T>> adj = g.adjacency();
      std::vector<int> sub(g.size());
      auto dfs1 = [&](auto &&self, T u, T p) -> void {
        for (T &i : adj[u]) {
          if (i == p) {
            continue;
          }
          self(self, i, u);
          sub[u] += sub[i] + 1;
          if (sub[i] > sub[adj[u][0]]) {
            std::swap(i, adj[u][0]);
          }
        }
      };
      int timer = 0;
      auto dfs2 = [&](auto &&self, T u, T p) -> void {
        start[u] = timer++;
        for (T &i : adj[u]) {
          if (i == p) {
            continue;
          }
          top[i] = i == adj[u][0] ? top[u] : i;
          self(self, i, u);
        }
      };
      dfs1(dfs1, root, -1);
      top[root] = root;
      dfs2(dfs2, root, -1);
    }

    void set(std::size_t i, const M &x) {
      seg.set(start[i], x);
    }

    /// @brief Returns the aggregate after traveling up `k` edges from `u`.
    /// @param u The node to start at.
    /// @param k The amount of edges to go up.
    /// @return If `k == 0`, returns the monoid identity. Otherwise, returns the monoid aggregate for `k` edges.
    M accum(const T &u, int k) const {
      k = std::min(k, depth[u]);
      M ans = M{};
      while (k > 0 && k - (depth[u] - depth[top[u]]) >= 0) {
        ans = ans + seg.query(start[top[u]], start[u]);
        k -= depth[u] - depth[top[u]];
        u = par[top[u]];
      }
      if (k > 0) {
        ans = ans + seg.query(start[u] - k, start[u]);
      }
      return ans;
    }

    /// @brief Query the aggregate of all edges on the path between `u` and `v`.
    /// @param u One endpoint of the path.
    /// @param v The other endpoint of the path.
    /// @return The monoid aggregate of edges along the unique path from `u` to `v`.
    M query_edges(const T &u, const T &v) const {
      return query_path(u, v, [&](int l, int r) { return seg.query(l + 1, r); });
    }

    /// @brief Query the aggregate of all nodes on the path between `u` and `v`.
    /// @param u One endpoint of the path.
    /// @param v The other endpoint of the path.
    /// @return The monoid aggregate of nodes along the unique path from `u` to `v`.
    M query_nodes(const T &u, const T &v) const {
      return query_path(u, v, [&](int l, int r) { return seg.query(l, r); });
    }

    /// @brief Proxy for convenient node access.
    struct accessor {
      hld_view &hv;
      T u;
      accessor(hld_view &hv, T u) : hv(hv), u(u) {}
      accessor &operator=(const M &x) {
        hv.set(u, x);
        return *this;
      }
      accessor &operator+=(const M &x) { return *this = M(*this) + x; }
      accessor &operator-=(const M &x) { return *this = M(*this) - x; }
      operator M() const { return hv.seg.at(hv.start[u]); }
      friend std::ostream &operator<<(std::ostream &os, const accessor &acc) {
        return os << M(acc);
      }
    };

    /// @brief Returns an accessor for node u.
    accessor operator[](const T &u) { return accessor(*this, u); }
  };

  /// @brief Builds a heavy-light decomposition view of the tree.
  /// @tparam M The monoid type (must define `operator+` and have an identity).
  /// @param root The root of the decomposition.
  /// @return An `hld_view<M>` object supporting path queries and updates.
  template <typename M>
  hld_view<M> hld(const T &root) const {
    return hld_view<M>{*this, root};
  }
};
}; // namespace algo