#pragma once

#include "internal_lazy_treap.hpp"
#include "lazy_op.hpp"

namespace algo {
template <typename T, typename F = T, typename traits = lazy_traits<T, F>>
class lazy_add_set_treap {
private:
  using lazy_op = internal::lazy_add_set_op<F>;
  using combine = internal::lazy_add_set_combine<T, F, traits>;
  internal::lazy_treap<T, lazy_op, combine> treap;

public:
  lazy_add_set_treap() : treap() {}

  void add(std::size_t l, std::size_t r, F x) {
    treap.apply(l, r, {x, F{}, false});
  }
  void add(std::size_t i, F x) { add(i, i, x); }
  void apply(std::size_t l, std::size_t r, F x) { add(l, r, x); }
  void apply(std::size_t i, F x) { apply(i, i, x); }

  void set(std::size_t l, std::size_t r, F x) {
    treap.apply(l, r, {F{}, x, true});
  }
  void set(std::size_t i, F x) { set(i, i, x); }

  T query(std::size_t l, std::size_t r) {
    return treap.query(l, r);
  }

  T at(std::size_t i) { return treap.at(i); }

  std::size_t size() const { return treap.size(); }
};

template <typename T, typename F = T, typename traits = lazy_traits<T, F>>
using lazy_treap =
    std::conditional_t<
        has_set_trait<traits, T, F>,
        lazy_add_set_treap<T, F, traits>,
        internal::lazy_treap<T, F, traits>>;
}; // namespace algo