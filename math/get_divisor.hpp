#include <algorithm>
#include <vector>
template <typename T>
std::vector<T> get_divisors(T x, bool sorted = true) {
  std::vector<T> res;
  for (T i = 1; i <= x / i; i++)
    if (x % i == 0) {
      res.push_back(i);
      if (i != x / i) res.push_back(x / i);
    }
  if (sorted) std::sort(res.begin(), res.end());
  return res;
}