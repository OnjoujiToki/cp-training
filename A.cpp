
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
#include <iterator>
#include <list>
#include <map>
#include <numeric>
#include <queue>
#include <random>
#include <set>
#include <stack>
#include <unordered_map>
#include <unordered_set>
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
int main() {
  int n;
  std::cin >> n;
  std::vector<int> h(n);
  for (int i = 0; i < n; ++i) std::cin >> h[i];
  std::vector<int> stk;
  std::vector<int> ans(n);
  for (int i = n - 1; i >= 0; i--) {
    while (!stk.empty() && h[stk.back()] < h[i]) {
      stk.pop_back();
    }
    // for (int x : stk) std::cout << x << ' ';

    stk.push_back(i);
  }
  for (int x : ans) std::cout << x << ' ';
}
