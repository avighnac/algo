#include "../../src/algo.hpp"
#include <iostream>

using namespace algo;

int main() {
  int r, c;
  std::cin >> r >> c;
  graph<point, int> g(r * c, point_serializer{c});
  std::vector<std::string> map(r);
  for (auto &i : map) {
    std::cin >> i;
  }
  std::array<int, 4> dx = {1, -1, 0, 0}, dy = {0, 0, 1, -1};
  point start, end;
  for (int i = 0; i < r; ++i) {
    for (int j = 0; j < c; ++j) {
      for (int k = 0; k < 4; ++k) {
        if (map[i][j] == 'A') {
          start = {i, j};
        } else if (map[i][j] == 'B') {
          end = {i, j};
        }
        int x = i + dx[k], y = j + dy[k];
        if (x < 0 or x >= r or y < 0 or y >= c or map[x][y] == '#') {
          continue;
        }
        g.add_edge({i, j}, {x, y});
      }
    }
  }
  auto [dist, par] = g.bfs(start);
  if (dist[end] == -1) {
    std::cout << "NO\n";
    return 0;
  }
  std::cout << "YES\n";
  std::vector<point> path;
  for (point u = end; u != start; u = par[u]) {
    path.push_back(u);
  }
  path.push_back(start);
  std::reverse(path.begin(), path.end());
  std::cout << path.size() - 1 << '\n';
  for (int i = 1; i < path.size(); ++i) {
    if (path[i - 1].y != path[i].y) {
      std::cout << (path[i - 1].y > path[i].y ? 'L' : 'R');
    } else {
      std::cout << (path[i - 1].x > path[i].x ? 'U' : 'D');
    }
  }
  std::cout << '\n';
}