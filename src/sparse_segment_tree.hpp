#pragma once

#include <memory>
#include <numeric>

namespace algo {
/// @brief A sparse segment tree with nodes created lazily, so memory usage scales with the number of updates/queries rather than the full coordinate range.
/// @tparam T Monoid type (must define `operator+` and provide an identity element `T{}`).
template <typename T>
class sparse_segment_tree {
private:
  int64_t l_m, r_m, m_m;
  T val_m;

  std::unique_ptr<sparse_segment_tree> left_m, right_m;

  std::unique_ptr<sparse_segment_tree> &left() {
    if (!left_m) {
      left_m = std::make_unique<sparse_segment_tree>(l_m, m_m);
    }
    return left_m;
  }

  std::unique_ptr<sparse_segment_tree> &right() {
    if (!right_m) {
      right_m = std::make_unique<sparse_segment_tree>(m_m + 1, r_m);
    }
    return right_m;
  }

  struct accessor {
    sparse_segment_tree &st;
    int64_t i;
    accessor(sparse_segment_tree &st, int64_t i) : st(st), i(i) {}
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
  sparse_segment_tree(const int64_t &l, const int64_t &r) : l_m(l), r_m(r), m_m(std::midpoint(l_m, r_m)), val_m(T{}) {}
  sparse_segment_tree(const std::size_t &n) : l_m(0), r_m(n - 1), m_m(std::midpoint(l_m, r_m)), val_m(T{}) {}

  /// @brief Sets the value at the `i`-th index to `x`.
  /// @param i The index at which the value is being modified.
  /// @param x The new value at that index.
  T set(int64_t i, const T &x) {
    if (l_m == r_m) {
      return val_m = x;
    }
    if (i <= m_m) {
      val_m = left()->set(i, x) + (right_m ? right_m->val_m : T{});
    } else {
      val_m = (left_m ? left_m->val_m : T{}) + right()->set(i, x);
    }
    return val_m;
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
    if (r_m < l) {
      return r_m + 1;
    }
    if (l_m == r_m) {
      return t(p + val_m) ? r_m + 1 : l_m;
    }
    if (l <= m_m) {
      if (left_m) {
        int64_t i = left_m->min_right(l, t, p);
        if (i <= m_m) {
          return i;
        }
        p = p + left_m->val_m;
      }
    }
    return right_m ? right_m->min_right(l, t, p) : r_m + 1;
  }

  /// @brief Performs associative accumulation.
  /// @param l The left endpoint (inclusive) of the range to accumulate.
  /// @param r The right endpoint (inclusive) of the range to accumulate.
  /// @return Returns the accumulated result of [l, r].
  T query(int64_t l, int64_t r) const {
    if (r < l_m || l > r_m) {
      return T{};
    }
    if (l <= l_m && r_m <= r) {
      return val_m;
    }
    return (left_m ? left_m->query(l, r) : T{}) + (right_m ? right_m->query(l, r) : T{});
  }

  std::size_t size() const { return r_m - l_m + 1; }

  /// @brief Returns the value at index `i`.
  /// @param i The index to access.
  /// @return The element at position `i` in the segment tree.
  T at(int64_t i) const { return query(i, i); }

  accessor operator[](int64_t i) { return accessor(*this, i); }
};
} // namespace algo