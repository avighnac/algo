#pragma once

namespace algo {
template <typename T>
class line {
private:
  T m, b;

public:
  line() = default;
  line(const T &m, const T &b) : m(m), b(b) {}

  T operator()(const T &x) const { return m * x + b; }
};
} // namespace algo