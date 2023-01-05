#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <chrono>
#include <cmath>
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
#include <sstream>
#include <stack>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

void solve() {
  int n;
  std::cin >> n;
  std::string s;
  std::cin >> s;
  std::vector l(n + 1, 0);
  std::vector r(n + 1, 0);
  for (int i = 0; i < n; i++) {
    l[i + 1] = l[i] + (s[i] == 'L');
    r[i + 1] = r[i] + (s[i] == 'R');
  }

  for (int i = 0; i < n; i++) {
    if (r[i] == 0 and l.back() - l[i + 1] == 0) {
      bool ok1 = false, ok2 = false;
      if (i) {
        int R1 = r[i], L1 = l.back() - l[i + 1];
        int R2 = r[i - 1], L2 = l.back() - l[i];
        R2 += s[i] == 'R';
        L2 -= s[i] == 'L';
        L1 += s[i - 1] == 'L';
        R1 -= s[i - 1] == 'R';
        if ((R2 or L2) and (L1 or R1)) ok1 = true;
      }

      if (i < n - 1) {
        int R1 = r[i + 1], L1 = l.back() - l[i + 2];
        int R2 = r[i], L2 = l.back() - l[i + 1];

        R2 += s[i + 1] == 'R';
        L2 -= s[i + 1] == 'L';
        L1 += s[i] == 'L';
        R1 -= s[i] == 'R';
        if ((R2 or L2) and (L1 or R1)) ok2 = true;
      }
      if (ok1) {
        std::cout << 1 + i - 1 << '\n';
        return;
      } else if (ok2) {
        std::cout << i + 1 << '\n';
        return;
      } else {
        std::cout << -1 << '\n';
        return;
      }
    }
  }
  std::cout << 0 << '\n';
}
int main() {
  std::cin.tie(nullptr);
  std::ios::sync_with_stdio(false);
  int t = 1;
  std::cin >> t;
  while (t--) solve();
  // solve();
  return 0;
}