#include <algorithm>
#include <functional>
#include <iostream>
#include <type_traits>
#include <vector>

namespace algo {
template <typename T>
  requires std::is_integral_v<T>
class tree {
private:
  int n;
  std::vector<std::vector<int>> adj;
  T valid_node = -1;
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
  template <typename child>
  void dfs(child &&child_after, const T &start) const {
    dfs(nullptr, nullptr, child_after, nullptr, start);
  }
  std::vector<T> par(const T &root) const {
    std::vector<T> ans(n);
    ans[root] = root;
    dfs([&](const T &u, const T &i) { ans[i] = u; }, root);
    return ans;
  }
  std::vector<T> dep(const T &root) const {
    std::vector<T> ans(n);
    dfs(nullptr, [&](const T &u, const T &i) { ans[i] = ans[u] + 1; }, nullptr, nullptr, root);
    return ans;
  }
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
        for (int i = 0; i < g.n; ++i) {
          lift[i][bt] = lift[lift[i][bt - 1]][bt - 1];
        }
      }
    }
    std::size_t size() const { return g.size(); }
    T ancestor(T u, int k) const {
      for (int bt = 0; bt < binary_lift_size; ++bt) {
        if (k & 1 << bt) {
          u = lift[u][bt];
        }
      }
      return u;
    }
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
      M query_edges(const T &u, const T &v) const {
        T lca = base.lca(u, v);
        return accum(u, base.depth[u] - base.depth[lca]) + accum(v, base.depth[v] - base.depth[lca]);
      }
      M query_nodes(const T &u, const T &v) const {
        T lca = base.lca(u, v);
        return accum(u, base.depth[u] - base.depth[lca] + 1) + accum(v, base.depth[v] - base.depth[lca]);
      }
    };
    template <typename M>
    augmented<M> with(const std::vector<M> &vals) {
      return augmented{*this, vals};
    }
  };
  template <int binary_lift_size = 20>
  lift_view<binary_lift_size> lift(const T &root) {
    return lift_view<binary_lift_size>(*this, root);
  }
};
};

using namespace algo;

int main() {
  int n, q;
  std::cin >> n >> q;
  tree<int> adj(n);
  for (int i = 0, u, v; i < n - 1; ++i) {
    std::cin >> u >> v;
    adj.add_edge(u - 1, v - 1);
  }
  auto lift = adj.lift(0);
  while (q--) {
    int u, v;
    std::cin >> u >> v;
    std::cout << lift.distance(u - 1, v - 1) << '\n';
  }
}