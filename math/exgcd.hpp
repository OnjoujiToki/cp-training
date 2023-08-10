std::pair<long long, long long> exgcd(long long a, long long b) {
  if (b == 0) {
    return std::make_pair(1LL, 0LL);
  } else {
    std::pair<long long, long long> ans = exgcd(b, a % b);
    swap(ans.first, ans.second);
    ans.second -= ans.first * (a / b);
    return ans;
  }
}  // get [x, y] for ax + by =  c also for a === b (mod m) if gcd(a, m) | b

/*
a===b(mod m)
equals to ax + my = b
[x, y], x = x * b / gcd(a, m)
*/
long long func(long long a, long long b, long long mod) {
  auto [x, y] = exgcd(a, mod);
  auto d = std::gcd(a, mod);
  if (b % d) return -1LL;
  long long t = b / d;

  return (x * t % (mod / d) + (mod / d)) % (mod / d);
}