#include "../../src/treap.hpp"
#include <iostream>

using algo::treap;

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int n, m;
  std::cin >> n >> m;
  treap<char> str;
  for (int i = 0; i < n; ++i) {
    char c;
    std::cin >> c;
    str.insert(i, c);
  }

  while (m--) {
    int a, b;
    std::cin >> a >> b;
    --a, --b;
    str.merge(str.cut(a - 1, b - 1));
  }

  for (int i = 0; i < n; ++i) {
    std::cout << str.at(i);
  }
  std::cout << '\n';
}
