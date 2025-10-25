#pragma once

#include <random>
#include <utility>

namespace algo {
/// @brief A binary search tree that keeps itself balanced through randomization.
/// @tparam T A monoid type: must have `operator+` and an identity element defined.
template <typename T>
class treap {
private:
  static inline std::mt19937 gen{std::random_device{}()};

  struct node {
    int64_t s, p;
    node *l, *r;
    T x, a;
    node() : s(0), l(nullptr), r(nullptr), x(T{}), a(T{}), p(gen()) {}
    node(const T &x_) : s(1), l(nullptr), r(nullptr), x(x_), a(x_), p(gen()) {}
  };

  node *root;

  void destruct(node *n) {
    if (n) {
      destruct(n->l);
      destruct(n->r);
      delete n;
    }
  }

  int64_t s(node *n) const { return n ? n->s : 0; }
  T x(node *n) { return n ? n->x : T{}; }
  T a(node *n) { return n ? n->a : T{}; }

  node *update(node *n) {
    n->a = a(n->l) + n->x + a(n->r);
    n->s = s(n->l) + s(n->r) + 1;
    return n;
  }

  const T &at(node *n, int64_t i) const {
    if (i == s(n->l)) {
      return n->x;
    }
    if (i < s(n->l)) {
      return at(n->l, i);
    }
    return at(n->r, i - s(n->l) - 1);
  }

  node *set(node *n, int64_t i, const T &x) {
    if (i == s(n->l)) {
      n->x = x;
    } else if (i < s(n->l)) {
      set(n->l, i, x);
    } else {
      set(n->r, i - s(n->l) - 1, x);
    }
    return update(n);
  }

  std::pair<node *, node *> split(node *n, int64_t i) {
    if (!n) {
      return {nullptr, nullptr};
    }
    if (i < 0) {
      return {nullptr, n};
    }
    if (i == s(n->l)) {
      node *r = std::exchange(n->r, nullptr);
      return {update(n), r};
    }
    if (i < s(n->l)) {
      auto [l, r] = split(std::exchange(n->l, nullptr), i);
      return {l, merge(r, update(n))};
    }
    auto [l, r] = split(std::exchange(n->r, nullptr), i - s(n->l) - 1);
    return {merge(update(n), l), r};
  }

  node *merge(node *l, node *r) {
    if (!l) {
      return r;
    }
    if (!r) {
      return l;
    }
    if (l->p < r->p) {
      l->r = merge(l->r, r);
      return update(l);
    }
    r->l = merge(l, r->l);
    return update(r);
  }

  node *insert(node *n, int64_t i, const T &x) {
    auto [l, r] = split(n, i - 1);
    return merge(l, merge(new node(x), r));
  }

  node *erase(node *n, int64_t i) {
    auto [l, r1] = split(n, i - 1);
    auto [m, r] = split(r1, 0);
    delete m;
    return merge(l, r);
  }

  T _query(int64_t l, int64_t r) {
    auto [l1, r1] = split(root, l - 1);
    auto [l2, r2] = split(r1, r - l);
    T ans = a(l2);
    root = merge(l1, merge(l2, r2));
    return ans;
  }

  treap(node *n) : root(n) {}

public:
  treap() : root(nullptr) {}
  treap(const treap &) = delete;
  treap &operator=(const treap &) = delete;
  treap(treap &&) noexcept = default;
  treap &operator=(treap &&) noexcept = default;
  ~treap() { destruct(root); }

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
  const T &at(std::size_t i) const { return at(root, i); }

  /// @brief Sets the element at the `i`-th index to `x`.
  /// @param i The index whose element needs to be modified.
  /// @param x The new value of the element at index `i`.
  void set(std::size_t i, const T &x) { root = set(root, i, x); }

  /// @brief Returns the accumulated value over the range [l, r].
  /// @param l The starting (inclusive) index of the range.
  /// @param r The ending (inclusive) index of the range.
  /// @return The aggregate of all elements in the range [l, r].
  T query(std::size_t l, std::size_t r) { return _query(l, r); }

  /// @brief Splits the treap at index `i`, keeping the left part in this treap.
  /// @param i The split index (0-based). Elements `[0, i]` remain here; elements `(i, end]` go to the returned treap.
  /// @return A new treap containing all elements after index `i`.
  /// @warning The returned treap owns its nodes; modifying either treap invalidates iterators into the other.
  treap split(std::size_t i) {
    auto [l, r] = split(root, i);
    root = l;
    return treap(r);
  }

  /// @brief Merges `other` into this treap.
  /// @param other Another treap. It becomes invalid after this call.
  /// @note `other` is left in an unspecified state. Using it afterward is undefined behaviour.
  void merge(treap &other) {
    root = merge(root, other.root);
    other.root = nullptr;
  }
};
} // namespace algo