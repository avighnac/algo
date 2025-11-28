#pragma once

#include "lazy_traits.hpp"
#include "monoid.hpp"
#include <random>
#include <utility>

namespace algo {
namespace internal {
template <typename T, typename f = std::plus<>, T base = monoid_identity<T, f>::x, typename F = T, typename traits = lazy_traits<T, F>>
class lazy_treap {
private:
  static inline std::mt19937 gen{std::random_device{}()};

  struct node {
    int64_t s, p;
    node *l, *r;
    T x, a;
    F t;
    bool rev;
    node(const T &_x) : s(1), p(gen()), l(nullptr), r(nullptr), x(_x), a(_x), t(F{}), rev(false) {}
  };

  T op(const T &a, const T &b) const {
    if constexpr (std::is_same_v<std::invoke_result_t<f &, T, T>, bool>) {
      return f{}(a, b) ? a : b;
    } else {
      return f{}(a, b);
    }
  }

  node *root;

  void destruct(node *n) {
    if (n) {
      destruct(n->l);
      destruct(n->r);
      delete n;
    }
  }

  int64_t s(node *n) const { return n ? n->s : 0; }
  T a(node *n) { return n ? n->a : base; }

  void flip(node *n) {
    if (n) {
      n->rev ^= 1;
    }
  }

  void __apply(node *n, const F &t) {
    if constexpr (!std::is_void_v<traits>) {
      if (n) {
        traits::apply(n->x, t, 1);
        traits::apply(n->a, t, n->s);
        n->t = n->t + t;
      }
    }
  }

  void pull_rev(node *n) {
    if (!n) {
      return;
    }
    if (n->rev) {
      std::swap(n->l, n->r);
      flip(n->l);
      flip(n->r);
      n->rev = false;
      traits::reverse(n->a);
    }
  }

  void push(node *n) {
    if (!n) {
      return;
    }
    pull_rev(n);
    __apply(n->l, n->t);
    __apply(n->r, n->t);
    n->t = F{};
  }

  node *pull(node *n) {
    if (!n) {
      return n;
    }
    pull_rev(n->l);
    pull_rev(n->r);
    n->s = s(n->l) + s(n->r) + 1;
    n->a = op(op(a(n->l), n->x), a(n->r));
    return n;
  }

  node *build(const std::vector<T> &a) {
    std::vector<node *> st;
    st.reserve(a.size());
    node *root = nullptr;
    for (const T &x : a) {
      node *cur = new node(x);
      node *prev = nullptr;
      while (!st.empty() && st.back()->p < cur->p) {
        prev = st.back();
        st.pop_back();
        pull(prev);
      }
      cur->l = prev;
      if (!st.empty()) {
        st.back()->r = cur;
      } else {
        root = cur;
      }
      st.push_back(cur);
    }
    for (int i = st.size() - 1; i >= 0; --i) {
      pull(st[i]);
    }
    return root;
  }

  std::pair<node *, node *> split(node *n, int64_t i) { // split into [0...i] [i+1,n-1]
    if (!n) {
      return {nullptr, nullptr};
    }
    if (i < 0) {
      return {nullptr, n};
    }
    push(n);
    if (i == s(n->l)) {
      node *r = std::exchange(n->r, nullptr);
      return {pull(n), r};
    }
    if (i < s(n->l)) {
      auto [l, r] = split(std::exchange(n->l, nullptr), i);
      return {l, merge(r, pull(n))};
    }
    auto [l, r] = split(std::exchange(n->r, nullptr), i - s(n->l) - 1);
    return {merge(pull(n), l), r};
  }

  node *merge(node *l, node *r) {
    if (!l) {
      return r;
    }
    if (!r) {
      return l;
    }
    if (l->p < r->p) {
      push(l);
      l->r = merge(l->r, r);
      return pull(l);
    }
    push(r);
    r->l = merge(l, r->l);
    return pull(r);
  }

  node *insert(node *n, int64_t i, const T &x) {
    auto [l, r] = split(n, i - 1);
    return merge(merge(l, new node(x)), r);
  }

  node *erase(node *n, int64_t i) {
    auto [l, r1] = split(n, i - 1);
    auto [m, r] = split(r1, 0);
    delete m;
    return merge(l, r);
  }

  T at(node *n, int64_t i) {
    push(n);
    if (i == s(n->l)) {
      return n->x;
    }
    if (i < s(n->l)) {
      return at(n->l, i);
    }
    return at(n->r, i - s(n->l) - 1);
  }

  T _query(int64_t l, int64_t r) {
    auto [l1, r1] = split(root, l - 1);
    auto [l2, r2] = split(r1, r - l);
    T ans = a(l2);
    root = merge(merge(l1, l2), r2);
    return ans;
  }

  void _apply(int64_t l, int64_t r, const F &x) {
    auto [l1, r1] = split(root, l - 1);
    auto [l2, r2] = split(r1, r - l);
    __apply(l2, x);
    root = merge(merge(l1, l2), r2);
  }

  void _reverse(int64_t l, int64_t r) {
    auto [l1, r1] = split(root, l - 1);
    auto [l2, r2] = split(r1, r - l);
    if (l2) {
      l2->rev ^= 1;
    }
    root = merge(merge(l1, l2), r2);
  }

  lazy_treap(node *n) : root(n) {}

public:
  lazy_treap() : root(nullptr) {}
  lazy_treap(const std::vector<T> &a) { root = build(a); }
  lazy_treap(std::size_t n, const T &x) {
    std::vector<T> a(n, x);
    root = build(a);
  }
  lazy_treap(std::size_t n) {
    std::vector<T> a(n);
    root = build(a);
  }
  lazy_treap(const lazy_treap &) = delete;
  lazy_treap &operator=(const lazy_treap &) = delete;
  lazy_treap(lazy_treap &&other) noexcept
      : root(std::exchange(other.root, nullptr)) {}
  lazy_treap &operator=(lazy_treap &&other) noexcept {
    if (this != &other) {
      destruct(root);
      root = std::exchange(other.root, nullptr);
    }
    return *this;
  }
  ~lazy_treap() { destruct(root); }

  std::size_t size() const { return s(root); }
  bool empty() const { return size() == 0; }

  void insert(std::size_t i, const T &x) { root = insert(root, i, x); }
  void insert(int64_t i, const std::vector<T> &a) {
    lazy_treap r = split(i - 1);
    lazy_treap vals(a);
    merge(vals);
    merge(r);
  }

  void erase(std::size_t i) { root = erase(root, i); }

  T at(std::size_t i) { return at(root, i); }

  T back() { return at(size() - 1); }

  void apply(std::size_t l, std::size_t r, const F &x)
    requires(!std::is_void_v<traits>)
  {
    _apply(l, r, x);
  }
  void apply(std::size_t i, const F &x)
    requires(!std::is_void_v<traits>)
  {
    apply(i, i, x);
  }

  T query(std::size_t l, std::size_t r) { return _query(l, r); }

  lazy_treap split(std::size_t i) {
    auto [l, r] = split(root, i);
    root = l;
    return lazy_treap(r);
  }

  void merge(lazy_treap &other) {
    root = merge(root, other.root);
    other.root = nullptr;
  }
  void merge(lazy_treap &&other) { merge(other); }

  lazy_treap cut(int64_t l, int64_t r) {
    auto s1 = split(l - 1);
    merge(s1.split(r - l));
    return s1;
  }

  void reverse(std::size_t l, std::size_t r) { _reverse(l, r); }
};
} // namespace internal
} // namespace algo