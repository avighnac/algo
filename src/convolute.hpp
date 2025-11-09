#pragma once

#include <bit>
#include <complex>
#include <cstdint>
#include <numbers>
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
} // namespace internal

std::vector<int64_t> convolute(const std::vector<int64_t> &a, const std::vector<int64_t> &b) {
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
  std::vector<int64_t> c(_n);
  for (int i = 0; i < _n; ++i) {
    c[i] = std::round(a_cmpl[i].real());
  }
  return c;
}
} // namespace algo