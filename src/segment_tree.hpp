#pragma once

#include <bit>
#include <functional>
#include <ostream>
#include <vector>

namespace algo {
/// @brief An iterative segment tree with a fixed size.
/// @tparam T Works on any type with a defined associative (but not necessarily
/// commutative) operator+, for example, `int` or `algo::min_t`.
template <typename T> class segment_tree {
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

  /// @brief Sets the value at the i-th index to x.
  /// @param i The index at which the value is being modified.
  /// @param x The new value at that index.
  void set(std::size_t i, const T &x) {
    for (seg[i += n] = x, i /= 2; i > 0; i /= 2) {
      seg[i] = seg[2 * i] + seg[2 * i + 1];
    }
  }

  /// @brief Performs associative accumulation.
  /// @param l The left endpoint (inclusive) of the range to accumulate.
  /// @param r The right endpoint (inclusive) of the range to accumulate.
  /// @return Returns the accumulated result of [l, r].
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

  /// @brief Finds the smallest index r ≥ l such that the predicate returns
  /// false for the accumulated value over [l, r].
  /// @param l The left boundary (inclusive) from which to start the
  /// accumulation.
  /// @param pred A monotonic predicate that returns true while the accumulated
  /// range is valid. Must return false eventually.
  /// @return The first index r ≥ l such that pred(accumulate(l, r)) == false.
  /// If pred is true for all r ∈ [l, _n), returns _n.
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

  /// @brief Returns a read-only reference to the value at index `i`.
  /// @param i The index to access.
  /// @return A const reference to the element at position `i` in the original
  /// array (i.e., the leaf at `seg[n + i]`).
  const T &at(std::size_t i) const { return seg[n + i]; }

  /// @brief Provides access to the element at index `i` via a proxy object.
  /// @param i The index to access or update.
  /// @return A proxy that allows assignment to the element at index `i`
  /// (calls `update()`), and read access via stream output.
  accessor operator[](std::size_t i) { return accessor(*this, i); }
};
} // namespace algo