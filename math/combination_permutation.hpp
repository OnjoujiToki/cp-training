#include <algorithm>
#include <tuple>
#include <vector>
template <typename T>
T mod_pow(T x, long long n, const T &p) {
  T ret = 1;
  while (n) {
    if (n & 1) (ret *= x) %= p;
    (x *= x) %= p;
    n >>= 1;
  }
  return ret;
}
int mod = 1e9 + 7;
std::pair<std::vector<int>, std::vector<int>> init_comb(int n,
                                                        int mod = 1000000007) {
  std::vector<int> fact(n + 5), invFact(n + 5);
  fact[0] = invFact[0] = 1;
  for (int i = 1; i < n + 5; i++) {
    fact[i] = (long long)fact[i - 1] * i % mod;
    invFact[i] =
        (long long)invFact[i - 1] * mod_pow(1LL * i, mod - 2, 1LL * mod) % mod;
  }
  return {fact, invFact};
}

std::vector<int> fac, inv;
std::tie(fac, inv) = init_comb(2005, mod);
auto combination = [&](int a, int b) -> int {
  if (a < b) return 0;
  return 1LL * fac[a] * inv[a - b] % mod * inv[b] % mod;
};
auto permutation = [&](int a, int b) -> int {
  if (a < b) return 0;
  return 1LL * fac[a] * inv[a - b] % mod;
};