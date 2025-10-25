#include "../../src/internal_lazy_treap.hpp"
#include <iostream>

using algo::internal::lazy_treap;
 
int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
 
  int n, m;
  std::cin >> n >> m;
  lazy_treap<char, char, void> str;
  for (int i = 0; i < n; ++i) {
    char c;
    std::cin >> c;
    str.insert(i, c);
  }
 
  while (m--) {
    int a, b;
    std::cin >> a >> b;
    str.reverse(a - 1, b - 1);
  }
 
  for (int i = 0; i < n; ++i) {
    std::cout << str.at(i);
  }
  std::cout << '\n';
}