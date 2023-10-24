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
#include <map>
#include <numeric>
#include <queue>
#include <random>
#include <set>
#include <unordered_map>
#include <unordered_set>

void solve() {
  int n, k;
  std::cin >> n >> k;
  std::string s;
  std::cin >> s;
  int ans = 0;

  for (int i = 0, j = 0, a = 0; i < n; i++) {
    while (j < n and a + k + (s[j] == 'a') >= j - i + 1) {
      a += (s[j] == 'a');
      j++;
    }
    ans = std::max(ans, j - i);

    a -= (s[i] == 'a');
  }
  for (int i = 0, j = 0, a = 0; i < n; i++) {
    while (j < n and a + k + (s[j] == 'b') >= j - i + 1) {
      a += s[j] == 'b';
      j++;
    }
    ans = std::max(ans, j - i);
    a -= s[i] == 'b';
  }
  std::cout << ans << '\n';
}
int main() {
  int t = 1;
  // std::cin >> t;
  while (t--) solve();
}
