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
#include <random>
#include <set>
#include <unordered_map>
#include <unordered_set>
using namespace std;
template <int mod>
struct ModInt {
  int x;
  ModInt() : x(0) {}
  ModInt(long long y) : x(y >= 0 ? y % mod : (mod - (-y) % mod) % mod) {}
  ModInt &operator+=(const ModInt &p) {
    if ((x += p.x) >= mod) x -= mod;
    return *this;
  }
  ModInt &operator-=(const ModInt &p) {
    if ((x += mod - p.x) >= mod) x -= mod;
    return *this;
  }
  ModInt &operator*=(const ModInt &p) {
    x = (int)(1LL * x * p.x % mod);
    return *this;
  }
  ModInt &operator/=(const ModInt &p) {
    *this *= p.inverse();
    return *this;
  }
  ModInt &operator^=(long long p) {  // quick_pow here:3
    ModInt res = 1;
    for (; p; p >>= 1) {
      if (p & 1) res *= *this;
      *this *= *this;
    }
    return *this = res;
  }
  ModInt operator-() const { return ModInt(-x); }
  ModInt operator+(const ModInt &p) const { return ModInt(*this) += p; }
  ModInt operator-(const ModInt &p) const { return ModInt(*this) -= p; }
  ModInt operator*(const ModInt &p) const { return ModInt(*this) *= p; }
  ModInt operator/(const ModInt &p) const { return ModInt(*this) /= p; }
  ModInt operator^(long long p) const { return ModInt(*this) ^= p; }
  bool operator==(const ModInt &p) const { return x == p.x; }
  bool operator!=(const ModInt &p) const { return x != p.x; }
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
  friend std::ostream &operator<<(std::ostream &os, const ModInt<mod> &p) {
    return os << p.x;
  }
  friend std::istream &operator>>(std::istream &is, ModInt<mod> &a) {
    long long x;
    is >> x;
    a = ModInt<mod>(x);
    return (is);
  }
};

template <typename T>
struct SegmentTree {
  using Monoid = typename T::Monoid;

  explicit SegmentTree(int n) : SegmentTree(std::vector<Monoid>(n, T::id())) {}

  explicit SegmentTree(const std::vector<Monoid> &a) : n(a.size()), sz(1) {
    while (sz < n) sz <<= 1;
    data.assign(sz << 1, T::id());
    std::copy(a.begin(), a.end(), data.begin() + sz);
    for (int i = sz - 1; i > 0; --i) {
      data[i] = T::merge(data[i << 1], data[(i << 1) + 1]);
    }
  }

  void set(int idx, const Monoid val) {
    idx += sz;
    data[idx] = val;
    while (idx >>= 1)
      data[idx] = T::merge(data[idx << 1], data[(idx << 1) + 1]);
  }

  Monoid get(int left, int right) const {
    Monoid res_l = T::id(), res_r = T::id();
    for (left += sz, right += sz; left < right; left >>= 1, right >>= 1) {
      if (left & 1) res_l = T::merge(res_l, data[left++]);
      if (right & 1) res_r = T::merge(data[--right], res_r);
    }
    return T::merge(res_l, res_r);
  }

  Monoid operator[](const int idx) const { return data[idx + sz]; }

 private:
  const int n;
  int sz;  // sz + 原数组坐标 = 线段树里的编号，1 based
  std::vector<Monoid> data;
};

namespace monoid {

template <typename T>
struct RangeMinimumQuery {
  using Monoid = T;
  static constexpr Monoid id() { return std::numeric_limits<Monoid>::max(); }
  static Monoid merge(const Monoid &a, const Monoid &b) {
    return std::min(a, b);
  }
};

template <typename T>
struct RangeMaximumQuery {
  using Monoid = T;
  static constexpr Monoid id() { return std::numeric_limits<Monoid>::lowest(); }
  static Monoid merge(const Monoid &a, const Monoid &b) {
    return std::max(a, b);
  }
};

template <typename T>
struct RangeSumQuery {
  using Monoid = T;
  static constexpr Monoid id() { return 0; }
  static Monoid merge(const Monoid &a, const Monoid &b) { return a + b; }
};

}  // namespace monoid
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

  int sx, sy;
  int tx, ty;
  std::vector<std::string> grid(n);
  for (auto &c : grid) std::cin >> c;

  std::vector<std::vector<std::pair<int, int>>> connect(26);
  std::array<int, 5> dx = {0, 1, 0, -1, 0};
  std::vector<std::vector<int>> dist(n, std::vector<int>(m, 1e8));
  for (int i = 0; i < n; i++) {
    for (int j = 0; j < m; j++) {
      if (grid[i][j] == 'S') {
        sx = i, sy = j;
        grid[i][j] = '.';
      }
      if (grid[i][j] == 'G') {
        tx = i, ty = j;
        grid[i][j] = '.';
      }
      if (grid[i][j] >= 'a' and grid[i][j] <= 'z') {
        connect[grid[i][j] - 'a'].emplace_back(i, j);
      }
    }
  }
  std::cout << sx << ' ' << sy << '\n';
  std::queue<std::tuple<int, int, int>> q;
  q.emplace(sx, sy, 0);
  dist[sx][sy] = 0;
  while (!q.empty()) {
    auto [x, y, d] = q.front();
    q.pop();

    for (int k = 0; k < 4; k++) {
      int nx = x + dx[k], ny = y + dx[k + 1];
      if (nx < n and ny < m and nx >= 0 and ny >= 0 and grid[x][y] != '#') {
        if (dist[nx][ny] == 1e8) q.emplace(nx, ny, d + 1);
      }
    }
    if (grid[x][y] >= 'a' and grid[x][y] <= 'z') {
      for (auto [nx, ny] : connect[grid[x][y] - 'a']) {
        if (nx == x and ny == y) continue;
        if (dist[nx][ny] == 1e8) q.emplace(nx, ny, d + 1);
      }
    }
  }
  if (dist[tx][ty] == 1e8) dist[tx][ty] = -1;
  std::cout << dist[tx][ty] << '\n';
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  int t = 1;

  while (t--) solve();
  return 0;
}