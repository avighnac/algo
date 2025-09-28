#include "../../src/tree.hpp"
#include <iostream>

using namespace algo;

int main() {
  int n;
  std::cin >> n;
  tree<int> adj(n);
  for (int i = 1, u; i < n; ++i) {
    std::cin >> u;
    adj.add_edge(i, u - 1);
  }
  std::vector<int> sub(n);
  adj.dfs([&](int u, int i) { sub[u] += sub[i] + 1; }, 0);
  for (int &i : sub) {
    std::cout << i << ' ';
  }
  std::cout << '\n';
}