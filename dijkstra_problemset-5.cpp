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
#include <vector>

template <typename T1, typename T2>
std::ostream &operator<<(std::ostream &os, const std::pair<T1, T2> &p) {
  os << p.first << " " << p.second;
  return os;
}

template <typename T1, typename T2>
std::istream &operator>>(std::istream &is, std::pair<T1, T2> &p) {
  is >> p.first >> p.second;
  return is;
}

template <typename T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &v) {
  for (int i = 0; i < (int)v.size(); i++) {
    os << v[i] << (i + 1 != (int)v.size() ? " " : "");
  }
  return os;
}

template <typename T>
std::istream &operator>>(std::istream &is, std::vector<T> &v) {
  for (T &in : v) is >> in;
  return is;
}
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
using mint = ModInt<1000000007>;
// using mint = ModInt<998244353>;
const int MOD = 998244353;
template <typename T>
std::pair<std::vector<T>, std::vector<T>> get_prime_factor_with_kinds(T n) {
  std::vector<T> prime_factors;
  std::vector<T> cnt;  // number of i_th factor
  for (T i = 2; i * i <= n; i++) {
    if (n % i == 0) {
      prime_factors.push_back(i);
      cnt.push_back(0);
      while (n % i == 0) n /= i, cnt[(int)prime_factors.size() - 1]++;
    }
  }
  if (n > 1) prime_factors.push_back(n), cnt.push_back(1);
  assert(prime_factors.size() == cnt.size());
  return {prime_factors, cnt};
}

/*
Range Add Range Sum

struct S {
  long long value;
  int size;
};
using F = long long;

S op(S a, S b) { return {a.value + b.value, a.size + b.size}; }
S e() { return {0, 0}; }
S mapping(F f, S x) { return {x.value + f * x.size, x.size}; }
F composition(F f, F g) { return f + g; }
F id() { return 0; }

Ranged affine range sum
struct S {
  mint sum;
  int len;
};
struct F {
  mint a;
  mint b;
};

S op(S l, S r) { return {l.sum + r.sum, l.len + r.len}; }
S e() { return {0, 0}; }
S mapping(F lazy, S node) {
  return {node.sum * lazy.a + node.len * lazy.b, node.len};
}
F composition(F f, F g) {  // f is son, g is parent
  return {f.a * g.a, g.b * f.a + f.b};
}

F id() { return {1, 0}; }

*/

// #pragma once

#include <bit>
#include <cassert>
#include <functional>
#include <vector>

namespace atcoder {

namespace internal {

#if __cplusplus >= 202002L

using std::bit_ceil;

#else

// @return same with std::bit::bit_ceil
unsigned int bit_ceil(unsigned int n) {
  unsigned int x = 1;
  while (x < (unsigned int)(n)) x *= 2;
  return x;
}

#endif

// @param n `1 <= n`
// @return same with std::bit::countr_zero
int countr_zero(unsigned int n) {
#ifdef _MSC_VER
  unsigned long index;
  _BitScanForward(&index, n);
  return index;
#else
  return __builtin_ctz(n);
#endif
}

// @param n `1 <= n`
// @return same with std::bit::countr_zero
constexpr int countr_zero_constexpr(unsigned int n) {
  int x = 0;
  while (!(n & (1 << x))) x++;
  return x;
}

}  // namespace internal

}  // namespace atcoder

namespace atcoder {

#if __cplusplus >= 201703L

template <class S, auto op, auto e, class F, auto mapping, auto composition,
          auto id>
struct lazy_segtree {
  static_assert(std::is_convertible_v<decltype(op), std::function<S(S, S)>>,
                "op must work as S(S, S)");
  static_assert(std::is_convertible_v<decltype(e), std::function<S()>>,
                "e must work as S()");
  static_assert(
      std::is_convertible_v<decltype(mapping), std::function<S(F, S)>>,
      "mapping must work as F(F, S)");
  static_assert(
      std::is_convertible_v<decltype(composition), std::function<F(F, F)>>,
      "compostiion must work as F(F, F)");
  static_assert(std::is_convertible_v<decltype(id), std::function<F()>>,
                "id must work as F()");

#else

template <class S, S (*op)(S, S), S (*e)(), class F, S (*mapping)(F, S),
          F (*composition)(F, F), F (*id)()>
struct lazy_segtree {

#endif

 public:
  lazy_segtree() : lazy_segtree(0) {}
  explicit lazy_segtree(int n) : lazy_segtree(std::vector<S>(n, e())) {}
  explicit lazy_segtree(const std::vector<S> &v) : _n(int(v.size())) {
    size = (int)internal::bit_ceil((unsigned int)(_n));
    log = internal::countr_zero((unsigned int)size);
    d = std::vector<S>(2 * size, e());
    lz = std::vector<F>(size, id());
    for (int i = 0; i < _n; i++) d[size + i] = v[i];
    for (int i = size - 1; i >= 1; i--) {
      update(i);
    }
  }

  void set(int p, S x) {
    assert(0 <= p && p < _n);
    p += size;
    for (int i = log; i >= 1; i--) push(p >> i);
    d[p] = x;
    for (int i = 1; i <= log; i++) update(p >> i);
  }

  S get(int p) {
    assert(0 <= p && p < _n);
    p += size;
    for (int i = log; i >= 1; i--) push(p >> i);
    return d[p];
  }

  S prod(int l, int r) {
    assert(0 <= l && l <= r && r <= _n);
    if (l == r) return e();

    l += size;
    r += size;

    for (int i = log; i >= 1; i--) {
      if (((l >> i) << i) != l) push(l >> i);
      if (((r >> i) << i) != r) push((r - 1) >> i);
    }

    S sml = e(), smr = e();
    while (l < r) {
      if (l & 1) sml = op(sml, d[l++]);
      if (r & 1) smr = op(d[--r], smr);
      l >>= 1;
      r >>= 1;
    }

    return op(sml, smr);
  }

  S all_prod() { return d[1]; }

  void apply(int p, F f) {
    assert(0 <= p && p < _n);
    p += size;
    for (int i = log; i >= 1; i--) push(p >> i);
    d[p] = mapping(f, d[p]);
    for (int i = 1; i <= log; i++) update(p >> i);
  }
  void apply(int l, int r, F f) {
    assert(0 <= l && l <= r && r <= _n);
    if (l == r) return;

    l += size;
    r += size;

    for (int i = log; i >= 1; i--) {
      if (((l >> i) << i) != l) push(l >> i);
      if (((r >> i) << i) != r) push((r - 1) >> i);
    }

    {
      int l2 = l, r2 = r;
      while (l < r) {
        if (l & 1) all_apply(l++, f);
        if (r & 1) all_apply(--r, f);
        l >>= 1;
        r >>= 1;
      }
      l = l2;
      r = r2;
    }

    for (int i = 1; i <= log; i++) {
      if (((l >> i) << i) != l) update(l >> i);
      if (((r >> i) << i) != r) update((r - 1) >> i);
    }
  }

  template <bool (*g)(S)>
  int max_right(int l) {
    return max_right(l, [](S x) { return g(x); });
  }
  template <class G>
  int max_right(int l, G g) {
    assert(0 <= l && l <= _n);
    assert(g(e()));
    if (l == _n) return _n;
    l += size;
    for (int i = log; i >= 1; i--) push(l >> i);
    S sm = e();
    do {
      while (l % 2 == 0) l >>= 1;
      if (!g(op(sm, d[l]))) {
        while (l < size) {
          push(l);
          l = (2 * l);
          if (g(op(sm, d[l]))) {
            sm = op(sm, d[l]);
            l++;
          }
        }
        return l - size;
      }
      sm = op(sm, d[l]);
      l++;
    } while ((l & -l) != l);
    return _n;
  }

  template <bool (*g)(S)>
  int min_left(int r) {
    return min_left(r, [](S x) { return g(x); });
  }
  template <class G>
  int min_left(int r, G g) {
    assert(0 <= r && r <= _n);
    assert(g(e()));
    if (r == 0) return 0;
    r += size;
    for (int i = log; i >= 1; i--) push((r - 1) >> i);
    S sm = e();
    do {
      r--;
      while (r > 1 && (r % 2)) r >>= 1;
      if (!g(op(d[r], sm))) {
        while (r < size) {
          push(r);
          r = (2 * r + 1);
          if (g(op(d[r], sm))) {
            sm = op(d[r], sm);
            r--;
          }
        }
        return r + 1 - size;
      }
      sm = op(d[r], sm);
    } while ((r & -r) != r);
    return 0;
  }

 private:
  int _n, size, log;
  std::vector<S> d;
  std::vector<F> lz;

  void update(int k) { d[k] = op(d[2 * k], d[2 * k + 1]); }
  void all_apply(int k, F f) {
    d[k] = mapping(f, d[k]);
    if (k < size) lz[k] = composition(f, lz[k]);
  }
  void push(int k) {
    all_apply(2 * k, lz[k]);
    all_apply(2 * k + 1, lz[k]);
    lz[k] = id();
  }
};

}  // namespace atcoder

using S = long long;
using F = long long;

const S INF = 8e18;

S op(S a, S b) { return std::max(a, b); }
S e() { return -INF; }
S mapping(F f, S x) { return f + x; }
F composition(F f, F g) { return f + g; }
F id() { return 0; }

void solve() {
  int n, m;
  std::cin >> n >> m;
  std::vector<std::vector<std::pair<int, int>>> g(n);
  for (int i = 0; i < m; i++) {
    char u, v;
    int w;
    std::cin >> u >> v >> w;
    u -= 'A';
    v -= 'A';
    g[u].emplace_back(w, v);
    g[v].emplace_back(w, u);
  }
  std::vector<int> dist(n, INF);
  std::vector<int> pre(n, -1);
  std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>,
                      std::greater<>>
      pq;

  dist[6] = 0;
  pq.emplace(0, 6);
  std::cout << "start from G\n";
  std::cout << "push pair(0, G) into min_heap, initialize dist[G] = 0\n";
  std::cout << '\n';
  int cnt = 0;
  while (!pq.empty()) {
    auto [d, u] = pq.top();
    pq.pop();
    std::cout << "iteration: " << cnt << " pop pair(" << d << ", "
              << char(u + 'A') << ") from min_heap\n";
    cnt++;
    if (d > dist[u]) {
      std::cout << "d > dist[u], continue(this vertex has been already "
                   "relaxed)\n";
      continue;
    }

    for (auto [w, v] : g[u]) {
      std::cout << "-for each edge(" << char(u + 'A') << ", " << char(v + 'A')
                << ") with weight " << w << '\n';
      if (dist[v] > d + w) {
        std::cout << "--dist[" << char(v + 'A') << "] > " << d + w
                  << ", relax dist[" << char(v + 'A') << "] = " << d + w
                  << '\n';
        dist[v] = d + w;
        pre[v] = u;
        pq.emplace(dist[v], v);
        std::cout << "--push pair(" << dist[v] << ", " << char(v + 'A')
                  << ") into min_heap\n";
      }
    }
    std::cout << '\n';
  }

  std::cout << dist << '\n';
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  int t = 1;
  // std::cin >> t;

  while (t--) {
    solve();
  }
}
