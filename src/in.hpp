#pragma once

#if defined(__unix__) || defined(__unix) || defined(__APPLE__)
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#else
#error "Fast input is not implemented for non-POSIX operating systems"
#endif

#include <cctype>
#include <charconv>

namespace algo {
class in {
private:
  const char *base, *begin, *end;
  std::size_t size;

public:
  in() {
    struct stat st;
    fstat(STDIN_FILENO, &st);
    size = st.st_size;
    base = begin = (const char *)mmap(NULL, size, PROT_READ, MAP_PRIVATE, STDIN_FILENO, 0);
    end = begin + size;
  }

  ~in() {
    munmap((void *)base, size);
  }

  template <std::unsigned_integral T>
  friend in &operator>>(in &in, T &x) {
    while (*in.begin < '0' || *in.begin > '9') {
      ++in.begin;
    }
    x = 0;
    while (*in.begin >= '0' && *in.begin <= '9') {
      x = 10 * x + (*in.begin++ - '0');
    }
    return in;
  }

  template <std::signed_integral T>
  friend in &operator>>(in &in, T &x) {
    while ((*in.begin < '0' || *in.begin > '9') && *in.begin != '-') {
      ++in.begin;
    }
    bool positive = *in.begin != '-';
    in.begin += !positive;
    std::make_unsigned_t<T> u = 0;
    while (*in.begin >= '0' && *in.begin <= '9') {
      u = 10 * u + (*in.begin++ - '0');
    }
    if (positive) {
      x = static_cast<T>(u);
    } else {
      x = -static_cast<T>(u);
    }
    return in;
  }

  friend in &operator>>(in &in, char &c) {
    while (*in.begin <= ' ') {
      ++in.begin;
    }
    c = *in.begin++;
    return in;
  }

  friend in &operator>>(in &in, std::string &s) {
    while (*in.begin <= ' ') {
      ++in.begin;
    }
    const char *begin = in.begin;
    while (*in.begin > ' ') {
      ++in.begin;
    }
    s.assign(begin, in.begin);
    return in;
  }
};

inline in cin;
}; // namespace algo