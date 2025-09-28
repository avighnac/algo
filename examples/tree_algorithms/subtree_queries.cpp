#include "../../src/tree.hpp"
#include <iostream>

using namespace algo;

int main() {
  int n, q;
  std::cin >> n >> q;
  std::vector<int64_t> a(n);
  for (int64_t &i : a) {
    std::cin >> i;
  }
  tree<int> adj(n);
  for (int i = 0, u, v; i < n - 1; ++i) {
    std::cin >> u >> v;
    adj.add_edge(u - 1, v - 1);
  }
  auto flat = adj.flatten<int64_t>(a, 0);
  while (q--) {
    int type, s, x;
    std::cin >> type >> s;
    if (type == 1) {
      std::cin >> x;
      flat[s - 1] = x;
    } else {
      std::cout << flat.query(s - 1) << '\n';
    }
  }
}