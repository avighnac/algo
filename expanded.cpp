#include <algorithm>
#include <bit>
#include <functional>
#include <iostream>
#include <ostream>
#include <type_traits>
#include <vector>

namespace algo {
template <typename T>
class segment_tree {
private:
  std::size_t _n, n;
  std::vector<T> seg;
  struct accessor {
    segment_tree &st;
    std::size_t i;
    accessor(segment_tree &st, std::size_t i) : st(st), i(i) {}
    accessor &operator=(const T &x) {
      st.set(i, x);
      return *this;
    }
    accessor &operator+=(const T &x) { return *this = st.at(i) + x; }
    accessor &operator-=(const T &x) { return *this = st.at(i) - x; }
    operator T() const { return st.at(i); }
    friend std::ostream &operator<<(std::ostream &os, const accessor &acc) {
      return os << T(acc);
    }
  };

public:
  segment_tree(std::size_t _n) : _n(_n), n(std::bit_ceil(_n)), seg(2 * n) {}
  segment_tree(const segment_tree &other)
      : _n(other._n), n(other.n), seg(other.seg) {}
  segment_tree() : _n(0), n(0) {}
  segment_tree(std::size_t _n, const T &x)
      : _n(_n), n(std::bit_ceil(_n)), seg(2 * n) {
    for (std::size_t i = n; i < n + _n; ++i) {
      seg[i] = x;
    }
    for (std::size_t i = n - 1; i > 0; --i) {
      seg[i] = seg[2 * i] + seg[2 * i + 1];
    }
  }
  segment_tree(const std::vector<T> &vals) : _n(vals.size()), n(std::bit_ceil(_n)), seg(2 * n) {
    set(vals);
  }
  void set(std::size_t i, const T &x) {
    for (seg[i += n] = x, i /= 2; i > 0; i /= 2) {
      seg[i] = seg[2 * i] + seg[2 * i + 1];
    }
  }
  void set(const std::vector<T> &vals) {
    for (std::size_t i = 0; i < _n; ++i) {
      seg[n + i] = vals[i];
    }
    for (std::size_t i = n - 1; i > 0; --i) {
      seg[i] = seg[2 * i] + seg[2 * i + 1];
    }
  }
  T query(std::size_t l, std::size_t r) const {
    T ans_l = T{}, ans_r = T{};
    for (l += n, r += n + 1; l < r; l /= 2, r /= 2) {
      if (l & 1)
        ans_l = ans_l + seg[l++];
      if (r & 1)
        ans_r = seg[--r] + ans_r;
    }
    return ans_l + ans_r;
  }
  std::size_t min_right(std::size_t l, const std::function<bool(T)> &t) const {
    T p{};
    for (l += n; t(p + seg[l]) && l & (l + 1); l /= 2) {
      if (l & 1)
        p = p + seg[l++];
    }
    if (t(p + seg[l])) {
      return _n;
    }
    while (l < n) {
      if (t(p + seg[l <<= 1]))
        p = p + seg[l++];
    }
    return l - n;
  }
  std::size_t size() const { return _n; }
  std::size_t _size() const { return n; }
  const T &at(std::size_t i) const { return seg[n + i]; }
  accessor operator[](std::size_t i) { return accessor(*this, i); }
};
} // namespace algo
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
    void set(const T &u, const M &x) {
      seg.set(start[u], x);
    }
    void set(const std::vector<M> &vals) {
      std::vector<M> m(g.size());
      for (std::size_t i = 0; i < g.size(); ++i) {
        m[start[i]] = vals[i];
      }
      seg.set(m);
    }
    M query(const T &u) const {
      return seg.query(start[u], end[u]);
    }
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
    accessor operator[](const T &u) { return accessor(*this, u); }
  };
  template <typename M>
  flatten_view<M> flatten(const T &root) const {
    return flatten_view<M>{*this, root};
  }
  template <typename M>
  flatten_view<M> flatten(const std::vector<M> &vals, const T &root) const {
    flatten_view<M> fv{*this, root};
    fv.set(vals);
    return fv;
  }
};
}; // namespace algo

using namespace algo;

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int n, q;
  std::cin >> n >> q;
  std::vector<int64_t> a(n);
  for (int64_t &i : a) {
    std::cin >> i;
  }
  tree<int> adj(n);
  for (int i = 0, u, v; i < n - 1; ++i) {
    std::cin >> u >> v;
    adj.add_edge(u - 1, v - 1);
  }
  auto flat = adj.flatten<int64_t>(a, 0);
  while (q--) {
    int type, s, x;
    std::cin >> type >> s;
    if (type == 1) {
      std::cin >> x;
      flat[s - 1] = x;
    } else {
      std::cout << flat.query(s - 1) << '\n';
    }
  }
}