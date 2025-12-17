#pragma once

#include "mint.hpp"
#include <array>
#include <bit>
#include <complex>
#include <cstdint>
#include <numbers>
#include <type_traits>
#include <vector>

#if defined(__AVX2__) && defined(__x86_64__)
#include "experimental/ntt998244353.hpp"
#endif

namespace algo {
namespace internal {
using complex = std::complex<double>;
constexpr inline double pi = std::numbers::pi;

inline const std::vector<int> &get_rev(int w) {
  static int last_w = -1;
  static std::vector<int> rev;
  if (w != last_w) {
    last_w = w;
    rev.resize(1 << w);
    rev[0] = 0;
    for (int i = 1; i < 1 << w; ++i) {
      // i = (i >> 1), append (i & 1)
      // rev(i) = (i & 1), append rev(i >> 1)
      rev[i] = ((i & 1) << (w - 1)) | (rev[i >> 1] >> 1);
    }
  }
  return rev;
}

void fft(std::vector<complex> &a, bool inv) {
  const int _n = a.size();
  const int w = std::bit_width(a.size()) - 1;
  const auto &rev = get_rev(w);
  for (int i = 0; i < _n; ++i) {
    if (i < rev[i]) {
      std::swap(a[i], a[rev[i]]);
    }
  }
  for (int n = 2; n <= _n; n <<= 1) {
    complex base = std::polar(1.0, (inv ? -2 : 2) * pi / n);
    for (int i = 0; i < _n; i += n) {
      complex w = 1;
      for (int j = 0; j < n / 2; ++j) {
        complex u = a[i + j], v = w * a[i + j + n / 2];
        a[i + j] = u + v;
        a[i + j + n / 2] = u - v;
        w *= base;
      }
    }
  }
  if (inv) {
    for (auto &x : a) {
      x /= _n;
    }
  }
}

template <int64_t mod>
struct ntt_traits;

template <>
struct ntt_traits<998244353> {
  static constexpr int64_t r = 31;
  static constexpr int p = 23;
};

template <int64_t mod>
const std::array<mint<mod>, ntt_traits<mod>::p + 1> &get_roots() {
  static const std::array<mint<mod>, ntt_traits<mod>::p + 1> roots = [] {
    std::array<mint<mod>, ntt_traits<mod>::p + 1> r{};
    r[ntt_traits<mod>::p] = ntt_traits<mod>::r;
    for (int i = ntt_traits<mod>::p - 1; i >= 0; --i) {
      r[i] = r[i + 1] * r[i + 1];
    }
    return r;
  }();
  return roots;
}

template <int64_t mod>
const std::array<mint<mod>, ntt_traits<mod>::p + 1> &get_inv_roots() {
  static const std::array<mint<mod>, ntt_traits<mod>::p + 1> inv_roots = [] {
    std::array<mint<mod>, ntt_traits<mod>::p + 1> r{};
    r[ntt_traits<mod>::p] = mint<mod>(ntt_traits<mod>::r).inv();
    for (int i = ntt_traits<mod>::p - 1; i >= 0; --i) {
      r[i] = r[i + 1] * r[i + 1];
    }
    return r;
  }();
  return inv_roots;
}

template <int64_t mod>
void ntt(std::vector<mint<mod>> &a, bool inv) {
  const int _n = a.size();
  const int w = std::bit_width(a.size()) - 1;
  const auto &roots = inv ? get_inv_roots<mod>() : get_roots<mod>();
  const auto &rev = get_rev(w);
  for (int i = 0; i < _n; ++i) {
    if (i < rev[i]) {
      std::swap(a[i], a[rev[i]]);
    }
  }
  for (uint32_t n = 2, wd = 1; n <= _n; n <<= 1, ++wd) {
    mint<mod> base = roots[wd];
    for (int i = 0; i < _n; i += n) {
      mint<mod> w = 1;
      for (int j = 0; j < n / 2; ++j) {
        mint<mod> u = a[i + j], v = w * a[i + j + n / 2];
        a[i + j] = u + v;
        a[i + j + n / 2] = u - v;
        w *= base;
      }
    }
  }
  if (inv) {
    mint<mod> inv_n = mint<mod>(_n).inv();
    for (auto &x : a) {
      x *= inv_n;
    }
  }
}
} // namespace internal

template <typename T>
  requires std::is_integral_v<T>
std::vector<T> convolute(const std::vector<T> &a, const std::vector<T> &b) {
  const uint32_t _n = a.size() + b.size() - 1;
  const int n = std::bit_ceil(_n);
  std::vector<internal::complex> a_cmpl(n), b_cmpl(n);
  std::copy(a.begin(), a.end(), a_cmpl.begin());
  std::copy(b.begin(), b.end(), b_cmpl.begin());
  internal::fft(a_cmpl, false);
  internal::fft(b_cmpl, false);
  for (int i = 0; i < n; ++i) {
    a_cmpl[i] *= b_cmpl[i];
  }
  internal::fft(a_cmpl, true);
  std::vector<T> c(_n);
  for (int i = 0; i < _n; ++i) {
    c[i] = std::round(a_cmpl[i].real());
  }
  return c;
}

template <int64_t mod>
std::vector<mint<mod>> convolute(std::vector<mint<mod>> a, std::vector<mint<mod>> b) {
  const uint32_t _n = a.size() + b.size() - 1;
  const int n = std::bit_ceil(_n);
  a.resize(n);
  b.resize(n);
  internal::ntt(a, false);
  internal::ntt(b, false);
  for (int i = 0; i < n; ++i) {
    a[i] *= b[i];
  }
  internal::ntt(a, true);
  a.resize(_n);
  return a;
}

template <>
inline std::vector<mint<998244353>>
convolute<998244353>(std::vector<mint<998244353>> a, std::vector<mint<998244353>> b) {
#if defined(__AVX2__) && defined(__x86_64__)
  std::vector<int> aa(a.size()), bb(b.size());
  for (int i = 0; i < a.size(); ++i) {
    aa[i] = a[i];
  }
  for (int i = 0; i < b.size(); ++i) {
    bb[i] = b[i];
  }
  std::vector<int> cc = algo::experimental::ntt998244353(aa, bb);
  std::vector<mint<998244353>> c(cc.size());
  for (int i = 0; i < cc.size(); ++i) {
    c[i] = mint<998244353>(cc[i]);
  }
  return c;
#else
  const uint32_t _n = a.size() + b.size() - 1;
  const int n = std::bit_ceil(_n);
  a.resize(n);
  b.resize(n);
  internal::ntt(a, false);
  internal::ntt(b, false);
  for (int i = 0; i < n; ++i) {
    a[i] *= b[i];
  }
  internal::ntt(a, true);
  a.resize(_n);
  return a;
#endif
}
} // namespace algo