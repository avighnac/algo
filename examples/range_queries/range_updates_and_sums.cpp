#include "../../src/lazy_segment_tree.hpp"
#include <iostream>

using namespace algo;

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int n, q;
  std::cin >> n >> q;

  lazy_segment_tree<int64_t> st(n);
  for (auto &i : st) {
    std::cin >> i;
  }

  while (q--) {
    int type;
    std::cin >> type;
    if (type == 1) {
      int a, b, x;
      std::cin >> a >> b >> x;
      st.add(a - 1, b - 1, x);
    } else if (type == 2) {
      int a, b, x;
      std::cin >> a >> b >> x;
      st.set(a - 1, b - 1, x);
    } else {
      int a, b;
      std::cin >> a >> b;
      std::cout << st.query(a - 1, b - 1) << '\n';
    }
  }
}