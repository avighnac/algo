#include "../../src/algo.hpp"
#include <iostream>

using namespace algo;

int main() {
  int n, q;
  std::cin >> n >> q;
  segment_tree<int64_t> st(n + 1);
  for (int i = 1, x; i <= n; ++i) {
    std::cin >> x;
    st[i] = x;
  }
  while (q--) {
    int type;
    std::cin >> type;
    if (type == 1) {
      int k, u;
      std::cin >> k >> u;
      st[k] = u;
    } else {
      int a, b;
      std::cin >> a >> b;
      std::cout << st.query(a, b) << '\n';
    }
  }
}