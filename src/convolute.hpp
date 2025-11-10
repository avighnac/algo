#pragma once

#include "mint.hpp"
#include <array>
#include <bit>
#include <complex>
#include <cstdint>
#include <numbers>
#include <type_traits>
#include <vector>

namespace algo {
namespace internal {
using complex = std::complex<double>;
constexpr inline double pi = std::numbers::pi;

unsigned bit_reverse(unsigned x, int bits) {
  unsigned r = 0;
  for (int i = 0; i < bits; ++i) {
    r = (r << 1) | (x & 1);
    x >>= 1;
  }
  return r;
}

void fft(std::vector<complex> &a, bool inv) {
  const int _n = a.size();
  const int w = std::bit_width(a.size()) - 1;
  std::vector<complex> b(_n);
  for (int i = 0; i < _n; ++i) {
    b[i] = a[bit_reverse(i, w)];
  }
  a = b;
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
void ntt(std::vector<mint<mod>> &a, bool inv) {
  const int _n = a.size();
  const int w = std::bit_width(a.size()) - 1;
  std::array<mint<mod>, ntt_traits<mod>::p + 1> roots;
  roots[ntt_traits<mod>::p] = ntt_traits<mod>::r;
  for (int i = roots.size() - 2; i >= 0; --i) {
    roots[i] = roots[i + 1] * roots[i + 1];
  }
  std::vector<mint<mod>> b(_n);
  for (int i = 0; i < _n; ++i) {
    b[i] = a[bit_reverse(i, w)];
  }
  a = b;
  for (uint32_t n = 2; n <= _n; n <<= 1) {
    mint<mod> base = roots[std::bit_width(n) - 1];
    if (inv) {
      base = base.inv();
    }
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
} // namespace algo