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
#include <map>
#include <numeric>
#include <queue>
#include <random>
#include <set>
#include <unordered_map>
#include <unordered_set>

template <int mod>
struct ModInt {
  int x;
  ModInt() : x(0) {}
  ModInt(long long y) : x(y >= 0 ? y % mod : (mod - (-y) % mod) % mod) {}
  ModInt& operator+=(const ModInt& p) {
    if ((x += p.x) >= mod) x -= mod;
    return *this;
  }
  ModInt& operator-=(const ModInt& p) {
    if ((x += mod - p.x) >= mod) x -= mod;
    return *this;
  }
  ModInt& operator*=(const ModInt& p) {
    x = (int)(1LL * x * p.x % mod);
    return *this;
  }
  ModInt& operator/=(const ModInt& p) {
    *this *= p.inverse();
    return *this;
  }
  ModInt& operator^=(long long p) {  // quick_pow here:3
    ModInt res = 1;
    for (; p; p >>= 1) {
      if (p & 1) res *= *this;
      *this *= *this;
    }
    return *this = res;
  }
  ModInt operator-() const { return ModInt(-x); }
  ModInt operator+(const ModInt& p) const { return ModInt(*this) += p; }
  ModInt operator-(const ModInt& p) const { return ModInt(*this) -= p; }
  ModInt operator*(const ModInt& p) const { return ModInt(*this) *= p; }
  ModInt operator/(const ModInt& p) const { return ModInt(*this) /= p; }
  ModInt operator^(long long p) const { return ModInt(*this) ^= p; }
  bool operator==(const ModInt& p) const { return x == p.x; }
  bool operator!=(const ModInt& p) const { return x != p.x; }
  explicit operator int() const { return x; }  // added by QCFium
  ModInt operator=(const int p) {
    x = p;
    return ModInt(*this);
  }  // added by QCFium
  ModInt inverse() const {
    int a = x, b = mod, u = 1, v = 0, t;
    while (b > 0) {
      t = a / b;
      a -= t * b;
      std::swap(a, b);
      u -= t * v;
      std::swap(u, v);
    }
    return ModInt(u);
  }
  friend std::ostream& operator<<(std::ostream& os, const ModInt<mod>& p) {
    return os << p.x;
  }
  friend std::istream& operator>>(std::istream& is, ModInt<mod>& a) {
    long long x;
    is >> x;
    a = ModInt<mod>(x);
    return (is);
  }
};

int solve() {
  int n, m;
  std::cin >> n >> m;
  std::string s;
  std::cin >> s;
  std::vector<int> ind(n);
  std::vector<std::vector<int>> g(n);
  for (int i = 0; i < m; i++) {
    int u, v;
    std::cin >> u >> v;
    u--, v--;
    g[u].push_back(v);
    ind[v]++;
  }
  std::vector dp(n, std::vector<int>(26, 0));

  std::queue<int> q;
  for (int i = 0; i < n; i++) {
    if (!ind[i]) q.emplace(i), dp[i][s[i] - 'a'] = 1;
  }
  int ans = 0, cnt = 0;
  while (!q.empty()) {
    int u = q.front();
    cnt += 1;

    q.pop();
    for (int v : g[u]) {
      if (--ind[v] == 0) {
        q.emplace(v);
      }
      for (int i = 0; i < 26; i++) {
        if (i == (s[v] - 'a'))
          dp[v][i] = std::max(dp[v][i], dp[u][i] + 1);
        else
          dp[v][i] = std::max(dp[v][i], dp[u][i]);
      }
    }
  }
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < 26; j++) {
      ans = std::max(ans, dp[i][j]);
    }
  }
  if (cnt != n) return -1;
  return ans;
}
int main() {
  // std::cin >> t;
  std::cout << solve();
}
