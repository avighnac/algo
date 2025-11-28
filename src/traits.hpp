#pragma once

#include <type_traits>

namespace algo {
template <typename T>
struct is_idempotent : std::false_type {};
template <typename T>
inline constexpr bool is_idempotent_v = is_idempotent<T>::value;
} // namespace algo
