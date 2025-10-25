#pragma once

#include "lazy_traits.hpp"
#include <random>
#include <utility>

namespace algo {
namespace internal {
template <typename T, typename F = T, typename traits = lazy_traits<T, F>>
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

  node *root;

  void destruct(node *n) {
    if (n) {
      destruct(n->l);
      destruct(n->r);
      delete n;
    }
  }

  int64_t s(node *n) { return n ? n->s : 0; }
  T a(node *n) { return n ? n->a : T{}; }

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

  void push(node *n) {
    if (!n) {
      return;
    }
    if (n->rev) {
      std::swap(n->l, n->r);
      flip(n->l);
      flip(n->r);
      n->rev = false;
    }
    __apply(n->l, n->t);
    __apply(n->r, n->t);
    n->t = F{};
  }

  node *pull(node *n) {
    if (!n) {
      return n;
    }
    n->s = s(n->l) + s(n->r) + 1;
    n->a = a(n->l) + n->x + a(n->r);
    return n;
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
  lazy_treap(const lazy_treap &) = delete;
  lazy_treap &operator=(const lazy_treap &) = delete;
  lazy_treap(lazy_treap &&) noexcept = default;
  lazy_treap &operator=(lazy_treap &&) noexcept = default;
  ~lazy_treap() { destruct(root); }

  /// @brief Returns the current size of the treap.
  std::size_t size() const { return s(root); }

  /// @brief Inserts `x` at the `i`-th index in the treap.
  /// @param i The index to insert at. If anything's already at this index, it's moved ahead.
  /// @param x The element to insert.
  void insert(std::size_t i, const T &x) { root = insert(root, i, x); }

  /// @brief Erases the `i`-th element.
  /// @param i The index whose element needs to be erased.
  void erase(std::size_t i) { root = erase(root, i); }

  /// @brief Returns the element at the `i`-th location.
  T at(std::size_t i) { return at(root, i); }

  /// @brief Applies a lazy operation over a range.
  /// @param l The starting index.
  /// @param r The ending index.
  /// @param x The lazy operation to apply to all elements in [l, r].
  void apply(std::size_t l, std::size_t r, const F &x)
    requires(!std::is_void_v<traits>)
  { _apply(l, r, x); }
  void apply(std::size_t i, const F &x)
    requires(!std::is_void_v<traits>)
  { apply(i, i, x); }

  /// @brief Returns the accumulated value over the range [l, r].
  /// @param l The starting (inclusive) index of the range.
  /// @param r The ending (inclusive) index of the range.
  /// @return The aggregate of all elements in the range [l, r].
  T query(std::size_t l, std::size_t r) { return _query(l, r); }

  /// @brief Splits the treap at index `i`, keeping the left part in this treap.
  /// @param i The split index (0-based). Elements `[0, i]` remain here; elements `(i, end]` go to the returned treap.
  /// @return A new treap containing all elements after index `i`.
  /// @warning The returned treap owns its nodes; modifying either treap invalidates iterators into the other.
  lazy_treap split(std::size_t i) {
    auto [l, r] = split(root, i);
    root = l;
    return treap(r);
  }

  /// @brief Merges `other` into this treap.
  /// @param other Another treap. After the call, it becomes empty.
  /// @note `other` can be safely reused or destroyed; it simply holds no elements after this.
  void merge(lazy_treap &other) {
    root = merge(root, other.root);
    other.root = nullptr;
  }
  void merge(lazy_treap &&other) { merge(other); }

  /// @brief Cuts out the range [l, r] from the treap and returns it. Note that the treap this is called on is modified.
  /// @param l The starting index of the range.
  /// @param r The ending index of the range.
  /// @return A treap containing the elements in [l, r].
  lazy_treap cut(int64_t l, int64_t r) {
    auto s1 = split(l - 1);
    merge(s1.split(r - l));
    return s1;
  }

  /// @brief Reverses the order of elements in the range [l, r], modifying the treap in place.
  /// @param l The starting index.
  /// @param r The ending index.
  void reverse(std::size_t l, std::size_t r) { _reverse(l, r); }
};
} // namespace internal
} // namespace algo