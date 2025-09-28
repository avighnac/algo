#include "../../src/tree.hpp"
#include <iostream>

using namespace algo;

int main() {
  int n;
  std::cin >> n;
  tree<int> adj(n);
  for (int i = 0, u, v; i < n - 1; ++i) {
    std::cin >> u >> v;
    adj.add_edge(u - 1, v - 1);
  }
  auto d1 = adj.dep(0);
  auto d2 = adj.dep(std::max_element(d1.begin(), d1.end()) - d1.begin());
  auto d3 = adj.dep(std::max_element(d2.begin(), d2.end()) - d2.begin());
  for (int i = 0; i < n; ++i) {
    std::cout << std::max(d2[i], d3[i]) << ' ';
  }
  std::cout << '\n';
}