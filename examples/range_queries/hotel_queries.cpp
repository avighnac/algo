#include "../../src/algo.hpp"
#include <iostream>

using namespace algo;

int main() {
  int n, m;
  std::cin >> n >> m;
  segment_tree<int, std::greater<>> st(n);
  for (auto &i : st) {
    std::cin >> i;
  }
  while (m--) {
    int x;
    std::cin >> x;
    int i = st.min_right(0, [&](int max) { return max < x; });
    std::cout << (i == n ? -1 : i) + 1 << ' ';
    if (i != n) {
      st[i] -= x;
    }
  }
  std::cout << '\n';
}
