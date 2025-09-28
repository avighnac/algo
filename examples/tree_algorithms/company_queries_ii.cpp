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
  auto lift = adj.lift(0);
  while (q--) {
    int u, v;
    std::cin >> u >> v;
    std::cout << lift.lca(u - 1, v - 1) + 1 << '\n';
  }
}