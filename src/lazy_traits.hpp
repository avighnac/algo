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
};
} // namespace algo