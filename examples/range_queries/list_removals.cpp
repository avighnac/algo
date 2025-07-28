#include "../../src/algo.hpp"
#include <iostream>

using namespace algo;

int main() {
  int n;
  std::cin >> n;
  std::vector<int> a(n);
  for (int &i : a) {
    std::cin >> i;
  }
  segment_tree<int> st(n, 1);
  for (int i = 0, x; i < n; ++i) {
    std::cin >> x;
    int idx = st.min_right(0, [&](int tot) { return tot < x; });
    std::cout << a[idx] << ' ';
    st[idx] -= 1;
  }
  std::cout << '\n';
}