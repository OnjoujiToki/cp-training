struct SquareFreeSieve {
    int N;
    std::vector<int> spf;  // smallest prime factor
    std::vector<int> sf;   // square-free part
  
    explicit SquareFreeSieve(int n) : N(n), spf(n + 1, 0), sf(n + 1, 0) {
      build_spf_linear();
      build_square_free();
    }
  
    void build_spf_linear() {
      std::vector<int> primes;
      spf[0] = 0;
      if (N >= 1) spf[1] = 1;
      for (int i = 2; i <= N; i++) {
        if (spf[i] == 0) {
          spf[i] = i;
          primes.push_back(i);
        }
        for (int p : primes) {
          long long v = 1LL * p * i;
          if (v > N) break;
          spf[(int)v] = p;
          if (p == spf[i]) break;
        }
      }
    }
  
    void build_square_free() {
      if (N >= 0) sf[0] = 0;
      if (N >= 1) sf[1] = 1;
  
      // We also maintain parity of exponent of spf[x] in x:
      // Let x = (x / p) * p. If p divides (x / p), exponent parity flips to even, else odd.
      // Recurrence:
      //   if ( (x/p) % p != 0 ) sf[x] = sf[x/p] * p
      //   else                  sf[x] = sf[(x/p)/p]
      for (int x = 2; x <= N; x++) {
        int p = spf[x];
        int y = x / p;
        if (y % p != 0) {
          // p appears odd times so far: toggle in
          // sf[y] is square-free, and p does not divide sf[y], safe to multiply
          sf[x] = sf[y] * p;
        } else {
          // exponent of p becomes even: remove p^2
          sf[x] = sf[y / p];
        }
      }
    }
  };
  


// Return square-free core (square-free part) of n.
int square_free_core(int n) {
    if (n == 0) return 0;
    int res = 1;
    for (int p = 2; 1LL * p * p <= n; ++p) {
      if (n % p == 0) {
        int cnt = 0;
        while (n % p == 0) {
          n /= p;
          cnt ^= 1;
        }
        if (cnt) res *= p;
      }
    }
    if (n > 1) res *= n;
    return res;
  }