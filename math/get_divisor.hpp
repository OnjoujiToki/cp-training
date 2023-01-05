#include <algorithm>
#include <vector>
std::vector<int> get_divisors(int x, bool sorted = true) {
  std::vector<int> res;
  for (int i = 1; i <= x / i; i++)
    if (x % i == 0) {
      res.push_back(i);
      if (i != x / i) res.push_back(x / i);
    }
  if (sorted) std::sort(res.begin(), res.end());
  return res;
}