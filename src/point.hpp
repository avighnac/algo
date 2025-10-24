#pragma once

#include "traits.hpp"
#include <limits>
#include <ostream>

namespace algo {
class point {
public:
  int x, y;
  bool operator==(const point &other) const {
    return x == other.x && y == other.y;
  }
  bool is_invalid() const {
    return x == std::numeric_limits<int>::min() &&
           y == std::numeric_limits<int>::min();
  }
  friend std::ostream &operator<<(std::ostream &os, const point &p) {
    return os << '(' << p.x << ", " << p.y << ')';
  }
};

struct point_serializer {
  int cols;
  int operator()(const point &p) const { return p.x * cols + p.y; }
  point operator()(int i) const { return {i / cols, i % cols}; }
};

template <> constexpr bool is_serializable_v<point> = true;
template <> inline point invalid_value<point>() {
  return {std::numeric_limits<int>::min(), std::numeric_limits<int>::min()};
}
} // namespace algo