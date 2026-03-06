#include <algorithm>
#include <bit>
#include <bitset>
#include <cassert>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <functional>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <numeric>
#include <optional>
#include <queue>
#include <ranges>
#include <set>
#include <sstream>
#include <stack>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

#ifdef _MSC_VER
#include <intrin.h>
#endif
#include <iomanip>
#include <random>

namespace internal {

// @param m `1 <= m`
// @return x mod m
constexpr long long safe_mod(long long x, long long m) {
  x %= m;
  if (x < 0) x += m;
  return x;
}

// Fast modular multiplication by barrett reduction
// Reference: https://en.wikipedia.org/wiki/Barrett_reduction
// NOTE: reconsider after Ice Lake
struct barrett {
  unsigned int _m;
  unsigned long long im;

  // @param m `1 <= m < 2^31`
  explicit barrett(unsigned int m)
      : _m(m), im((unsigned long long)(-1) / m + 1) {}

  // @return m
  unsigned int umod() const { return _m; }

  // @param a `0 <= a < m`
  // @param b `0 <= b < m`
  // @return `a * b % m`
  unsigned int mul(unsigned int a, unsigned int b) const {
    // [1] m = 1
    // a = b = im = 0, so okay

    // [2] m >= 2
    // im = ceil(2^64 / m)
    // -> im * m = 2^64 + r (0 <= r < m)
    // let z = a*b = c*m + d (0 <= c, d < m)
    // a*b * im = (c*m + d) * im = c*(im*m) + d*im = c*2^64 + c*r + d*im
    // c*r + d*im < m * m + m * im < m * m + 2^64 + m <= 2^64 + m * (m + 1) <
    // 2^64 * 2
    // ((ab * im) >> 64) == c or c + 1
    unsigned long long z = a;
    z *= b;
#ifdef _MSC_VER
    unsigned long long x;
    _umul128(z, im, &x);
#else
    unsigned long long x =
        (unsigned long long)(((unsigned __int128)(z)*im) >> 64);
#endif
    unsigned int v = (unsigned int)(z - x * _m);
    if (_m <= v) v += _m;
    return v;
  }
};

// @param n `0 <= n`
// @param m `1 <= m`
// @return `(x ** n) % m`
constexpr long long pow_mod_constexpr(long long x, long long n, int m) {
  if (m == 1) return 0;
  unsigned int _m = (unsigned int)(m);
  unsigned long long r = 1;
  unsigned long long y = safe_mod(x, m);
  while (n) {
    if (n & 1) r = (r * y) % _m;
    y = (y * y) % _m;
    n >>= 1;
  }
  return r;
}

// Reference:
// M. Forisek and J. Jancina,
// Fast Primality Testing for Integers That Fit into a Machine Word
// @param n `0 <= n`
constexpr bool is_prime_constexpr(int n) {
  if (n <= 1) return false;
  if (n == 2 || n == 7 || n == 61) return true;
  if (n % 2 == 0) return false;
  long long d = n - 1;
  while (d % 2 == 0) d /= 2;
  constexpr long long bases[3] = {2, 7, 61};
  for (long long a : bases) {
    long long t = d;
    long long y = pow_mod_constexpr(a, t, n);
    while (t != n - 1 && y != 1 && y != n - 1) {
      y = y * y % n;
      t <<= 1;
    }
    if (y != n - 1 && t % 2 == 0) {
      return false;
    }
  }
  return true;
}
template <int n>
constexpr bool is_prime = is_prime_constexpr(n);

// @param b `1 <= b`
// @return pair(g, x) s.t. g = gcd(a, b), xa = g (mod b), 0 <= x < b/g
constexpr std::pair<long long, long long> inv_gcd(long long a, long long b) {
  a = safe_mod(a, b);
  if (a == 0) return {b, 0};

  // Contracts:
  // [1] s - m0 * a = 0 (mod b)
  // [2] t - m1 * a = 0 (mod b)
  // [3] s * |m1| + t * |m0| <= b
  long long s = b, t = a;
  long long m0 = 0, m1 = 1;

  while (t) {
    long long u = s / t;
    s -= t * u;
    m0 -= m1 * u;  // |m1 * u| <= |m1| * s <= b

    // [3]:
    // (s - t * u) * |m1| + t * |m0 - m1 * u|
    // <= s * |m1| - t * u * |m1| + t * (|m0| + |m1| * u)
    // = s * |m1| + t * |m0| <= b

    auto tmp = s;
    s = t;
    t = tmp;
    tmp = m0;
    m0 = m1;
    m1 = tmp;
  }
  // by [3]: |m0| <= b/g
  // by g != b: |m0| < b/g
  if (m0 < 0) m0 += b / s;
  return {s, m0};
}

// Compile time primitive root
// @param m must be prime
// @return primitive root (and minimum in now)
constexpr int primitive_root_constexpr(int m) {
  if (m == 2) return 1;
  if (m == 167772161) return 3;
  if (m == 469762049) return 3;
  if (m == 754974721) return 11;
  if (m == 998244353) return 3;
  int divs[20] = {};
  divs[0] = 2;
  int cnt = 1;
  int x = (m - 1) / 2;
  while (x % 2 == 0) x /= 2;
  for (int i = 3; (long long)(i)*i <= x; i += 2) {
    if (x % i == 0) {
      divs[cnt++] = i;
      while (x % i == 0) {
        x /= i;
      }
    }
  }
  if (x > 1) {
    divs[cnt++] = x;
  }
  for (int g = 2;; g++) {
    bool ok = true;
    for (int i = 0; i < cnt; i++) {
      if (pow_mod_constexpr(g, (m - 1) / divs[i], m) == 1) {
        ok = false;
        break;
      }
    }
    if (ok) return g;
  }
}
template <int m>
constexpr int primitive_root = primitive_root_constexpr(m);

// @param n `n < 2^32`
// @param m `1 <= m < 2^32`
// @return sum_{i=0}^{n-1} floor((ai + b) / m) (mod 2^64)
unsigned long long floor_sum_unsigned(unsigned long long n,
                                      unsigned long long m,
                                      unsigned long long a,
                                      unsigned long long b) {
  unsigned long long ans = 0;
  while (true) {
    if (a >= m) {
      ans += n * (n - 1) / 2 * (a / m);
      a %= m;
    }
    if (b >= m) {
      ans += n * (b / m);
      b %= m;
    }

    unsigned long long y_max = a * n + b;
    if (y_max < m) break;
    // y_max < m * (n + 1)
    // floor(y_max / m) <= n
    n = (unsigned long long)(y_max / m);
    b = (unsigned long long)(y_max % m);
    std::swap(m, a);
  }
  return ans;
}

}  // namespace internal

namespace internal {

#ifndef _MSC_VER
template <class T>
using is_signed_int128 =
    typename std::conditional<std::is_same<T, __int128_t>::value ||
                                  std::is_same<T, __int128>::value,
                              std::true_type, std::false_type>::type;

template <class T>
using is_unsigned_int128 =
    typename std::conditional<std::is_same<T, __uint128_t>::value ||
                                  std::is_same<T, unsigned __int128>::value,
                              std::true_type, std::false_type>::type;

template <class T>
using make_unsigned_int128 =
    typename std::conditional<std::is_same<T, __int128_t>::value, __uint128_t,
                              unsigned __int128>;

template <class T>
using is_integral =
    typename std::conditional<std::is_integral<T>::value ||
                                  is_signed_int128<T>::value ||
                                  is_unsigned_int128<T>::value,
                              std::true_type, std::false_type>::type;

template <class T>
using is_signed_int =
    typename std::conditional<(is_integral<T>::value &&
                               std::is_signed<T>::value) ||
                                  is_signed_int128<T>::value,
                              std::true_type, std::false_type>::type;

template <class T>
using is_unsigned_int =
    typename std::conditional<(is_integral<T>::value &&
                               std::is_unsigned<T>::value) ||
                                  is_unsigned_int128<T>::value,
                              std::true_type, std::false_type>::type;

template <class T>
using to_unsigned = typename std::conditional<
    is_signed_int128<T>::value, make_unsigned_int128<T>,
    typename std::conditional<std::is_signed<T>::value, std::make_unsigned<T>,
                              std::common_type<T>>::type>::type;

#else

template <class T>
using is_integral = typename std::is_integral<T>;

template <class T>
using is_signed_int =
    typename std::conditional<is_integral<T>::value && std::is_signed<T>::value,
                              std::true_type, std::false_type>::type;

template <class T>
using is_unsigned_int =
    typename std::conditional<is_integral<T>::value &&
                                  std::is_unsigned<T>::value,
                              std::true_type, std::false_type>::type;

template <class T>
using to_unsigned =
    typename std::conditional<is_signed_int<T>::value, std::make_unsigned<T>,
                              std::common_type<T>>::type;

#endif

template <class T>
using is_signed_int_t = std::enable_if_t<is_signed_int<T>::value>;

template <class T>
using is_unsigned_int_t = std::enable_if_t<is_unsigned_int<T>::value>;

template <class T>
using to_unsigned_t = typename to_unsigned<T>::type;

}  // namespace internal

namespace internal {

struct modint_base {};
struct static_modint_base : modint_base {};

template <class T>
using is_modint = std::is_base_of<modint_base, T>;
template <class T>
using is_modint_t = std::enable_if_t<is_modint<T>::value>;

}  // namespace internal

template <int m, std::enable_if_t<(1 <= m)>* = nullptr>
struct static_modint : internal::static_modint_base {
  using mint = static_modint;

 public:
  static constexpr int mod() { return m; }
  static mint raw(int v) {
    mint x;
    x._v = v;
    return x;
  }

  static_modint() : _v(0) {}
  template <class T, internal::is_signed_int_t<T>* = nullptr>
  static_modint(T v) {
    long long x = (long long)(v % (long long)(umod()));
    if (x < 0) x += umod();
    _v = (unsigned int)(x);
  }
  template <class T, internal::is_unsigned_int_t<T>* = nullptr>
  static_modint(T v) {
    _v = (unsigned int)(v % umod());
  }

  unsigned int val() const { return _v; }

  mint& operator++() {
    _v++;
    if (_v == umod()) _v = 0;
    return *this;
  }
  mint& operator--() {
    if (_v == 0) _v = umod();
    _v--;
    return *this;
  }
  mint operator++(int) {
    mint result = *this;
    ++*this;
    return result;
  }
  mint operator--(int) {
    mint result = *this;
    --*this;
    return result;
  }

  mint& operator+=(const mint& rhs) {
    _v += rhs._v;
    if (_v >= umod()) _v -= umod();
    return *this;
  }
  mint& operator-=(const mint& rhs) {
    _v -= rhs._v;
    if (_v >= umod()) _v += umod();
    return *this;
  }
  mint& operator*=(const mint& rhs) {
    unsigned long long z = _v;
    z *= rhs._v;
    _v = (unsigned int)(z % umod());
    return *this;
  }
  mint& operator/=(const mint& rhs) { return *this = *this * rhs.inv(); }

  mint operator+() const { return *this; }
  mint operator-() const { return mint() - *this; }

  mint pow(long long n) const {
    assert(0 <= n);
    mint x = *this, r = 1;
    while (n) {
      if (n & 1) r *= x;
      x *= x;
      n >>= 1;
    }
    return r;
  }
  mint inv() const {
    if (prime) {
      assert(_v);
      return pow(umod() - 2);
    } else {
      auto eg = internal::inv_gcd(_v, m);
      assert(eg.first == 1);
      return eg.second;
    }
  }

  friend mint operator+(const mint& lhs, const mint& rhs) {
    return mint(lhs) += rhs;
  }
  friend mint operator-(const mint& lhs, const mint& rhs) {
    return mint(lhs) -= rhs;
  }
  friend mint operator*(const mint& lhs, const mint& rhs) {
    return mint(lhs) *= rhs;
  }
  friend mint operator/(const mint& lhs, const mint& rhs) {
    return mint(lhs) /= rhs;
  }
  friend bool operator==(const mint& lhs, const mint& rhs) {
    return lhs._v == rhs._v;
  }
  friend bool operator!=(const mint& lhs, const mint& rhs) {
    return lhs._v != rhs._v;
  }

 private:
  unsigned int _v;
  static constexpr unsigned int umod() { return m; }
  static constexpr bool prime = internal::is_prime<m>;
};

template <int id>
struct dynamic_modint : internal::modint_base {
  using mint = dynamic_modint;

 public:
  static int mod() { return (int)(bt.umod()); }
  static void set_mod(int m) {
    assert(1 <= m);
    bt = internal::barrett(m);
  }
  static mint raw(int v) {
    mint x;
    x._v = v;
    return x;
  }

  dynamic_modint() : _v(0) {}
  template <class T, internal::is_signed_int_t<T>* = nullptr>
  dynamic_modint(T v) {
    long long x = (long long)(v % (long long)(mod()));
    if (x < 0) x += mod();
    _v = (unsigned int)(x);
  }
  template <class T, internal::is_unsigned_int_t<T>* = nullptr>
  dynamic_modint(T v) {
    _v = (unsigned int)(v % mod());
  }

  unsigned int val() const { return _v; }

  mint& operator++() {
    _v++;
    if (_v == umod()) _v = 0;
    return *this;
  }
  mint& operator--() {
    if (_v == 0) _v = umod();
    _v--;
    return *this;
  }
  mint operator++(int) {
    mint result = *this;
    ++*this;
    return result;
  }
  mint operator--(int) {
    mint result = *this;
    --*this;
    return result;
  }

  mint& operator+=(const mint& rhs) {
    _v += rhs._v;
    if (_v >= umod()) _v -= umod();
    return *this;
  }
  mint& operator-=(const mint& rhs) {
    _v += mod() - rhs._v;
    if (_v >= umod()) _v -= umod();
    return *this;
  }
  mint& operator*=(const mint& rhs) {
    _v = bt.mul(_v, rhs._v);
    return *this;
  }
  mint& operator/=(const mint& rhs) { return *this = *this * rhs.inv(); }

  mint operator+() const { return *this; }
  mint operator-() const { return mint() - *this; }

  mint pow(long long n) const {
    assert(0 <= n);
    mint x = *this, r = 1;
    while (n) {
      if (n & 1) r *= x;
      x *= x;
      n >>= 1;
    }
    return r;
  }
  mint inv() const {
    auto eg = internal::inv_gcd(_v, mod());
    assert(eg.first == 1);
    return eg.second;
  }

  friend mint operator+(const mint& lhs, const mint& rhs) {
    return mint(lhs) += rhs;
  }
  friend mint operator-(const mint& lhs, const mint& rhs) {
    return mint(lhs) -= rhs;
  }
  friend mint operator*(const mint& lhs, const mint& rhs) {
    return mint(lhs) *= rhs;
  }
  friend mint operator/(const mint& lhs, const mint& rhs) {
    return mint(lhs) /= rhs;
  }
  friend bool operator==(const mint& lhs, const mint& rhs) {
    return lhs._v == rhs._v;
  }
  friend bool operator!=(const mint& lhs, const mint& rhs) {
    return lhs._v != rhs._v;
  }

 private:
  unsigned int _v;
  static internal::barrett bt;
  static unsigned int umod() { return bt.umod(); }
};
template <int id>
internal::barrett dynamic_modint<id>::bt(998244353);

using modint998244353 = static_modint<998244353>;
using modint1000000007 = static_modint<1000000007>;
using modint = dynamic_modint<-1>;

namespace internal {

template <class T>
using is_static_modint = std::is_base_of<internal::static_modint_base, T>;

template <class T>
using is_static_modint_t = std::enable_if_t<is_static_modint<T>::value>;

template <class>
struct is_dynamic_modint : public std::false_type {};
template <int id>
struct is_dynamic_modint<dynamic_modint<id>> : public std::true_type {};

template <class T>
using is_dynamic_modint_t = std::enable_if_t<is_dynamic_modint<T>::value>;

}  // namespace internal
// using mint = modint998244353;
/*
g++ -std=c++23 -O2 -Wall -Wextra A.cpp -o A
./A < input.in > output.out

*/

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
using mint = ModInt<1000000007>;
const int MOD = 1000000007;
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

template <typename T, bool IS_MINIMIZED = true>
struct ConvexHullTrick {
  ConvexHullTrick() = default;

  void add(T a, T b) {
    if constexpr (!IS_MINIMIZED) {
      a = -a;
      b = -b;
    }
    const Line line(a, b);
    if (deq.empty()) [[unlikely]] {
      deq.emplace_back(line);
    } else if (deq.back().first >= a) {
      if (deq.back().first == a) {
        if (b >= deq.back().second) return;
        deq.pop_back();
      }
      for (int i = std::ssize(deq) - 2; i >= 0; --i) {
        if (!must_pop(deq[i], deq[i + 1], line)) break;
        deq.pop_back();
      }
      deq.emplace_back(line);
    } else {
      if (deq.front().first == a) {
        if (b >= deq.front().second) return;
        deq.pop_front();
      }
      while (deq.size() >= 2 && must_pop(line, deq.front(), deq[1])) {
        deq.pop_front();
      }
      deq.emplace_front(line);
    }
  }

  T query(const T x) const {
    assert(!deq.empty());
    int lb = -1, ub = deq.size() - 1;
    while (ub - lb > 1) {
      const int mid = std::midpoint(lb, ub);
      (f(deq[mid], x) < f(deq[mid + 1], x) ? ub : lb) = mid;
    }
    return IS_MINIMIZED ? f(deq[ub], x) : -f(deq[ub], x);
  }

  T monotonically_increasing_query(const T x) {
    while (deq.size() >= 2 && f(deq.front(), x) >= f(deq[1], x)) {
      deq.pop_front();
    }
    return IS_MINIMIZED ? f(deq.front(), x) : -f(deq.front(), x);
  }

  T monotonically_decreasing_query(const T x) {
    for (int i = std::ssize(deq) - 2; i >= 0; --i) {
      if (f(deq[i], x) > f(deq[i + 1], x)) break;
      deq.pop_back();
    }
    return IS_MINIMIZED ? f(deq.back(), x) : -f(deq.back(), x);
  }

 private:
  using Line = std::pair<T, T>;

  std::deque<Line> deq;

  bool must_pop(const Line& l1, const Line& l2, const Line& l3) const {
#ifdef __SIZEOF_INT128__
    const T lhs_num = l3.second - l2.second, lhs_den = l2.first - l3.first;
    const T rhs_num = l2.second - l1.second, rhs_den = l1.first - l2.first;
    return __int128{lhs_num} * rhs_den <= __int128{rhs_num} * lhs_den;
#else
    const long double lhs =
        static_cast<long double>(l3.second - l2.second) / (l2.first - l3.first);
    const long double rhs =
        static_cast<long double>(l2.second - l1.second) / (l1.first - l2.first);
    return lhs <= rhs;
#endif  // __SIZEOF_INT128__
  }

  T f(const Line& l, const T x) const { return l.first * x + l.second; }
};

template <class T>
struct Matrix {
  std::vector<std::vector<T>> A;

  Matrix() {}

  Matrix(int n, int m) : A(n, std::vector<T>(m, 0)) {}

  Matrix(int n) : A(n, std::vector<T>(n, 0)) {};

  int size() const {
    if (A.empty()) return 0;
    assert(A.size() == A[0].size());
    return A.size();
  }

  int height() const { return (A.size()); }

  int width() const { return (A[0].size()); }

  inline const std::vector<T>& operator[](int k) const { return (A.at(k)); }

  inline std::vector<T>& operator[](int k) { return (A.at(k)); }

  static Matrix I(int n) {
    Matrix mat(n);
    for (int i = 0; i < n; i++) mat[i][i] = 1;
    return (mat);
  }

  Matrix& operator+=(const Matrix& B) {
    int n = height(), m = width();
    assert(n == B.height() && m == B.width());
    for (int i = 0; i < n; i++)
      for (int j = 0; j < m; j++) (*this)[i][j] += B[i][j];
    return (*this);
  }

  Matrix& operator-=(const Matrix& B) {
    int n = height(), m = width();
    assert(n == B.height() && m == B.width());
    for (int i = 0; i < n; i++)
      for (int j = 0; j < m; j++) (*this)[i][j] -= B[i][j];
    return (*this);
  }

  Matrix& operator*=(const Matrix& B) {
    int n = height(), m = B.width(), p = width();
    assert(p == B.height());
    std::vector<std::vector<T>> C(n, std::vector<T>(m, 0));
    for (int i = 0; i < n; i++)
      for (int j = 0; j < m; j++)
        for (int k = 0; k < p; k++)
          C[i][j] = (C[i][j] + (*this)[i][k] * B[k][j]);
    A.swap(C);
    return (*this);
  }

  Matrix multiply_mod(const Matrix& A, const Matrix& B, long long mod) {
    int n = height(), m = B.width(), p = width();
    assert(p == B.height());
    Matrix C(n, m);
    for (int i = 0; i < n; i++)
      for (int j = 0; j < m; j++)
        for (int k = 0; k < p; k++)
          C[i][j] = (C[i][j] + A[i][k] * B[k][j]) % mod;

    return C;
  }

  Matrix& operator^=(long long k) {
    Matrix B = Matrix::I(height());
    while (k > 0) {
      if (k & 1) B *= *this;
      *this *= *this;
      k >>= 1LL;
    }
    A.swap(B.A);
    return (*this);
  }

  Matrix pow_mod(Matrix& A, long long k, long long mod) {
    Matrix B = Matrix::I(height());
    while (k > 0) {
      if (k & 1) B = multiply_mod(B, A, mod);
      A = multiply_mod(A, A, mod);
      k >>= 1LL;
    }
    return B;
  }

  template <typename Func>
  Matrix power(long long k, const Matrix& unit, Func mult) const {
    if (k == 0) return unit;

    Matrix res;
    Matrix base = *this;
    bool initialized = false;

    while (k > 0) {
      if (k & 1) {
        if (!initialized) {
          res = base;
          initialized = true;
        } else {
          res = mult(res, base);
        }
      }
      base = mult(base, base);
      k >>= 1;
    }
    return res;
  }

  Matrix operator+(const Matrix& B) const { return (Matrix(*this) += B); }

  Matrix operator-(const Matrix& B) const { return (Matrix(*this) -= B); }

  Matrix operator*(const Matrix& B) const { return (Matrix(*this) *= B); }

  Matrix operator^(const long long k) const { return (Matrix(*this) ^= k); }

  friend std::ostream& operator<<(std::ostream& os, Matrix& p) {
    int n = p.height(), m = p.width();
    for (int i = 0; i < n; i++) {
      os << "[";
      for (int j = 0; j < m; j++) {
        os << p[i][j] << (j + 1 == m ? "]\n" : ",");
      }
    }
    return (os);
  }

  T determinant() {
    Matrix B(*this);
    assert(width() == height());
    T ret = 1;
    for (int i = 0; i < width(); i++) {
      int idx = -1;
      for (int j = i; j < width(); j++) {
        if (B[j][i] != 0) idx = j;
      }
      if (idx == -1) return (0);
      if (i != idx) {
        ret *= -1;
        swap(B[i], B[idx]);
      }
      ret *= B[i][i];
      T vv = B[i][i];
      for (int j = 0; j < width(); j++) {
        B[i][j] /= vv;
      }
      for (int j = i + 1; j < width(); j++) {
        T a = B[j][i];
        for (int k = 0; k < width(); k++) {
          B[j][k] -= B[i][k] * a;
        }
      }
    }
    return (ret);
  }
};

void solve() {
  int n, l, r;
  std::cin >> n >> l >> r;
  std::vector<long long> a(n);
  for (int i = 0; i < n; i++) std::cin >> a[i];
  std::vector C(n + 1, std::vector<long long>(n + 1));
  for (int i = 0; i <= n; i++) {
    C[i][0] = 1;
    for (int j = 1; j <= i; j++) {
      C[i][j] = C[i - 1][j - 1] + C[i - 1][j];
    }
  }
  std::ranges::sort(a, std::greater<long long>());  
  long long sum_a = std::ranges::fold_left(a | std::views::take(l), 0LL, std::plus{});
  std::cout << std::fixed << std::setprecision(10) << (double)sum_a / l << "\n";
  long long target = a[l - 1];
  int total = std::ranges::count(a, target);
  int less = std::ranges::count(a | std::views::take(l), target);
  long long ans = 0;
  if (a[0] > target) {
    ans += C[total][less];
  } else {
    for (int i = less; i <= r; i++) {
      ans += C[total][i]; 
    }
  }
  std::cout << ans << "\n";

}

int main() {
  std::ios::sync_with_stdio(false);
  std::cin.tie(nullptr);
  int t = 1;
  // std::cin >> t;
  while (t--) {
    solve();
  }
  return 0;
}
