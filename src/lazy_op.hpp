#pragma once

namespace algo {
namespace internal {
template <typename F>
struct lazy_add_set_op {
  F x, set;
  bool has_set;

  lazy_add_set_op() : x(F{}), set(F{}), has_set(false) {}
  lazy_add_set_op(F x, F set, bool has_set) : x(x), set(set), has_set(has_set) {}

  lazy_add_set_op operator+(const lazy_add_set_op &o) const {
    if (o.has_set) {
      return o;
    }
    if (has_set) {
      return {F{}, set + o.x, true};
    }
    return {x + o.x, F{}, false};
  }
};

template <typename T, typename F, typename traits>
struct lazy_add_set_combine {
  static void apply(T &a, const lazy_add_set_op<F> &f, int len) {
    if (f.has_set) {
      traits::set(a, f.set, len);
    } else {
      traits::apply(a, f.x, len);
    }
  }
};
} // namespace internal
} // namespace algo