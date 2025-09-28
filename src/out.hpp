#pragma once

#if defined(__unix__) || defined(__unix) || defined(__APPLE__)
#include <unistd.h>
#else
#error "Fast output is not implemented for non-POSIX operating systems"
#endif

#include <array>
#include <charconv>
#include <cstring>
#include <string_view>

namespace algo {
template <int size = 1 << 20>
class out {
private:
  char base[size];
  char *begin;

public:
  out() : begin(base) {}
  ~out() { write(STDOUT_FILENO, base, begin - base); }

  template <std::unsigned_integral T>
  friend out &operator<<(out &out, T x) {
    char *p = out.begin;
    if (x == 0) {
      *p++ = '0';
    } else {
      char *q = p;
      do {
        *q++ = '0' + (x % 10);
        x /= 10;
      } while (x);
      std::reverse(p, q);
      p = q;
    }
    out.begin = p;
    return out;
  }

  friend out &operator<<(out &out, const char &x) {
    *out.begin++ = x;
    return out;
  }

  friend out &operator<<(out &out, std::string_view s) {
    std::memcpy(out.begin, s.data(), s.size());
    out.begin += s.size();
    return out;
  }
};

inline out<> cout;
} // namespace algo