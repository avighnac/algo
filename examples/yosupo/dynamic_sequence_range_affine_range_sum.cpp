#include "../../src/internal_lazy_treap.hpp"
#include "../../src/mint.hpp"
#include <iostream>

using algo::internal::lazy_treap;
using mint = algo::mint<998244353>;

struct linear {
  mint a, b;
  linear() : a(1), b(0) {}
  linear(mint a, mint b) : a(a), b(b) {}
  linear operator+(const linear &other) const {
    return {other.a * a, other.a * b + other.b};
  }
};

struct traits {
  static void apply(mint &a, const linear &f, int len) {
    a = f.a * a + f.b * len;
  }
};

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int n, q;
  std::cin >> n >> q;
  lazy_treap<mint, linear, traits> treap;
  for (int i = 0, x; i < n; ++i) {
    std::cin >> x;
    treap.insert(i, x);
  }

  while (q--) {
    int type;
    std::cin >> type;
    if (type == 0) {
      int i, x;
      std::cin >> i >> x;
      treap.insert(i, x);
    } else if (type == 1) {
      int i;
      std::cin >> i;
      treap.erase(i);
    } else if (type == 2) {
      int l, r;
      std::cin >> l >> r;
      treap.reverse(l, r - 1);
    } else if (type == 3) {
      int l, r, b, c;
      std::cin >> l >> r >> b >> c;
      treap.apply(l, r - 1, {b, c});
    } else {
      int l, r;
      std::cin >> l >> r;
      std::cout << treap.query(l, r - 1) << '\n';
    }
  }
}