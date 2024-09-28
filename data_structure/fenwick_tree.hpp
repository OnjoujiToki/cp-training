template <typename T>
struct FenwickTree {
  std::vector<T> bit;
  int n;
  FenwickTree(int _n) : n(_n), bit(_n) {}

  T sum(int r) {
    T ret = 0;
    for (; r >= 0; r = (r & (r + 1)) - 1) ret += bit[r];
    return ret;
  }

  T sum(int l, int r) {
    assert(l <= r);
    return sum(r) - sum(l - 1);
  }  // [l, r]

  void add(int idx, T delta) {
    for (; idx < n; idx = idx | (idx + 1)) bit[idx] += delta;
  }
  void set(int idx, T val) { add(idx, val - sum(idx, idx)); }
};
