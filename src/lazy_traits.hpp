#pragma once

#include "traits.hpp"

namespace algo {
template <typename T, typename F>
struct lazy_traits {
  static void apply(T &a, const F &f, int len) {
    if constexpr (is_idempotent_v<T>) {
      a = F{a} + f;
    } else {
      a = a + f * len;
    }
  }
  static void set(T &a, const F &f, int len) {
    if constexpr (is_idempotent_v<T>) {
      a = f;
    } else {
      a = f * len;
    }
  }
  static void reverse(T &a) {}
};

template <typename traits, typename T, typename F>
concept has_set_trait = requires(T &a, const F &f, int len) {
  { traits::set(a, f, len) };
};
} // namespace algo