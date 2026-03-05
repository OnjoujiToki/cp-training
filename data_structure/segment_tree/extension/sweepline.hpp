#pragma once
// credit: emthrm.github.io/library
#include <algorithm>
#include <limits>
#include <vector>

template <typename T>
struct LazySegmentTree {
  using Monoid = typename T::Monoid;
  using LazyMonoid = typename T::LazyMonoid;

  explicit LazySegmentTree(const int n)
      : LazySegmentTree(std::vector<Monoid>(n, T::m_id())) {}

  explicit LazySegmentTree(const std::vector<Monoid>& a)
      : n(a.size()), height(0) {
    while ((1 << height) < n) ++height;
    sz = 1 << height;
    lazy.assign(sz, T::lazy_id());
    data.assign(sz << 1, T::m_id());
    std::copy(a.begin(), a.end(), data.begin() + sz);
    for (int i = sz - 1; i > 0; --i) {
      data[i] = T::m_merge(data[i << 1], data[(i << 1) + 1]);
    }
  }

  void set(int idx, const Monoid val) {
    idx += sz;
    for (int i = height; i > 0; --i) {
      propagate(idx >> i);
    }
    data[idx] = val;
    for (int i = 1; i <= height; ++i) {
      const int current_idx = idx >> i;
      data[current_idx] =
          T::m_merge(data[current_idx << 1], data[(current_idx << 1) + 1]);
    }
  }

  void apply(int idx, const LazyMonoid val) {
    idx += sz;
    for (int i = height; i > 0; --i) {
      propagate(idx >> i);
    }
    data[idx] = T::apply(data[idx], val);
    for (int i = 1; i <= height; ++i) {
      const int current_idx = idx >> i;
      data[current_idx] =
          T::m_merge(data[current_idx << 1], data[(current_idx << 1) + 1]);
    }
  }

  void apply(int left, int right, const LazyMonoid val) {
    if (right <= left) return;
    left += sz;
    right += sz;
    const int ctz_left = __builtin_ctz(left);
    for (int i = height; i > ctz_left; --i) {
      propagate(left >> i);
    }
    const int ctz_right = __builtin_ctz(right);
    for (int i = height; i > ctz_right; --i) {
      propagate(right >> i);
    }
    for (int l = left, r = right; l < r; l >>= 1, r >>= 1) {
      if (l & 1) apply_sub(l++, val);
      if (r & 1) apply_sub(--r, val);
    }
    for (int i = left >> (ctz_left + 1); i > 0; i >>= 1) {
      data[i] = T::m_merge(data[i << 1], data[(i << 1) + 1]);
    }
    for (int i = right >> (ctz_right + 1); i > 0; i >>= 1) {
      data[i] = T::m_merge(data[i << 1], data[(i << 1) + 1]);
    }
  }

  Monoid get(int left, int right) {
    if (right <= left) return T::m_id();
    left += sz;
    right += sz;
    const int ctz_left = __builtin_ctz(left);
    for (int i = height; i > ctz_left; --i) {
      propagate(left >> i);
    }
    const int ctz_right = __builtin_ctz(right);
    for (int i = height; i > ctz_right; --i) {
      propagate(right >> i);
    }
    Monoid res_l = T::m_id(), res_r = T::m_id();
    for (; left < right; left >>= 1, right >>= 1) {
      if (left & 1) res_l = T::m_merge(res_l, data[left++]);
      if (right & 1) res_r = T::m_merge(data[--right], res_r);
    }
    return T::m_merge(res_l, res_r);
  }

  Monoid operator[](const int idx) {
    const int node = idx + sz;
    for (int i = height; i > 0; --i) {
      propagate(node >> i);
    }
    return data[node];
  }

  template <typename G>
  int find_right(int left, const G g) {
    if (left >= n) return n;
    left += sz;
    for (int i = height; i > 0; --i) {
      propagate(left >> i);
    }
    Monoid val = T::m_id();
    do {
      while (!(left & 1)) left >>= 1;
      Monoid nxt = T::m_merge(val, data[left]);
      if (!g(nxt)) {
        while (left < sz) {
          propagate(left);
          left <<= 1;
          nxt = T::m_merge(val, data[left]);
          if (g(nxt)) {
            val = nxt;
            ++left;
          }
        }
        return left - sz;
      }
      val = nxt;
      ++left;
    } while (__builtin_popcount(left) > 1);
    return n;
  }

  template <typename G>
  int find_left(int right, const G g) {
    if (right <= 0) return -1;
    right += sz;
    for (int i = height; i > 0; --i) {
      propagate((right - 1) >> i);
    }
    Monoid val = T::m_id();
    do {
      --right;
      while (right > 1 && (right & 1)) right >>= 1;
      Monoid nxt = T::m_merge(data[right], val);
      if (!g(nxt)) {
        while (right < sz) {
          propagate(right);
          right = (right << 1) + 1;
          nxt = T::m_merge(data[right], val);
          if (g(nxt)) {
            val = nxt;
            --right;
          }
        }
        return right - sz;
      }
      val = nxt;
    } while (__builtin_popcount(right) > 1);
    return -1;
  }

  const int n;
  int sz, height;
  std::vector<Monoid> data;
  std::vector<LazyMonoid> lazy;

  void apply_sub(const int idx, const LazyMonoid& val) {
    data[idx] = T::apply(data[idx], val);
    if (idx < sz) lazy[idx] = T::lazy_merge(lazy[idx], val);
  }

  void propagate(const int idx) {
    apply_sub(idx << 1, lazy[idx]);
    apply_sub((idx << 1) + 1, lazy[idx]);
    lazy[idx] = T::lazy_id();
  }
};

namespace monoid {

template <typename T>
struct RangeMinimumAndUpdateQuery {
  using Monoid = T;
  using LazyMonoid = T;
  static constexpr Monoid m_id() { return std::numeric_limits<Monoid>::max(); }
  static constexpr LazyMonoid lazy_id() {
    return std::numeric_limits<LazyMonoid>::max();
  }
  static Monoid m_merge(const Monoid& a, const Monoid& b) {
    return std::min(a, b);
  }
  static LazyMonoid lazy_merge(const LazyMonoid& a, const LazyMonoid& b) {
    return b == lazy_id() ? a : b;
  }
  static Monoid apply(const Monoid& a, const LazyMonoid& b) {
    return b == lazy_id() ? a : b;
  }
};

template <typename T>
struct RangeMaximumAndUpdateQuery {
  using Monoid = T;
  using LazyMonoid = T;
  static constexpr Monoid m_id() {
    return std::numeric_limits<Monoid>::lowest();
  }
  static constexpr LazyMonoid lazy_id() {
    return std::numeric_limits<LazyMonoid>::lowest();
  }
  static Monoid m_merge(const Monoid& a, const Monoid& b) {
    return std::max(a, b);
  }
  static LazyMonoid lazy_merge(const LazyMonoid& a, const LazyMonoid& b) {
    return b == lazy_id() ? a : b;
  }
  static Monoid apply(const Monoid& a, const LazyMonoid& b) {
    return b == lazy_id() ? a : b;
  }
};

template <typename T, T Inf>
struct RangeMinimumAndAddQuery {
  using Monoid = T;
  using LazyMonoid = T;
  static constexpr Monoid m_id() { return Inf; }
  static constexpr LazyMonoid lazy_id() { return 0; }
  static Monoid m_merge(const Monoid& a, const Monoid& b) {
    return std::min(a, b);
  }
  static LazyMonoid lazy_merge(const LazyMonoid& a, const LazyMonoid& b) {
    return a + b;
  }
  static Monoid apply(const Monoid& a, const LazyMonoid& b) { return a + b; }
};

template <typename T, T Inf>
struct RangeMaximumAndAddQuery {
  using Monoid = T;
  using LazyMonoid = T;
  static constexpr Monoid m_id() { return -Inf; }
  static constexpr LazyMonoid lazy_id() { return 0; }
  static Monoid m_merge(const Monoid& a, const Monoid& b) {
    return std::max(a, b);
  }
  static LazyMonoid lazy_merge(const LazyMonoid& a, const LazyMonoid& b) {
    return a + b;
  }
  static Monoid apply(const Monoid& a, const LazyMonoid& b) { return a + b; }
};

template <typename T>
struct RangeSumAndUpdateQuery {
  using Monoid = struct {
    T sum;
    int len;
  };
  using LazyMonoid = T;
  static std::vector<Monoid> init(const int n) {
    return std::vector<Monoid>(n, Monoid{0, 1});
  }
  static constexpr Monoid m_id() { return {0, 0}; }
  static constexpr LazyMonoid lazy_id() {
    return std::numeric_limits<LazyMonoid>::max();
  }
  static Monoid m_merge(const Monoid& a, const Monoid& b) {
    return Monoid{a.sum + b.sum, a.len + b.len};
  }
  static LazyMonoid lazy_merge(const LazyMonoid& a, const LazyMonoid& b) {
    return b == lazy_id() ? a : b;
  }
  static Monoid apply(const Monoid& a, const LazyMonoid& b) {
    return Monoid{b == lazy_id() ? a.sum : b * a.len, a.len};
  }
};

template <typename T>
struct RangeSumAndAddQuery {
  using Monoid = struct {
    T sum;
    int len;
  };
  using LazyMonoid = T;
  static std::vector<Monoid> init(const int n) {
    return std::vector<Monoid>(n, Monoid{0, 1});
  }
  static constexpr Monoid m_id() { return {0, 0}; }
  static constexpr LazyMonoid lazy_id() { return 0; }
  static Monoid m_merge(const Monoid& a, const Monoid& b) {
    return Monoid{a.sum + b.sum, a.len + b.len};
  }
  static LazyMonoid lazy_merge(const LazyMonoid& a, const LazyMonoid& b) {
    return a + b;
  }
  static Monoid apply(const Monoid& a, const LazyMonoid& b) {
    return Monoid{a.sum + b * a.len, a.len};
  }
};

}  // namespace monoid
namespace monoid {

// 维护：mn = 区间最小覆盖次数，len = 达到 mn 的总长度
template <typename LenT>
struct RangeMinCountAndAddLen {
  struct Monoid {
    int mn;
    LenT len;
  };
  using LazyMonoid = int;  // 覆盖次数的增量 +1/-1

  static constexpr Monoid m_id() { return Monoid{(int)1e9, (LenT)0}; }
  static constexpr LazyMonoid lazy_id() { return 0; }

  static Monoid m_merge(const Monoid& a, const Monoid& b) {
    if (a.mn < b.mn) return a;
    if (b.mn < a.mn) return b;
    return Monoid{a.mn, a.len + b.len};
  }
  static LazyMonoid lazy_merge(const LazyMonoid& a, const LazyMonoid& b) {
    return a + b;
  }
  static Monoid apply(const Monoid& a, const LazyMonoid& b) {
    return Monoid{a.mn + b, a.len};
  }
};

}  // namespace monoid

enum class SweepAxis { X, Y };

template <typename T>
struct AreaOfUnionOfRectangles {
 private:
  struct Rectangle {
    T l, d, r, u;
  };
  std::vector<Rectangle> rectangles;

 public:
  AreaOfUnionOfRectangles() = default;
  explicit AreaOfUnionOfRectangles(int n) { rectangles.reserve(n); }

  void add_rectangle(T l, T d, T r, T u) {
    assert(l < r && d < u);
    rectangles.push_back(Rectangle{l, d, r, u});
  }

  // T2: 面积返回类型（long long / __int128 / long double 等）
  template <typename T2>
  T2 calc(SweepAxis axis = SweepAxis::X) const {
    const int n = (int)rectangles.size();
    if (n == 0) return 0;

    // coord: 被线段树维护的那一维（与扫线轴正交）
    // events: 扫线轴上的事件点
    std::vector<T> coord;
    std::vector<std::tuple<T, int, int>>
        events;  // (pos_on_sweep_axis, rect_id, +1/-1)
    coord.reserve(2 * n);
    events.reserve(2 * n);

    auto get_lo = [&](const Rectangle& r) -> T {
      return (axis == SweepAxis::X ? r.d : r.l);
    };
    auto get_hi = [&](const Rectangle& r) -> T {
      return (axis == SweepAxis::X ? r.u : r.r);
    };
    auto get_enter = [&](const Rectangle& r) -> T {
      return (axis == SweepAxis::X ? r.l : r.d);
    };
    auto get_leave = [&](const Rectangle& r) -> T {
      return (axis == SweepAxis::X ? r.r : r.u);
    };

    for (int i = 0; i < n; i++) {
      const auto& rect = rectangles[i];
      coord.push_back(get_lo(rect));
      coord.push_back(get_hi(rect));
      events.emplace_back(get_enter(rect), i, +1);
      events.emplace_back(get_leave(rect), i, -1);
    }

    std::sort(coord.begin(), coord.end());
    coord.erase(std::unique(coord.begin(), coord.end()), coord.end());
    std::sort(events.begin(), events.end());

    // 线段树叶子表示 [coord[i], coord[i+1]) 小段
    const int m = (int)coord.size() - 1;
    if (m <= 0) return 0;

    std::vector<int> to_lo(n), to_hi(n);
    for (int i = 0; i < n; i++) {
      const auto& rect = rectangles[i];
      const T lo = get_lo(rect), hi = get_hi(rect);
      to_lo[i] = (int)(std::lower_bound(coord.begin(), coord.end(), lo) -
                       coord.begin());
      to_hi[i] =
          (int)(std::lower_bound(coord.begin() + to_lo[i], coord.end(), hi) -
                coord.begin());
    }

    using LenT = T;
    using Tag = monoid::RangeMinCountAndAddLen<LenT>;

    std::vector<typename Tag::Monoid> init(m);
    for (int i = 0; i < m; i++) {
      init[i] = typename Tag::Monoid{0, (LenT)(coord[i + 1] - coord[i])};
    }

    LazySegmentTree<Tag> seg(init);

    const LenT total = (LenT)(coord.back() - coord.front());
    T2 ret = 0;

    for (int i = 0; i + 1 < 2 * n; i++) {
      const auto& [pos, rid, delta] = events[i];
      seg.apply(to_lo[rid], to_hi[rid], delta);

      const auto root = seg.get(0, m);
      const LenT uncovered = (root.mn == 0 ? root.len : (LenT)0);
      const T2 covered = (T2)total - (T2)uncovered;

      const T2 dpos = (T2)(std::get<0>(events[i + 1]) - pos);
      ret += covered * dpos;
    }

    return ret;
  }

  // 沿 sweep axis 的坐标，使得扫线轴之前的并集面积 = 总面积 / 2
  template <typename T2>
  double get_half(SweepAxis axis = SweepAxis::X) const {
    using LD = long double;

    const int n = (int)rectangles.size();
    if (n == 0) return 0.0;

    std::vector<T> coord;
    std::vector<std::tuple<T, int, int>> events;  // (pos, rid, delta)
    coord.reserve(2 * n);
    events.reserve(2 * n);

    auto get_lo = [&](const Rectangle& r) -> T {
      return axis == SweepAxis::X ? r.d : r.l;
    };
    auto get_hi = [&](const Rectangle& r) -> T {
      return axis == SweepAxis::X ? r.u : r.r;
    };
    auto get_enter = [&](const Rectangle& r) -> T {
      return axis == SweepAxis::X ? r.l : r.d;
    };
    auto get_leave = [&](const Rectangle& r) -> T {
      return axis == SweepAxis::X ? r.r : r.u;
    };

    for (int i = 0; i < n; i++) {
      const auto& rect = rectangles[i];
      coord.push_back(get_lo(rect));
      coord.push_back(get_hi(rect));
      events.emplace_back(get_enter(rect), i, +1);
      events.emplace_back(get_leave(rect), i, -1);
    }

    std::sort(coord.begin(), coord.end());
    coord.erase(std::unique(coord.begin(), coord.end()), coord.end());
    std::sort(events.begin(), events.end());

    const int m = (int)coord.size() - 1;
    if (m <= 0) return 0.0;
    if (events.size() < 2) return (double)std::get<0>(events.front());

    std::vector<int> to_lo(n), to_hi(n);
    for (int i = 0; i < n; i++) {
      const auto& rect = rectangles[i];
      const T lo = get_lo(rect), hi = get_hi(rect);
      to_lo[i] = (int)(std::lower_bound(coord.begin(), coord.end(), lo) -
                       coord.begin());
      to_hi[i] =
          (int)(std::lower_bound(coord.begin() + to_lo[i], coord.end(), hi) -
                coord.begin());
    }

    using LenT = T;
    using Tag = monoid::RangeMinCountAndAddLen<LenT>;

    std::vector<typename Tag::Monoid> init(m);
    for (int i = 0; i < m; i++) {
      init[i] = typename Tag::Monoid{0, (LenT)(coord[i + 1] - coord[i])};
    }
    LazySegmentTree<Tag> seg(init);

    const LenT total = (LenT)(coord.back() - coord.front());

    // records[i] = {area_before_strip_i, covered_len_on_strip_i}
    std::vector<std::pair<T2, T2>> records(events.size() - 1);

    T2 area = 0;
    for (int i = 0; i + 1 < (int)events.size(); i++) {
      const auto& [pos, rid, delta] = events[i];
      seg.apply(to_lo[rid], to_hi[rid], delta);

      const auto root = seg.get(0, m);
      const LenT uncovered = (root.mn == 0 ? root.len : (LenT)0);
      const T2 covered = (T2)total - (T2)uncovered;

      records[i] = {area, covered};

      const T2 dpos = (T2)(std::get<0>(events[i + 1]) - pos);
      area += covered * dpos;
    }

    const T2 totArea = area;
    const T2 half2 = totArea;

    // 找到最大的 idx，使得 records[idx].first * 2 < totArea
    int idx =
        (int)(std::lower_bound(records.begin(), records.end(), half2,
                               [&](const std::pair<T2, T2>& p, const T2& v) {
                                 return p.first * 2 < v;
                               }) -
              records.begin()) -
        1;

    if (idx < 0) idx = 0;

    const auto [area_before, sum_len] = records[idx];
    const T pos0 = std::get<0>(events[idx]);

    // 需要保证 sum_len > 0（理论上 half 落点必在 sum_len>0 的strip好像？）
    if (sum_len == 0) return (double)pos0;

    // area_before + sum_len * (ans - pos0) = totArea / 2
    // 等价：ans = pos0 + (totArea - 2*area_before) / (2*sum_len)
    LD ans = (LD)pos0 + (LD)(totArea - area_before * 2) / (LD)(sum_len * 2);
    return (double)ans;
  }
};