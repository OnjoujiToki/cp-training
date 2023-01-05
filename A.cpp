#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <deque>
#include <iomanip>
#include <iostream>
#include <map>
#include <numeric>
#include <queue>
#include <random>
#include <set>
#include <sstream>
#include <stack>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>
template <typename T>
struct DSU {
  std::vector<T> f, siz;
  DSU(int n) : f(n), siz(n, 1) { std::iota(f.begin(), f.end(), 0); }
  T leader(T x) {
    while (x != f[x]) x = f[x] = f[f[x]];
    return x;
  }
  bool same(T x, T y) { return leader(x) == leader(y); }
  bool merge(T x, T y) {
    x = leader(x);
    y = leader(y);
    if (x == y) return false;
    siz[x] += siz[y];
    f[y] = x;
    return true;
  }
  T size(int x) { return siz[leader(x)]; }
};
void solve() {
  int n, m;
  std::cin >> n >> m;
  std::vector d(n, 0);

  DSU<int> dsu(n);
  for (int i = 0; i < m; i++) {
    int u, v;
    std::cin >> u >> v;
    u--, v--;
    if (dsu.same(u, v)) {
      std::cout << "No\n";
      return;
    }
    dsu.merge(u, v);
    d[u]++, d[v]++;
  }
  for (int i = 0; i < n; i++) {
    if (d[i] > 2) {
      std::cout << "No" << '\n';
      return;
    }
  }
  std::cout << "Yes\n";
}
int main() {
  std::cin.tie(nullptr);
  std::ios::sync_with_stdio(false);
  int t = 1;
  // std::cin >> t;
  while (t--) solve();
  // solve();
  return 0;
}