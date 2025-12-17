#pragma once

#include <cstdint>
#include <vector>

namespace algo {
template <typename T>
class matrix {
private:
  std::vector<std::vector<T>> m;

public:
  matrix(const std::vector<std::vector<T>> &m) : m(m) {}

  /// @brief Returns a unit matrix with our dimensions.
  /// @note Requires us to be a square matrix.
  matrix unit() const {
    matrix ans{std::vector(m.size(), std::vector<T>(m.size()))};
    for (std::size_t i = 0; i < m.size(); ++i) {
      ans[i][i] = 1;
    }
    return ans;
  }

  matrix pow(int64_t b) const {
    matrix ans = unit(), p = *this;
    while (b > 0) {
      if (b & 1) {
        ans *= p;
      }
      p *= p, b >>= 1;
    }
    return ans;
  }

  matrix &operator*=(const matrix &r) {
    matrix ans{std::vector(size(), std::vector<T>(r[0].size()))};
    for (std::size_t i = 0; i < size(); ++i) {
      for (std::size_t k = 0; k < r.size(); ++k) {
        for (std::size_t j = 0; j < r[0].size(); ++j) {
          ans[i][j] += m[i][k] * r[k][j];
        }
      }
    }
    return *this = std::move(ans);
  }
  matrix operator*(const matrix &r) { return matrix{*this} *= r; }

  std::vector<T> &operator[](std::size_t i) { return m[i]; }
  const std::vector<T> &operator[](std::size_t i) const { return m[i]; }
  std::size_t size() const { return m.size(); }
};
} // namespace algo