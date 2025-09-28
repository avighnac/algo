#include "../../src/algo.hpp"
#include <iostream>

using namespace algo;

int main() {
  int n, m;
  std::cin >> n >> m;
 
  generic_graph<int, int> g(n + 1);
  for (int i = 0, u, v; i < m; ++i) {
    std::cin >> u >> v;
    g.add_edge(u, v);
    g.add_edge(v, u);
  }
 
  auto [dist, par] = g.bfs(1);
  if (dist[n] == -1) {
    std::cout << "IMPOSSIBLE\n";
    return 0;
  }
  std::vector<int> path;
  for (int u = n; u != 1; u = par[u]) {
    path.push_back(u);
  }
  path.push_back(1);
  std::reverse(path.begin(), path.end());
  
  std::cout << path.size() << '\n';
  for (int &i : path) {
    std::cout << i << ' ';
  }
}