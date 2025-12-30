// credit: https://emthrm.github.io/cp-library/include/emthrm/string/longest_common_prefix.hpp

template <typename T = std::string>
  requires requires { typename T::value_type; }
struct SuffixArray {
  std::vector<int> sa, rank;

  explicit SuffixArray(const T& s_, const typename T::value_type sentinel = 0)
      : s(s_) {
    const int n = s.size();
    s.push_back(sentinel);
    sa.resize(n + 1);
    std::iota(sa.rbegin(), sa.rend(), 0);
    std::ranges::stable_sort(
        sa, {}, [this](const int index) -> int { return s[index]; });
    rank.resize(n + 1);
    for (int i = 0; i <= n; ++i) {
      rank[i] = s[i];
    }
    std::vector<int> tmp(n + 1), prev_sa(n + 1);
    for (int len = 1; len <= n; len <<= 1) {
      tmp[sa[0]] = 0;
      for (int i = 1; i <= n; ++i) {
        if (rank[sa[i - 1]] == rank[sa[i]] && sa[i - 1] + len <= n &&
            rank[sa[i - 1] + (len >> 1)] == rank[sa[i] + (len >> 1)]) {
          tmp[sa[i]] = tmp[sa[i - 1]];
        } else {
          tmp[sa[i]] = i;
        }
      }
      rank.swap(tmp);
      std::iota(tmp.begin(), tmp.end(), 0);
      std::copy(sa.begin(), sa.end(), prev_sa.begin());
      for (int i = 0; i <= n; ++i) {
        const int idx = prev_sa[i] - len;
        if (idx >= 0) sa[tmp[rank[idx]]++] = idx;
      }
    }
    for (int i = 0; i <= n; ++i) {
      rank[sa[i]] = i;
    }
  }

  std::vector<int> match(T* t) const {
    const int lb = lower_bound(t);
    ++t->back();
    const int ub = lower_bound(t);
    --t->back();
    std::vector<int> res(ub - lb);
    std::copy(sa.begin() + lb, sa.begin() + ub, res.begin());
    std::sort(res.begin(), res.end());
    return res;
  }

 private:
  T s;

  int lower_bound(const T* t) const {
    const int s_size = s.size(), t_size = t->size();
    int lb = 0, ub = s_size;
    while (ub - lb > 1) {
      const int mid = std::midpoint(lb, ub);
      int s_idx = sa[mid], t_idx = 0;
      bool finished = false;
      for (; s_idx < s_size && t_idx < t_size; ++s_idx, ++t_idx) {
        if (s[s_idx] != (*t)[t_idx]) {
          (s[s_idx] < (*t)[t_idx] ? lb : ub) = mid;
          finished = true;
          break;
        }
      }
      if (!finished) (s_idx == s_size && t_idx < t_size ? lb : ub) = mid;
    }
    return ub;
  }
};


template <typename Band>
struct SparseTable {
  using BinOp = std::function<Band(Band, Band)>;

  SparseTable() = default;

  explicit SparseTable(const std::vector<Band>& a, const BinOp bin_op) {
    init(a, bin_op);
  }

  void init(const std::vector<Band>& a, const BinOp bin_op_) {
    bin_op = bin_op_;
    const int n = a.size();
    assert(n > 0);
    lg.assign(n + 1, 0);
    for (int i = 2; i <= n; ++i) {
      lg[i] = lg[i >> 1] + 1;
    }
    const int table_h = std::countr_zero(std::bit_floor(a.size())) + 1;
    data.assign(table_h, std::vector<Band>(n));
    std::copy(a.begin(), a.end(), data.front().begin());
    for (int i = 1; i < table_h; ++i) {
      for (int j = 0; j + (1 << i) <= n; ++j) {
        data[i][j] = bin_op(data[i - 1][j], data[i - 1][j + (1 << (i - 1))]);
      }
    }
  }

  Band query(const int left, const int right) const {
    assert(left < right);
    const int h = lg[right - left];
    return bin_op(data[h][left], data[h][right - (1 << h)]);
  }

 private:
  BinOp bin_op;
  std::vector<int> lg;
  std::vector<std::vector<Band>> data;
};

template <typename T = std::string>
struct LongestCommonPrefix : SuffixArray<T> {
  std::vector<int> lcp_array;

  explicit LongestCommonPrefix(const T& s) : SuffixArray<T>(s) {
    const int n = s.size();
    lcp_array.resize(n);
    for (int i = 0, common = 0; i < n; ++i) {
      const int j = this->sa[this->rank[i] - 1];
      if (common > 0) --common;
      while (i + common < n && j + common < n &&
             s[i + common] == s[j + common]) {
        ++common;
      }
      lcp_array[this->rank[i] - 1] = common;
    }
    st.init(lcp_array, [](const int a, const int b) -> int {
      return std::min(a, b);
    });
  }

  int query(int i, int j) const {
    assert(i != j);
    i = this->rank[i];
    j = this->rank[j];
    if (i > j) std::swap(i, j);
    return st.query(i, j);
  }

 private:
  SparseTable<int> st;
};