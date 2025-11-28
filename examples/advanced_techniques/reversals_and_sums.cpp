#include "../../src/lazy_treap.hpp"
#include <iostream>

using algo::lazy_treap;

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int n, m;
  std::cin >> n >> m;
  lazy_treap<int64_t> treap;
  for (int i = 0, x; i < n; ++i) {
    std::cin >> x;
    treap.insert(i, x);
  }

  while (m--) {
    int t, a, b;
    std::cin >> t >> a >> b;
    if (t == 1) {
      treap.reverse(a - 1, b - 1);
    } else {
      std::cout << treap.query(a - 1, b - 1) << '\n';
    }
  }
}