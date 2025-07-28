#include "../../src/algo.hpp"
#include <iostream>

using namespace algo;

int main() {
  int n, q;
  std::cin >> n >> q;
  std::vector<int> a(n + 1);
  segment_tree<int64_t> d(n + 2);
  for (int i = 1; i <= n; ++i) {
    std::cin >> a[i];
    d[i] = a[i] - a[i - 1];
  }
  while (q--) {
    int type;
    std::cin >> type;
    if (type == 1) {
      int a, b, u;
      std::cin >> a >> b >> u;
      d[a] += u, d[b + 1] -= u;
    } else {
      int k;
      std::cin >> k;
      std::cout << d.query(1, k) << '\n';
    }
  }
}