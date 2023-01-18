/* 💕💕💕💕💕
💗💗💗💗💗
  /)/)
( . .)
( づ💗

    💗💗💗  💗💗💗
  💗💗💗💗💗💗💗💗💗
  💗💗💗💗💗💗💗💗💗
    💗💗💗💗💗💗💗
      💗💗💗💗💗
        💗💗💗
          💗

*/

#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <chrono>
#include <cmath>
#include <complex>
#include <cstdint>
#include <cstring>
#include <ctime>
#include <deque>
#include <iomanip>
#include <iostream>
#include <map>
#include <numeric>
#include <queue>
#include <set>
#include <unordered_map>
#include <unordered_set>

void solve() {
  int n, m;
  std::cin >> n >> m;
  std::vector<std::vector<int>> exist(
      n);  // in which lines (top), has value of i
  std::vector<std::queue<int>> a(m);
  for (int i = 0; i < m; i++) {
    int k;
    std::cin >> k;
    for (int j = 0; j < k; j++) {
      int x;
      std::cin >> x;
      x--;
      a[i].push(x);
      if (j == 0) exist[x].push_back(i);
    }
  }
  std::queue<int> q;
  for (int i = 0; i < n; i++) {
    if (exist[i].size() == 2) {
      q.emplace(i);
    }
  }
  while(!q.empty()) {
    int u = q.front();
    q.pop();
    for (int v : exist[u]) {
      a[v].pop();
      if (!a[v].empty()) {
        exist[a[v].front()].push_back(v);
        if (exist[a[v].front()].size() == 2) {
          q.emplace(a[v].front());
        }
      }
    }
  }
  
  for (int i = 0; i < m; i++) {
    if (a[i].size()) {
      std::cout << "No" << '\n';
      return;
    }
  }
  std::cout << "Yes" << '\n';

}
int main() {
  std::cin.tie(nullptr);
  int t = 1;
  // std::cout << std::boolalpha;
  // std::cin >> t;
  while (t--) solve();
  return 0;
}