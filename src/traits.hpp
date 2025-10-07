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

template <typename T>
struct is_idempotent : std::false_type {};
template <typename T>
inline constexpr bool is_idempotent_v = is_idempotent<T>::value;
} // namespace algo
