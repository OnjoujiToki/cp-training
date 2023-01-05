// C(a, b) % p a <= 1e18, b <= 1e18, c is prime
long long mod_pow(long long x, long long n, long long p) {
  long long ret = 1;
  while (n) {
    if (n & 1) (ret *= x) %= p;
    (x *= x) %= p;
    n >>= 1;
  }
  return ret;
}

int combination(int a, int b, int mod) {
  int res = 1;
  for (int i = 1, j = a; i <= b; i++, j--) {
    res = 1LL * res * j % mod;
    res = 1LL * res * mod_pow(i, mod - 2, mod) % mod;
  }
  return res;
}
long long lucas(long long a, long long b, int mod) {
  if (a < mod and b < mod) return combination(a, b, mod);
  return 1LL * combination(a % mod, b % mod, mod) *
         lucas(a / mod, b / mod, mod) % mod;
}
