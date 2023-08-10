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

long long mod_pow(long long x, int n, int p) {
  long long ret = 1;
  while (n) {
    /*
  ∧,,,∧
(  ̳• · • ̳)
/    づ♡ I love you
    */
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

  explicit SegmentTree(const std::vector<Monoid>& a) : n(a.size()), sz(1) {
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
  static Monoid merge(const Monoid& a, const Monoid& b) {
    return std::min(a, b);
  }
};

template <typename T>
struct RangeMaximumQuery {
  using Monoid = T;
  static constexpr Monoid id() { return std::numeric_limits<Monoid>::lowest(); }
  static Monoid merge(const Monoid& a, const Monoid& b) {
    return std::max(a, b);
  }
};

template <typename T>
struct RangeSumQuery {
  using Monoid = T;
  static constexpr Monoid id() { return 0; }
  static Monoid merge(const Monoid& a, const Monoid& b) { return a + b; }
};

}  // namespace monoid

template <typename T>
struct LazySegmentTree {
  using Monoid = typename T::Monoid;
  using LazyMonoid = typename T::LazyMonoid;

  explicit LazySegmentTree(const int n)
      : LazySegmentTree(std::vector<Monoid>(n, T::m_id())) {}

  explicit LazySegmentTree(const std::vector<Monoid>& a)
      : n(a.size()), height(0) {
    while ((1 << height) < n) ++height;
    sz = 1 << height;
    lazy.assign(sz, T::lazy_id());
    data.assign(sz << 1, T::m_id());
    std::copy(a.begin(), a.end(), data.begin() + sz);
    for (int i = sz - 1; i > 0; --i) {
      data[i] = T::m_merge(data[i << 1], data[(i << 1) + 1]);
    }
  }

  void set(int idx, const Monoid val) {
    idx += sz;
    for (int i = height; i > 0; --i) {
      propagate(idx >> i);
    }
    data[idx] = val;
    for (int i = 1; i <= height; ++i) {
      const int current_idx = idx >> i;
      data[current_idx] =
          T::m_merge(data[current_idx << 1], data[(current_idx << 1) + 1]);
    }
  }

  void apply(int idx, const LazyMonoid val) {
    idx += sz;
    for (int i = height; i > 0; --i) {
      propagate(idx >> i);
    }
    data[idx] = T::apply(data[idx], val);
    for (int i = 1; i <= height; ++i) {
      const int current_idx = idx >> i;
      data[current_idx] =
          T::m_merge(data[current_idx << 1], data[(current_idx << 1) + 1]);
    }
  }

  void apply(int left, int right, const LazyMonoid val) {
    if (right <= left) return;
    left += sz;
    right += sz;
    const int ctz_left = __builtin_ctz(left);
    for (int i = height; i > ctz_left; --i) {
      propagate(left >> i);
    }
    const int ctz_right = __builtin_ctz(right);
    for (int i = height; i > ctz_right; --i) {
      propagate(right >> i);
    }
    for (int l = left, r = right; l < r; l >>= 1, r >>= 1) {
      if (l & 1) apply_sub(l++, val);
      if (r & 1) apply_sub(--r, val);
    }
    for (int i = left >> (ctz_left + 1); i > 0; i >>= 1) {
      data[i] = T::m_merge(data[i << 1], data[(i << 1) + 1]);
    }
    for (int i = right >> (ctz_right + 1); i > 0; i >>= 1) {
      data[i] = T::m_merge(data[i << 1], data[(i << 1) + 1]);
    }
  }

  Monoid get(int left, int right) {
    if (right <= left) return T::m_id();
    left += sz;
    right += sz;
    const int ctz_left = __builtin_ctz(left);
    for (int i = height; i > ctz_left; --i) {
      propagate(left >> i);
    }
    const int ctz_right = __builtin_ctz(right);
    for (int i = height; i > ctz_right; --i) {
      propagate(right >> i);
    }
    Monoid res_l = T::m_id(), res_r = T::m_id();
    for (; left < right; left >>= 1, right >>= 1) {
      if (left & 1) res_l = T::m_merge(res_l, data[left++]);
      if (right & 1) res_r = T::m_merge(data[--right], res_r);
    }
    return T::m_merge(res_l, res_r);
  }

  Monoid operator[](const int idx) {
    const int node = idx + sz;
    for (int i = height; i > 0; --i) {
      propagate(node >> i);
    }
    return data[node];
  }

  template <typename G>
  int find_right(int left, const G g) {
    if (left >= n) return n;
    left += sz;
    for (int i = height; i > 0; --i) {
      propagate(left >> i);
    }
    Monoid val = T::m_id();
    do {
      while (!(left & 1)) left >>= 1;
      Monoid nxt = T::m_merge(val, data[left]);
      if (!g(nxt)) {
        while (left < sz) {
          propagate(left);
          left <<= 1;
          nxt = T::m_merge(val, data[left]);
          if (g(nxt)) {
            val = nxt;
            ++left;
          }
        }
        return left - sz;
      }
      val = nxt;
      ++left;
    } while (__builtin_popcount(left) > 1);
    return n;
  }

  template <typename G>
  int find_left(int right, const G g) {
    if (right <= 0) return -1;
    right += sz;
    for (int i = height; i > 0; --i) {
      propagate((right - 1) >> i);
    }
    Monoid val = T::m_id();
    do {
      --right;
      while (right > 1 && (right & 1)) right >>= 1;
      Monoid nxt = T::m_merge(data[right], val);
      if (!g(nxt)) {
        while (right < sz) {
          propagate(right);
          right = (right << 1) + 1;
          nxt = T::m_merge(data[right], val);
          if (g(nxt)) {
            val = nxt;
            --right;
          }
        }
        return right - sz;
      }
      val = nxt;
    } while (__builtin_popcount(right) > 1);
    return -1;
  }

  const int n;
  int sz, height;
  std::vector<Monoid> data;
  std::vector<LazyMonoid> lazy;

  void apply_sub(const int idx, const LazyMonoid& val) {
    data[idx] = T::apply(data[idx], val);
    if (idx < sz) lazy[idx] = T::lazy_merge(lazy[idx], val);
  }

  void propagate(const int idx) {
    apply_sub(idx << 1, lazy[idx]);
    apply_sub((idx << 1) + 1, lazy[idx]);
    lazy[idx] = T::lazy_id();
  }
};

namespace monoid {

template <typename T>
struct RangeMinimumAndUpdateQuery {
  using Monoid = T;
  using LazyMonoid = T;
  static constexpr Monoid m_id() { return std::numeric_limits<Monoid>::max(); }
  static constexpr LazyMonoid lazy_id() {
    return std::numeric_limits<LazyMonoid>::max();
  }
  static Monoid m_merge(const Monoid& a, const Monoid& b) {
    return std::min(a, b);
  }
  static LazyMonoid lazy_merge(const LazyMonoid& a, const LazyMonoid& b) {
    return b == lazy_id() ? a : b;
  }
  static Monoid apply(const Monoid& a, const LazyMonoid& b) {
    return b == lazy_id() ? a : b;
  }
};

template <typename T>
struct RangeMaximumAndUpdateQuery {
  using Monoid = T;
  using LazyMonoid = T;
  static constexpr Monoid m_id() {
    return std::numeric_limits<Monoid>::lowest();
  }
  static constexpr LazyMonoid lazy_id() {
    return std::numeric_limits<LazyMonoid>::lowest();
  }
  static Monoid m_merge(const Monoid& a, const Monoid& b) {
    return std::max(a, b);
  }
  static LazyMonoid lazy_merge(const LazyMonoid& a, const LazyMonoid& b) {
    return b == lazy_id() ? a : b;
  }
  static Monoid apply(const Monoid& a, const LazyMonoid& b) {
    return b == lazy_id() ? a : b;
  }
};

template <typename T, T Inf>
struct RangeMinimumAndAddQuery {
  using Monoid = T;
  using LazyMonoid = T;
  static constexpr Monoid m_id() { return Inf; }
  static constexpr LazyMonoid lazy_id() { return 0; }
  static Monoid m_merge(const Monoid& a, const Monoid& b) {
    return std::min(a, b);
  }
  static LazyMonoid lazy_merge(const LazyMonoid& a, const LazyMonoid& b) {
    return a + b;
  }
  static Monoid apply(const Monoid& a, const LazyMonoid& b) { return a + b; }
};

template <typename T, T Inf>
struct RangeMaximumAndAddQuery {
  using Monoid = T;
  using LazyMonoid = T;
  static constexpr Monoid m_id() { return -Inf; }
  static constexpr LazyMonoid lazy_id() { return 0; }
  static Monoid m_merge(const Monoid& a, const Monoid& b) {
    return std::max(a, b);
  }
  static LazyMonoid lazy_merge(const LazyMonoid& a, const LazyMonoid& b) {
    return a + b;
  }
  static Monoid apply(const Monoid& a, const LazyMonoid& b) { return a + b; }
};

template <typename T>
struct RangeSumAndUpdateQuery {
  using Monoid = struct {
    T sum;
    int len;
  };
  using LazyMonoid = T;
  static std::vector<Monoid> init(const int n) {
    return std::vector<Monoid>(n, Monoid{0, 1});
  }
  static constexpr Monoid m_id() { return {0, 0}; }
  static constexpr LazyMonoid lazy_id() {
    return std::numeric_limits<LazyMonoid>::max();
  }
  static Monoid m_merge(const Monoid& a, const Monoid& b) {
    return Monoid{a.sum + b.sum, a.len + b.len};
  }
  static LazyMonoid lazy_merge(const LazyMonoid& a, const LazyMonoid& b) {
    return b == lazy_id() ? a : b;
  }
  static Monoid apply(const Monoid& a, const LazyMonoid& b) {
    return Monoid{b == lazy_id() ? a.sum : b * a.len, a.len};
  }
};

template <typename T>
struct RangeSumAndAddQuery {
  using Monoid = struct {
    T sum;
    int len;
  };
  using LazyMonoid = T;
  static std::vector<Monoid> init(const int n) {
    return std::vector<Monoid>(n, Monoid{0, 1});
  }
  static constexpr Monoid m_id() { return {0, 0}; }
  static constexpr LazyMonoid lazy_id() { return 0; }
  static Monoid m_merge(const Monoid& a, const Monoid& b) {
    return Monoid{a.sum + b.sum, a.len + b.len};
  }
  static LazyMonoid lazy_merge(const LazyMonoid& a, const LazyMonoid& b) {
    return a + b;
  }
  static Monoid apply(const Monoid& a, const LazyMonoid& b) {
    return Monoid{a.sum + b * a.len, a.len};
  }
};

}  // namespace monoid
void solve() {
  auto cal = [&](int x) -> int {
    int ans = 0;
    while (x) {
      ans += x % 10;
      x /= 10;
    }
    return ans;
  };
  int n, q;
  std::cin >> n >> q;
  std::vector<int> a(n);
  for (int& x : a) std::cin >> x;
  LazySegmentTree<monoid::RangeSumAndAddQuery<int>> tr(
      monoid::RangeSumAndAddQuery<int>::init(n + 1));
  for (int i = 0; i < q; i++) {
    int op;
    std::cin >> op;
    if (op == 1) {
      int l, r;
      std::cin >> l >> r;
      l--;
      tr.apply(l, r, 1);
    } else {
      int cur;
      std::cin >> cur;
      cur--;
      int cnt = tr.get(cur, cur + 1).sum;
      int nx = a[cur];
      // std::cout << cnt << '\n';
      while (cnt--) {
        nx = cal(nx);
        if (nx < 10) break;
      }
      std::cout << nx << '\n';
    }
  }
}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);

  int t = 1;
  std::cin >> t;
  while (t--) solve();
  return 0;
}