
template <class T>
struct WeightedDSU {
  std::vector<int> par;
  std::vector<int> rnk;
  std::vector<T> diff_weight;

  WeightedDSU(int n = 1, T sum_unity = 0) { init(n, sum_unity); }

  void init(int n, T sum_unity = 0) {
    par.resize(n);
    rnk.assign(n, 0);
    diff_weight.assign(n, sum_unity);
    std::iota(par.begin(), par.end(), 0);
  }

  int root(int x) {
    if (par[x] == x) {
      return x;
    } else {
      int r = root(par[x]);
      diff_weight[x] += diff_weight[par[x]];
      return par[x] = r;
    }
  }

  T weight(int x) {
    root(x);
    return diff_weight[x];
  }

  bool same(int x, int y) { return root(x) == root(y); }

  bool merge(int x, int y, T w) {
    w += weight(x);
    w -= weight(y);
    x = root(x);
    y = root(y);
    if (x == y) return false;

    if (rnk[x] < rnk[y]) {
      std::swap(x, y);
      w = -w;
    }
    if (rnk[x] == rnk[y]) ++rnk[x];

    par[y] = x;
    diff_weight[y] = w;
    return true;
  }

  T diff(int x, int y) { return weight(y) - weight(x); }
};

template <class T>
struct XorDSU {
  std::vector<int> par;
  std::vector<int> rnk;
  std::vector<T> diff_weight;

  XorDSU(int n = 1, T sum_unity = 0) { init(n, sum_unity); }

  void init(int n, T sum_unity = 0) {
    par.resize(n);
    rnk.assign(n, 0);
    diff_weight.assign(n, sum_unity);
    std::iota(par.begin(), par.end(), 0);
  }

  int root(int x) {
    if (par[x] == x) {
      return x;
    } else {
      int r = root(par[x]);
      diff_weight[x] ^= diff_weight[par[x]];
      return par[x] = r;
    }
  }

  T weight(int x) {
    root(x);
    return diff_weight[x];
  }

  bool same(int x, int y) { return root(x) == root(y); }

  bool merge(int x, int y, T w) {
    w ^= weight(x);
    w ^= weight(y);
    x = root(x);
    y = root(y);
    if (x == y) return false;

    if (rnk[x] < rnk[y]) {
      std::swap(x, y);
    }
    if (rnk[x] == rnk[y]) ++rnk[x];

    par[y] = x;
    diff_weight[y] = w;
    return true;
  }

  T diff(int x, int y) { return weight(y) ^ weight(x); }
};