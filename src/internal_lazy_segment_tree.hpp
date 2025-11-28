#pragma once

#include "lazy_traits.hpp"
#include "monoid.hpp"
#include <bit>
#include <vector>

namespace algo {
namespace internal {
template <typename T, typename f = std::plus<>, T base = monoid_identity<T, f>::x, typename F = T, typename traits = lazy_traits<T, F>>
class lazy_segment_tree {
private:
  std::size_t _n, n;
  int h;
  std::vector<T> seg;
  std::vector<F> tag;

  T op(const T &a, const T &b) const {
    if constexpr (std::is_same_v<std::invoke_result_t<f &, T, T>, bool>) {
      return f{}(a, b) ? a : b;
    } else {
      return f{}(a, b);
    }
  }

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
    seg[i] = op(seg[2 * i], seg[2 * i + 1]);
  }

  void push_all(std::size_t l, std::size_t r) {
    for (int i = h; i > 0; --i) {
      if (((l >> i) << i) != l)
        push(l >> i, 1 << i);
      if (((r >> i) << i) != r)
        push((r - 1) >> i, 1 << i);
    }
  }

  void pull_all(std::size_t l, std::size_t r) {
    for (int i = 1; i <= h; ++i) {
      if (((l >> i) << i) != l)
        pull(l >> i);
      if (((r >> i) << i) != r)
        pull((r - 1) >> i);
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
  lazy_segment_tree(std::size_t _n, T x) : _n(_n), n(std::bit_ceil(_n)), h(std::countr_zero(n)), seg(2 * n), tag(n) {
    std::vector<T> vals(n, x);
    set(vals);
  }

  template <typename M>
  void set(const std::vector<M> &vals) {
    for (std::size_t i = 0; i < _n; ++i) {
      seg[n + i] = vals[i];
    }
    for (std::size_t i = n - 1; i > 0; --i) {
      seg[i] = op(seg[2 * i], seg[2 * i + 1]);
    }
    tag.assign(n, F{});
  }

  void apply(std::size_t l, std::size_t r, F x) {
    push_all(l += n, r += n + 1);
    for (std::size_t i = l, j = r, len = 1; i < j; i >>= 1, j >>= 1, len <<= 1) {
      if (i & 1)
        apply_to(i++, x, len);
      if (j & 1)
        apply_to(--j, x, len);
    }
    pull_all(l, r);
  }

  void apply(std::size_t i, F x) {
    apply(i, i, x);
  }

  T query(std::size_t l, std::size_t r) {
    push_all(l += n, r += n + 1);
    T ans_l = base, ans_r = base;
    for (std::size_t i = l, j = r; i < j; i >>= 1, j >>= 1) {
      if (i & 1)
        ans_l = op(ans_l, seg[i++]);
      if (j & 1)
        ans_r = op(seg[--j], ans_r);
    }
    pull_all(l, r);
    return op(ans_l, ans_r);
  }

  T at(std::size_t i) { return query(i, i); }

  std::size_t size() const { return _n; }
};
} // namespace internal
} // namespace algo