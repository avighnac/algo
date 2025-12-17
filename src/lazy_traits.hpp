#pragma once

#include "traits.hpp"
#include <functional>

namespace algo {
template <typename T, typename F, typename Op>
struct lazy_traits;

template <typename T, typename F>
struct lazy_traits<T, F, std::plus<>> {
  static void apply(T &a, const F &f, int len) {
    a += f * len;
  }
  static void set(T &a, const F &f, int len) {
    a = f * len;
  }
  static void reverse(T &) {}
};

template <typename T, typename F>
struct lazy_traits<T, F, std::greater<>> {
  static void apply(T &a, const F &f, int) {
    a += f;
  }
  static void set(T &a, const F &f, int) {
    a = f;
  }
  static void reverse(T &) {}
};

template <typename T, typename F>
struct lazy_traits<T, F, std::less<>> {
  static void apply(T &a, const F &f, int) {
    a += f;
  }
  static void set(T &a, const F &f, int) {
    a = f;
  }
  static void reverse(T &) {}
};

template <typename traits, typename T, typename F>
concept has_set_trait = requires(T &a, const F &f, int len) {
  { traits::set(a, f, len) };
};
} // namespace algo