#pragma once

#include <type_traits>

namespace algo {
template <typename T>
constexpr bool is_serializable_v = false;
template <typename T>
T invalid_value();
template <typename T>
  requires std::is_arithmetic_v<T>
T invalid_value() {
  return -1;
}
} // namespace algo