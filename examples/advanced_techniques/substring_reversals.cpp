#include "../../src/lazy_treap.hpp"
#include <iostream>

using algo::lazy_treap;

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int n, m;
  std::cin >> n >> m;
  lazy_treap<char> str(n);
  for (auto &i : str) {
    std::cin >> i;
  }

  while (m--) {
    int a, b;
    std::cin >> a >> b;
    str.reverse(a - 1, b - 1);
  }

  for (int i = 0; i < n; ++i) {
    std::cout << str[i];
  }
  std::cout << '\n';
}