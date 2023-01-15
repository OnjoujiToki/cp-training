#include <algorithm>
#include <array>
#include <bit>
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
long long mod_pow(long long x, int n, int p) {
  long long ret = 1;
  while (n) {
    if (n & 1) (ret *= x) %= p;
    (x *= x) %= p;
    n >>= 1;
  }
  return ret;
}

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
  int n;
  std::cin >> n;
  std::vector<int> a(n + 1);
  for (int i = 1; i <= n; i++) {
    std::cin >> a[i];
  }
  n *= 2;
  for (int i = 1; i <= n / 2; i++) {
    a.push_back(a[i]);
  }

  std::vector<std::vector<int>> dp(n + 1, std::vector<int>(2));
  for (int i = 1; i <= n; i++) {
    for (int j = 1; j < i; j++) {
      if (j < n / 2) dp[i][0] = std::max(dp[i][0], dp[j][1] + a[i - j]);
      dp[i][0] = std::max(dp[i][0], dp[j][1] + a[i - j]);
    }
    if (i) dp[i][1] = std::max(dp[i - 1][0], dp[i - 1][1]);
  }
  std::cout << std::max(dp[n][0], dp[n][1]);

}
int main() {
  std::cin.tie(nullptr);
  int t = 1;

  while (t--) solve();
  // solve();
  return 0;
}