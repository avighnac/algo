#include "../../src/algo.hpp"
#include <iostream>

using namespace algo;

int main() {
  int n, m;
  std::cin >> n >> m;
  segment_tree<max_t<int>> st(n);
  for (int i = 0, x; i < n; ++i) {
    std::cin >> x;
    st[i] = x;
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
