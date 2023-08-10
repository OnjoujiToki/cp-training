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

long long mod_pow(long long x, int n, int p) {
  long long ret = 1;
  while (n) {
    if (n & 1) (ret *= x) %= p;
    (x *= x) %= p;
    n >>= 1;
  }
  return ret;
}
std::pair<std::vector<long long>, std::vector<int>> get_prime_factor_with_kinds(
    long long n) {
  std::vector<long long> prime_factors;
  std::vector<int> cnt;  // number of i_th factor
  for (long long i = 2; i <= sqrt(n); i++) {
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
using mint = ModInt<1000000007>;
// using mint = ModInt<998244353>;

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
void solve() {
  int n, k;
  std::cin >> n >> k;
  std::set<int> s;
  std::vector<int> a(n);
  for (int &x : a) {
    std::cin >> x;
    s.insert(x);
  }
  if (s.size() > k) {
    std::cout << -1 << '\n';
    return;
  }

  std::vector<int> ans;
  for (int i = 0; i < n; i++) {
    for (int x : s) {
      ans.push_back(x);
    }
    ans.push_back(1);
  }
  int m = ans.size();
  std::cout << m << '\n';
  for (int x : ans) std::cout << x << ' ';
  std::cout << '\n';
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int t = 1;
  std::cin >> t;
  while (t--) solve();
  return 0;
}