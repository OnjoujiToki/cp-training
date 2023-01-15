#include <algorithm>
#include <array>
#include <bit>
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
#include <set>
#include <unordered_map>
#include <unordered_set>

std::vector<bool> prime_table(int n) {
  std::vector<bool> prime(n + 1, true);
  if (n >= 0) prime[0] = false;
  if (n >= 1) prime[1] = false;
  for (int i = 2; i * i <= n; i++) {
    if (!prime[i]) continue;
    for (int j = i * i; j <= n; j += i) {
      prime[j] = false;
    }
  }
  return prime;
}

std::vector<int> add(std::vector<int> &A, std::vector<int> &B) {
  std::vector<int> C;
  int t = 0;
  for (int i = 0; i < A.size() || i < B.size(); i++) {
    if (i < A.size()) t += A[i];
    if (i < B.size()) t += B[i];
    C.push_back(t % 10);
    t /= 10;  // 进位
  }
  if (t) C.push_back(1);  // 如果还有进位
  return C;
}

void solve() {
  std::string a;
  std::cin >> a;
  std::vector<int> A;
  for (int i = a.size() - 1; i >= 0; i--) {
    A.push_back(a[i] - '0');
  }
  int n = a.size();
  std::vector<int> ans = {0};
  // k == 0 for all, k ==  1 for n - 1, k == n for 0 ...
  // max k is n!
  //
  for (int i = n; i >= 1; i--) {
    std::vector<int> B;
    for (int j = i - 1; j >= 0; j--) {
      // std::cout << a[j] - '0';
      B.push_back(a[j] - '0');
    }
    // std::cout << '\n';
    ans = add(ans, B);
  }
  for (int i = ans.size() - 1; i >= 0; i--) {
    std::cout << ans[i];
  }
}
int main() {
  std::cin.tie(nullptr);
  int t = 1;

  while (t--) solve();
  // solve();
  return 0;
}