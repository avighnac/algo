#pragma once

#include "internal_lazy_segment_tree.hpp"
#include "lazy_op.hpp"

namespace algo {
template <typename T, typename F = T, typename traits = lazy_traits<T, F>>
class lazy_add_set_segment_tree {
private:
  using lazy_op = internal::lazy_add_set_op<F>;
  using combine = internal::lazy_add_set_combine<T, F, traits>;
  internal::lazy_segment_tree<T, lazy_op, combine> st;

public:
  lazy_add_set_segment_tree(std::size_t _n) : st(_n) {}
  lazy_add_set_segment_tree(std::size_t _n, T x) : st(_n, x) {}

  template <typename M>
  void set(const std::vector<M> &vals) { st.set(vals); }

  void add(std::size_t l, std::size_t r, F x) {
    st.apply(l, r, {x, F{}, false});
  }
  void add(std::size_t i, F x) { add(i, i, x); }
  void apply(std::size_t l, std::size_t r, F x) { add(l, r, x); }
  void apply(std::size_t i, F x) { apply(i, i, x); }

  void set(std::size_t l, std::size_t r, F x) {
    st.apply(l, r, {F{}, x, true});
  }
  void set(std::size_t i, F x) { set(i, i, x); }

  T query(std::size_t l, std::size_t r) {
    return st.query(l, r);
  }

  T at(std::size_t i) { return st.query(i, i); }

  std::size_t size() const { return st.size(); }
};

/// @brief A generic lazy segment tree supporting range updates and queries.
/// @tparam T Value type (e.g. `int64_t` for range sum, `algo::min_t<int>` for range min). Must define `operator+`.
/// @tparam F Lazy type (e.g. `int` for range add). Must define `operator+`.
/// @tparam traits A struct that must define `static void apply(T &a, const F &f, int len)`;
/// optionally, defining `static void set(T &a, const F &f, int len)` enables range assignment updates.
template <typename T, typename F = T, typename traits = lazy_traits<T, F>>
using lazy_segment_tree =
    std::conditional_t<
        has_set_trait<traits, T, F>,
        lazy_add_set_segment_tree<T, F, traits>,
        internal::lazy_segment_tree<T, F, traits>>;
} // namespace algo