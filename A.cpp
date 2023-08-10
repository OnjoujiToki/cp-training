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

struct Mo {
  int n;
  std::vector<std::pair<int, int>> lr;

  explicit Mo(int n) : n(n) {}

  void add(int l, int r) { /* [l, r) */
    lr.emplace_back(l, r);
  }

  template <typename AL, typename AR, typename EL, typename ER, typename O>
  void build(const AL &add_left, const AR &add_right, const EL &erase_left,
             const ER &erase_right, const O &out) {
    int q = (int)lr.size();
    int bs = n / std::min<int>(n, sqrt(q));
    std::vector<int> ord(q);
    std::iota(std::begin(ord), std::end(ord), 0);
    std::sort(std::begin(ord), std::end(ord), [&](int a, int b) {
      int ablock = lr[a].first / bs, bblock = lr[b].first / bs;
      if (ablock != bblock) return ablock < bblock;
      return (ablock & 1) ? lr[a].second > lr[b].second
                          : lr[a].second < lr[b].second;
    });
    int l = 0, r = 0;
    for (auto idx : ord) {
      while (l > lr[idx].first) add_left(--l);
      while (r < lr[idx].second) add_right(r++);
      while (l < lr[idx].first) erase_left(l++);
      while (r > lr[idx].second) erase_right(--r);
      out(idx);
    }
  }

  template <typename A, typename E, typename O>
  void build(const A &add, const E &erase, const O &out) {
    build(add, add, erase, erase, out);
  }
};
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
using mint = ModInt<998244353>;
const int MOD = 998244353;
struct MComb {
  std::vector<mint> fact;
  std::vector<mint> inversed;
  MComb(int n) {  // O(n+log(mod))
    fact = std::vector<mint>(n + 1, 1);
    for (int i = 1; i <= n; i++) fact[i] = fact[i - 1] * mint(i);
    inversed = std::vector<mint>(n + 1);
    inversed[n] = fact[n] ^ (MOD - 2);
    for (int i = n - 1; i >= 0; i--)
      inversed[i] = inversed[i + 1] * mint(i + 1);
  }
  mint ncr(int n, int r) {
    if (n < r) return 0;
    return (fact[n] * inversed[r] * inversed[n - r]);
  }
  mint npr(int n, int r) { return (fact[n] * inversed[n - r]); }
  mint nhr(int n, int r) {
    assert(n + r - 1 < (int)fact.size());
    return ncr(n + r - 1, r);
  }
};

mint ncr(int n, int r) {
  mint res = 1;
  for (int i = n - r + 1; i <= n; i++) res *= i;
  for (int i = 1; i <= r; i++) res /= i;
  return res;
}
long long mod_pow(long long x, int n, int p) {
  long long ret = 1;
  while (n) {
    if (n & 1) (ret *= x) %= p;
    (x *= x) %= p;
    n >>= 1;
  }
  return ret;
}
namespace internal {

template <class E>
struct csr {
  std::vector<int> start;
  std::vector<E> elist;
  explicit csr(int n, const std::vector<std::pair<int, E>> &edges)
      : start(n + 1), elist(edges.size()) {
    for (auto e : edges) {
      start[e.first + 1]++;
    }
    for (int i = 1; i <= n; i++) {
      start[i] += start[i - 1];
    }
    auto counter = start;
    for (auto e : edges) {
      elist[counter[e.first]++] = e.second;
    }
  }
};

}  // namespace internal

struct scc_graph {
 public:
  explicit scc_graph(int n) : _n(n) {}

  int num_vertices() { return _n; }

  void add_edge(int from, int to) { edges.push_back({from, {to}}); }

  // @return pair of (# of scc, scc id)
  std::pair<int, std::vector<int>> scc_ids() {
    auto g = internal::csr<edge>(_n, edges);
    int now_ord = 0, group_num = 0;
    std::vector<int> visited, low(_n), ord(_n, -1), ids(_n);
    visited.reserve(_n);
    auto dfs = [&](auto self, int v) -> void {
      low[v] = ord[v] = now_ord++;
      visited.push_back(v);
      for (int i = g.start[v]; i < g.start[v + 1]; i++) {
        auto to = g.elist[i].to;
        if (ord[to] == -1) {
          self(self, to);
          low[v] = std::min(low[v], low[to]);
        } else {
          low[v] = std::min(low[v], ord[to]);
        }
      }
      if (low[v] == ord[v]) {
        while (true) {
          int u = visited.back();
          visited.pop_back();
          ord[u] = _n;
          ids[u] = group_num;
          if (u == v) break;
        }
        group_num++;
      }
    };
    for (int i = 0; i < _n; i++) {
      if (ord[i] == -1) dfs(dfs, i);
    }
    for (auto &x : ids) {
      x = group_num - 1 - x;
    }
    return {group_num, ids};
  }

  std::vector<std::vector<int>> scc() {
    auto ids = scc_ids();
    int group_num = ids.first;
    std::vector<int> counts(group_num);
    for (auto x : ids.second) counts[x]++;
    std::vector<std::vector<int>> groups(ids.first);
    for (int i = 0; i < group_num; i++) {
      groups[i].reserve(counts[i]);
    }
    for (int i = 0; i < _n; i++) {
      groups[ids.second[i]].push_back(i);
    }
    return groups;
  }

 private:
  int _n;
  struct edge {
    int to;
  };
  std::vector<std::pair<int, edge>> edges;
};

void solve() {
  int t;
  std::cin >> t;
  while (t--) {
    long long n, s, k;
    std::cin >> n >> s >> k;
    }
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  int t = 1;

  while (t--) solve();
  return 0;
}