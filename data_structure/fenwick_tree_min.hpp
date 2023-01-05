#include <vector>
struct FenwickTreeMin {
  std::vector<int> bit;
  int n;
  const int INF = (int)1e9;

  FenwickTreeMin(int n) {
    this->n = n;
    bit.assign(n, INF);
  }

  FenwickTreeMin(std::vector<int> a) : FenwickTreeMin(a.size()) {
    for (size_t i = 0; i < a.size(); i++) update(i, a[i]);
  }

  int getmin(int r) {
    int ret = INF;
    for (; r >= 0; r = (r & (r + 1)) - 1) ret = std::min(ret, bit[r]);
    return ret;
  }

  void update(int idx, int val) {
    for (; idx < n; idx = idx | (idx + 1)) bit[idx] = std::min(bit[idx], val);
  }
};