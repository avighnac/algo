#pragma once

namespace algo {
template <typename T, typename F>
struct lazy_traits {
  static void apply(T &a, const F &f, int len) {
    a = a + f * len;
  }
};
} // namespace algo