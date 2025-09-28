#include "../../src/max_t.hpp"
#include "../../src/tree.hpp"
#include <iostream>

using namespace algo;

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  int n, q;
  std::cin >> n >> q;
  std::vector<int> a(n);
  for (int &i : a) {
    std::cin >> i;
  }
  tree<int> adj(n);
  for (int i = 0, u, v; i < n - 1; ++i) {
    std::cin >> u >> v;
    adj.add_edge(u - 1, v - 1);
  }
  auto hld = adj.hld<max_t<int>>(0);
  for (int i = 0; i < n; ++i) {
    hld[i] = a[i];
  }
  while (q--) {
    int type;
    std::cin >> type;
    if (type == 1) {
      int s, x;
      std::cin >> s >> x;
      hld[s - 1] = x;
    } else {
      int u, v;
      std::cin >> u >> v;
      std::cout << hld.query_nodes(u - 1, v - 1) << ' ';
    }
  }
  std::cout << '\n';
}