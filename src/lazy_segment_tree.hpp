#pragma once

#include "internal_lazy_segment_tree.hpp"
#include "lazy_op.hpp"
#include "proxy.hpp"

namespace algo {
template <typename T, typename f = std::plus<>, T base = monoid_identity<T, f>::x, typename F = T, typename traits = lazy_traits<T, F>>
class lazy_add_set_segment_tree {
private:
  using lazy_op = internal::lazy_add_set_op<F>;
  using combine = internal::lazy_add_set_combine<T, F, traits>;
  using reference = internal::proxy_ref<lazy_add_set_segment_tree, T>;

  internal::lazy_segment_tree<T, f, base, lazy_op, combine> st;

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
  reference operator[](std::size_t i) { return reference(this, i); }
  reference front() { return reference(this, 0); }
  reference back() { return reference(this, size() - 1); }

  std::size_t size() const { return st.size(); }

  using iterator = internal::iterator<reference, lazy_add_set_segment_tree>;

  iterator begin() { return iterator(this, 0); }
  iterator end() { return iterator(this, size()); }
};

/// @brief A generic lazy segment tree supporting range updates and queries.
template <typename T, typename f = std::plus<>, T base = monoid_identity<T, f>::x, typename F = T, typename traits = lazy_traits<T, F>>
using lazy_segment_tree =
    std::conditional_t<
        has_set_trait<traits, T, F>,
        lazy_add_set_segment_tree<T, f, base, F, traits>,
        internal::lazy_segment_tree<T, f, base, F, traits>>;
} // namespace algo