#pragma once

#include "monoid.hpp"
#include "proxy.hpp"
#include <algorithm>
#include <bit>
#include <istream>
#include <ostream>
#include <ranges>
#include <vector>

namespace algo {
/// @brief An iterative segment tree with a fixed size.
template <typename T, typename f = std::plus<>, T base = monoid_identity<T, f>::x>
class segment_tree {
private:
  std::size_t _n, n;
  std::vector<T> seg;

  T op(const T &a, const T &b) const {
    if constexpr (std::is_same_v<std::invoke_result_t<f &, T, T>, bool>) {
      return f{}(a, b) ? a : b;
    } else {
      return f{}(a, b);
    }
  }

  using reference = internal::proxy_ref<segment_tree, T>;

public:
  segment_tree(std::size_t _n) : _n(_n), n(std::bit_ceil(_n)), seg(2 * n, base) {}
  segment_tree(const segment_tree &other) : _n(other._n), n(other.n), seg(other.seg) {}
  segment_tree() : _n(0), n(0) {}
  segment_tree(std::size_t _n, const T &x) : _n(_n), n(std::bit_ceil(_n)), seg(2 * n) {
    std::fill(seg.begin() + n, seg.begin() + n + _n, x);
    for (std::size_t i = n - 1; i > 0; --i) {
      seg[i] = op(seg[2 * i], seg[2 * i + 1]);
    }
  }
  segment_tree(const std::vector<T> &vals) : _n(vals.size()), n(std::bit_ceil(_n)), seg(2 * n) {
    set(vals);
  }
  template <typename It>
  segment_tree(It first, It last) {
    _n = std::distance(first, last);
    n = std::bit_ceil(_n);
    seg.assign(2 * n, base);
    std::copy(first, last, seg.begin() + n);
    for (std::size_t i = n - 1; i > 0; --i) {
      seg[i] = op(seg[2 * i], seg[2 * i + 1]);
    }
  }
  template <std::ranges::input_range R>
  segment_tree(R &&r) : segment_tree(std::ranges::begin(r), std::ranges::end(r)) {}

  using iterator = internal::iterator<reference, segment_tree>;

  /// @brief Sets the value at the `i`-th index to `x`.
  /// @param i The index at which the value is being modified.
  /// @param x The new value at that index.
  void set(std::size_t i, const T &x) {
    for (seg[i += n] = x, i /= 2; i > 0; i /= 2) {
      seg[i] = op(seg[2 * i], seg[2 * i + 1]);
    }
  }

  /// @brief Bulk-assigns values to all leaves.
  /// @param vals A vector of size `n`, where `vals[i]` is the value for index `i`.
  template <typename M>
  void set(const std::vector<M> &vals) {
    std::copy(vals.begin(), vals.end(), seg.begin() + n);
    for (std::size_t i = n - 1; i > 0; --i) {
      seg[i] = op(seg[2 * i], seg[2 * i + 1]);
    }
  }

  /// @brief Performs associative accumulation.
  /// @param l The left endpoint (inclusive) of the range to accumulate.
  /// @param r The right endpoint (inclusive) of the range to accumulate.
  /// @return Returns the accumulated result of [l, r].
  T query(std::size_t l, std::size_t r) const {
    T ans_l = base, ans_r = base;
    for (l += n, r += n + 1; l < r; l /= 2, r /= 2) {
      if (l & 1)
        ans_l = op(ans_l, seg[l++]);
      if (r & 1)
        ans_r = op(seg[--r], ans_r);
    }
    return op(ans_l, ans_r);
  }

  /// @brief Finds the smallest index r ≥ l such that the predicate returns
  /// false for the accumulated value over [l, r].
  /// @param l The left boundary (inclusive) from which to start the
  /// accumulation.
  /// @param t A monotonic predicate that returns true while the accumulated
  /// range is valid.
  /// @return The first index r ≥ l such that t(accumulate(l, r)) == false.
  /// If `t` is true for all r ∈ [l, n), returns n.
  template <typename Fn>
  std::size_t min_right(std::size_t l, Fn &&t) const {
    T p = base;
    for (l += n; t(op(p, seg[l])) && l & (l + 1); l /= 2) {
      if (l & 1)
        p = op(p, seg[l++]);
    }
    if (t(op(p, seg[l]))) {
      return _n;
    }
    while (l < n) {
      if (t(op(p, seg[l <<= 1])))
        p = op(p, seg[l++]);
    }
    return l - n;
  }

  std::size_t size() const { return _n; }

  /// @brief Returns a read-only reference to the value at index `i`.
  /// @param i The index to access.
  /// @return A const reference to the element at position `i` in the segment tree (i.e., the leaf at `seg[n + i]`).
  const T &at(std::size_t i) const { return seg[n + i]; }
  const T &operator[](std::size_t i) const { return seg[n + i]; }

  reference operator[](std::size_t i) { return reference(this, i); }
  reference front() { return reference(this, 0); }
  reference back() { return reference(this, _n - 1); }

  iterator begin() { return iterator(this, 0); }
  iterator end() { return iterator(this, size()); }
};
} // namespace algo