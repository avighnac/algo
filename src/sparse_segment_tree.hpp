#pragma once

#include <numeric>

namespace algo {
/// @brief A sparse segment tree with nodes created lazily, so memory usage scales with the number of updates/queries rather than the full coordinate range.
/// @tparam T Monoid type (must define `operator+` and provide an identity element `T{}`).
template <typename T>
class sparse_segment_tree {
private:
  int64_t s, e, m;
  T val;

  sparse_segment_tree *left, *right;

public:
  sparse_segment_tree(int64_t l, int64_t r) : s(l), e(r), m(std::midpoint(s, e)), val(T{}), left(nullptr), right(nullptr) {}
  sparse_segment_tree(std::size_t n) : s(0), e(n - 1), m((n - 1) / 2), val(T{}), left(nullptr), right(nullptr) {}
  ~sparse_segment_tree() {
    delete left;
    delete right;
  }

  /// @brief Sets the value at the `i`-th index to `x`.
  /// @param i The index at which the value is being modified.
  /// @param x The new value at that index.
  T set(int64_t i, const T &x) {
    if (s == e) {
      return val = x;
    }
    if (i <= m) {
      val = (left = left ? left : new sparse_segment_tree(s, m))->set(i, x) + (right ? right->val : T{});
    } else {
      val = (left ? left->val : T{}) + (right = right ? right : new sparse_segment_tree(m + 1, e))->set(i, x);
    }
    return val;
  }

  /// @brief Finds the smallest index i ≥ l such that the predicate returns
  /// false for the accumulated value over [l, i].
  /// @param l The left boundary (inclusive) from which to start the
  /// accumulation.
  /// @param t A monotonic predicate that returns true while the accumulated
  /// range is valid.
  /// @return The first index i ≥ l such that pred(accumulate(l, i)) == false.
  /// If pred is true for all i ∈ [l, r], returns r + 1.
  template <typename Fn>
  int64_t min_right(int64_t l, Fn &&t, T p = T{}) const {
    if (e < l) {
      return e + 1;
    }
    if (s == e) {
      return t(p + val) ? e + 1 : s;
    }
    if (l <= m) {
      if (left) {
        int64_t i = left->min_right(l, t, p);
        if (i <= m) {
          return i;
        }
        p = p + left->val;
      }
    }
    return right ? right->min_right(l, t, p) : e + 1;
  }

  /// @brief Performs associative accumulation.
  /// @param l The left endpoint (inclusive) of the range to accumulate.
  /// @param r The right endpoint (inclusive) of the range to accumulate.
  /// @return Returns the accumulated result of [l, r].
  T query(int64_t l, int64_t r) const {
    if (r < s || l > e) {
      return T{};
    }
    if (l <= s && e <= r) {
      return val;
    }
    return (left ? left->query(l, r) : T{}) + (right ? right->query(l, r) : T{});
  }

  std::size_t size() const { return e - s + 1; }

  /// @brief Returns the value at index `i`.
  /// @param i The index to access.
  /// @return The element at position `i` in the segment tree.
  T at(int64_t i) const { return query(i, i); }
};
} // namespace algo