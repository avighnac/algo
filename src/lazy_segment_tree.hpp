#pragma once

#include "lazy_traits.hpp"
#include <bit>
#include <vector>

namespace algo {
/// @brief A generic lazy segment tree supporting range accumulation and composition.
/// @tparam T Value type. For example, `int64_t` for range sum queries, `algo::min_t<int>` for range min queries. Must define `operator+`.
/// @tparam F Lazy type. For example, `int` for range add updates. Must define `operator+`.
/// @tparam traits A struct that must define
/// `static void apply(T &a, const F &f, int len)`;
/// used to apply lazy updates to children. This only needs to be specified if you're using a custom `T` or `F`.
template <typename T, typename F = T, typename traits = lazy_traits<T, F>>
class lazy_segment_tree {
private:
  std::size_t _n, n;
  int h;
  std::vector<T> seg;
  std::vector<F> tag;

  void push(std::size_t i, std::size_t len) {
    len >>= 1;
    traits::apply(seg[2 * i], tag[i], len);
    traits::apply(seg[2 * i + 1], tag[i], len);
    if (2 * i < n) {
      tag[2 * i] = tag[2 * i] + tag[i];
      tag[2 * i + 1] = tag[2 * i + 1] + tag[i];
    }
    tag[i] = F{};
  }

  void pull(std::size_t i) {
    seg[i] = seg[2 * i] + seg[2 * i + 1];
  }

  void push_all(std::size_t l, std::size_t r) {
    for (int i = h; i > 0; --i) {
      if (((l >> i) << i) != l) push(l >> i, 1 << i);
      if (((r >> i) << i) != r) push((r - 1) >> i, 1 << i);
    }
  }

  void pull_all(std::size_t l, std::size_t r) {
    for (int i = 1; i <= h; ++i) {
      if (((l >> i) << i) != l) pull(l >> i);
      if (((r >> i) << i) != r) pull((r - 1) >> i);
    }
  }

  void apply_to(std::size_t i, const F &x, std::size_t len) {
    traits::apply(seg[i], x, len);
    if (i < n) {
      tag[i] = tag[i] + x;
    }
  }

public:
  lazy_segment_tree(std::size_t _n) : _n(_n), n(std::bit_ceil(_n)), h(std::countr_zero(n)), seg(2 * n), tag(n) {}

  void apply(std::size_t l, std::size_t r, F x) {
    push_all(l += n, r += n + 1);
    for (std::size_t i = l, j = r, len = 1; i < j; i >>= 1, j >>= 1, len <<= 1) {
      if (i & 1) apply_to(i++, x, len);
      if (j & 1) apply_to(--j, x, len);
    }
    pull_all(l, r);
  }

  void apply(std::size_t i, F x) {
    apply(i, i, x);
  }

  T query(std::size_t l, std::size_t r) {
    push_all(l += n, r += n + 1);
    T ans_l = T{}, ans_r = T{};
    for (std::size_t i = l, j = r; i < j; i >>= 1, j >>= 1) {
      if (i & 1) ans_l = ans_l + seg[i++];
      if (j & 1) ans_r = seg[--j] + ans_r;
    }
    pull_all(l, r);
    return ans_l + ans_r;
  }

  T at(std::size_t i) { return query(i, i); }

  T size() const { return _n; }
};
} // namespace algo