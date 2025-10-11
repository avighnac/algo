#include "../../src/lazy_segment_tree.hpp"
#include "../../src/mint.hpp"
#include <iostream>

using mint = algo::mint<998244353>;
using algo::lazy_segment_tree;

struct linear {
  mint a, b;
  linear() : a(1), b(0) {}
  linear(mint a, mint b) : a(a), b(b) {}
  linear operator+(const linear &other) const {
    return {other.a * a, other.a * b + other.b};
  }
};

struct combine {
  static void apply(mint &a, const linear &f, int len) {
    a = f.a * a + f.b * len;
  }
};

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int n, q;
  std::cin >> n >> q;

  lazy_segment_tree<mint, linear, combine> st(n);
  for (int i = 0, x; i < n; ++i) {
    std::cin >> x;
    st.apply(i, {0, x});
  }

  while (q--) {
    int type;
    std::cin >> type;
    if (type == 0) {
      int l, r, b, c;
      std::cin >> l >> r >> b >> c;
      st.apply(l, r - 1, {b, c});
    } else {
      int l, r;
      std::cin >> l >> r;
      std::cout << st.query(l, r - 1) << '\n';
    }
  }
}