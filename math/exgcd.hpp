std::pair<int, int> ext_gcd(int a, int b) {
  if (b == 0) {
    return std::make_pair(1, 0);
  } else {
    pair<int, int> ans = ext_gcd(b, a % b);
    swap(ans.first, ans.second);
    ans.second += ans.first * (a / b);
    return ans;
  }
}