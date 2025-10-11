#include "../../src/lazy_segment_tree.hpp"
#include "../../src/min_t.hpp"
#include <iostream>

using namespace algo;

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int n, q;
  std::cin >> n >> q;

  lazy_segment_tree<min_t<int64_t>, int64_t> st(n, 0);
  for (int i = 0, x; i < n; ++i) {
    std::cin >> x;
    st.apply(i, x);
  }

  while (q--) {
    int type, l, r, x;
    std::cin >> type >> l >> r;
    if (type == 0) {
      std::cin >> x;
      st.apply(l, r - 1, x);
    } else {
      std::cout << st.query(l, r - 1) << '\n';
    }
  }
}