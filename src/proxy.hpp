#pragma once

#include <cstdint>
#include <iostream>
#include <type_traits>

namespace algo {
namespace internal {
template <typename T, typename Container>
struct getter {
  T operator()(Container *st, std::size_t i) const { return st->at(i); }
};
template <typename T, typename Container>
struct setter {
  void operator()(Container *st, std::size_t i, const T &x) const { st->set(i, x); }
};

template <typename Container, typename T, typename Getter = getter<T, Container>, typename Setter = setter<T, Container>>
struct proxy_ref {
  Container *st;
  std::size_t i;
  proxy_ref(Container *st, std::size_t i) : st(st), i(i) {}
  proxy_ref(const proxy_ref &) = default;
  static inline Getter get{};
  static inline Setter set{};

  operator T() const { return get(st, i); }

  proxy_ref &operator=(const T &x) { return set(st, i, x), *this; }
  proxy_ref &operator=(const proxy_ref &r) { return *this = T(r); }

  template <typename U>
    requires requires(T a, U b) { a + b; }
  proxy_ref &operator+=(const U &r) { return set(st, i, T(*this) + r), *this; }
  template <typename U>
    requires requires(T a, U b) { a - b; }
  proxy_ref &operator-=(const U &r) { return set(st, i, T(*this) - r), *this; }
  template <typename U>
    requires requires(T a, U b) { a * b; }
  proxy_ref &operator*=(const U &r) { return set(st, i, T(*this) * r), *this; }
  template <typename U>
    requires requires(T a, U b) { a / b; }
  proxy_ref &operator/=(const U &r) { return set(st, i, T(*this) / r), *this; }

  T operator++(int)
    requires std::is_arithmetic_v<T>
  {
    T old = *this;
    set(st, i, old + 1);
    return old;
  }
  T operator--(int)
    requires std::is_arithmetic_v<T>
  {
    T old = *this;
    set(st, i, old - 1);
    return old;
  }
  proxy_ref &operator++()
    requires std::is_arithmetic_v<T>
  { return set(st, i, T(*this) + 1), *this; }
  proxy_ref &operator--()
    requires std::is_arithmetic_v<T>
  { return set(st, i, T(*this) - 1), *this; }

  friend std::istream &operator>>(std::istream &is, proxy_ref &r) {
    T x;
    is >> x;
    r = x;
    return is;
  }
  friend std::ostream &operator<<(std::ostream &os, const proxy_ref &r) {
    return os << T(r);
  }

  friend T max(const proxy_ref &a, const proxy_ref &b) { return std::max<T>(T(a), T(b)); }
  friend T max(const proxy_ref &a, const T &b) { return std::max<T>(T(a), b); }
  friend T max(const T &a, const proxy_ref &b) { return std::max<T>(a, T(b)); }
};

template <typename reference, typename Container>
class iterator {
private:
  reference ref;

public:
  iterator(Container *st, std::size_t i) : ref(st, i) {}

  reference &operator*() { return ref; }
  const reference &operator*() const { return ref; }
  iterator &operator++() { return ++ref.i, *this; }
  iterator operator++(int) {
    iterator tmp = *this;
    ++(*this);
    return tmp;
  }

  bool operator==(const iterator &r) const { return ref.i == r.ref.i && ref.st == r.ref.st; }
  bool operator!=(const iterator &r) const { return !(*this == r); }
};
} // namespace internal
} // namespace algo