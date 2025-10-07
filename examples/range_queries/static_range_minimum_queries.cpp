#include "../../src/min_t.hpp"
#include "../../src/sparse_table.hpp"
#include <algorithm>
#include <iostream>

using namespace algo;

int main() {
  int n, q;
  std::cin >> n >> q;
  std::vector<int> a(n);
  for (int &i : a) {
    std::cin >> i;
  }
  sparse_table<min_t<int>> table(a);
  while (q--) {
    int l, r;
    std::cin >> l >> r;
    std::cout << table.query(l - 1, r - 1) << '\n';
  }
}