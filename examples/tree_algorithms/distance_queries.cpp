#include "../../src/tree.hpp"
#include <iostream>

using namespace algo;

int main() {
  int n, q;
  std::cin >> n >> q;
  tree<int> adj(n);
  for (int i = 0, u, v; i < n - 1; ++i) {
    std::cin >> u >> v;
    adj.add_edge(u - 1, v - 1);
  }
  auto lift = adj.lift(0);
  while (q--) {
    int u, v;
    std::cin >> u >> v;
    std::cout << lift.distance(u - 1, v - 1) << '\n';
  }
}