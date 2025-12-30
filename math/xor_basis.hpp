
template <int D>
struct Basis {
  std::vector<std::bitset<D>> v;
  std::vector<int> msb;

  Basis() = default;

  static bool greater_bitset(const std::bitset<D>& a, const std::bitset<D>& b) {
    for (int i = D - 1; i >= 0; --i) {
      if (a[i] != b[i]) return a[i] > b[i];
    }
    return false;  // 相等则不大于
  }

  // 判断 val 能否被表示为基的线性组合
  bool representable(std::bitset<D> val) const {
    const int n = rank();

    for (int i = 0; i < n; ++i) {
      if (val[msb[i]]) val ^= v[i];
    }
    return val.none();  // true 表示能被表示（x ∈ span）
  }

  bool add(std::bitset<D> val) {
    const int n = rank();
    if (n == D) return false;

    // 用已有基消元
    for (int i = 0; i < n; ++i) {
      if (val[msb[i]]) val ^= v[i];
    }
    if (val.none()) return false;

    int m = D - 1;
    while (m >= 0 && !val[m]) --m;

    if (v.empty()) {
      v.emplace_back(val);
      msb.emplace_back(m);
      return true;
    }

    const int idx = std::distance(std::upper_bound(msb.rbegin(), msb.rend(), m),
                                  msb.rend());

    v.emplace(std::next(v.begin(), idx), val);
    msb.emplace(std::next(msb.begin(), idx), m);

    for (int i = idx + 1; i <= n; ++i) {
      if (v[idx][msb[i]]) v[idx] ^= v[i];
    }

    for (int i = idx - 1; i >= 0; --i) {
      if (v[i][m]) v[i] ^= v[idx];
    }
    return true;
  }

  int rank() const { return (int)v.size(); }

  std::bitset<D> max_xor_by_all_xor() const {
    std::bitset<D> ans;
    for (const auto& e : v) ans ^= e;
    return ans;
  }

  // 返回所有可达的 XOR 结果（所有子集方案），大小为 2^rank()
  // 复杂度：O(2^r * r) 次 bitset XOR；内存：O(2^r)
  std::vector<std::bitset<D>> all_subset_xors() const {
    const int r = rank();
    std::vector<std::bitset<D>> res;
    res.reserve(1u << r);
    res.emplace_back();

    for (int i = 0; i < r; ++i) {
      const int sz = (int)res.size();
      res.resize(sz * 2);
      for (int j = 0; j < sz; ++j) {
        res[sz + j] = res[j] ^ v[i];
      }
    }
    return res;
  }

    // 返回第 k 小的 XOR 值（0-indexed，包含 0）
  // 若 k 超出范围（k >= 2^rank）则返回 false
  bool kth_smallest_xor(unsigned long long k, std::bitset<D>& out) const {
    const int r = rank();

    // 若 r < 64，则可以精确判断是否越界；若 r >= 64，任何 unsigned long long 都 < 2^r
    if (r < 64 && k >= (1ULL << r)) return false;

    std::bitset<D> ans;
    // v 按 msb 从大到小存；k 的低位应该对应 msb 更小的向量
    for (int i = 0; i < r; ++i) {
      if ((k >> i) & 1ULL) ans ^= v[r - 1 - i];
    }
    out = ans;
    return true;
  }

  inline bool operator<(const Basis& x) const {
    const int n = (int)v.size();
    if (n != x.rank()) return n < x.rank();
    if (n == D) return false;
    for (int i = 0; i < n; ++i) {
      if (msb[i] != x.msb[i]) return msb[i] < x.msb[i];
    }
    for (int i = 0; i < n; ++i) {
      for (int j = msb[i] - 1;; --j) {
        if (v[i][j] != x.v[i][j]) return x.v[i][j];
      }
    }
    return false;
  }
};