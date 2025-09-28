#include "../../src/tree.hpp"
#include <iostream>

using namespace algo;

int main() {
  int n, q;
  std::cin >> n >> q;
  tree<int> adj(n);
  for (int i = 0, u; i < n - 1; ++i) {
    std::cin >> u;
    adj.add_edge(i + 1, u - 1);
  }
  auto depth = adj.dep(0);
  auto lift = adj.lift(0);
  while (q--) {
    int x, k;
    std::cin >> x >> k;
    --x;
    std::cout << (depth[x] < k ? -2 : lift.ancestor(x, k)) + 1 << '\n';
  }
}