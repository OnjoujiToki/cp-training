struct dsu {
 public:
  dsu() : _n(0) {}
  explicit dsu(int n) : _n(n), parent_or_size(n, -1) {}

  int merge(int a, int b) {
    assert(0 <= a && a < _n);
    assert(0 <= b && b < _n);
    int x = leader(a), y = leader(b);
    if (x == y) return x;
    if (-parent_or_size[x] < -parent_or_size[y]) std::swap(x, y);
    parent_or_size[x] += parent_or_size[y];
    parent_or_size[y] = x;
    return x;
  }

  bool same(int a, int b) {
    assert(0 <= a && a < _n);
    assert(0 <= b && b < _n);
    return leader(a) == leader(b);
  }

  int leader(int a) {
    assert(0 <= a && a < _n);
    return _leader(a);
  }

  int size(int a) {
    assert(0 <= a && a < _n);
    return -parent_or_size[leader(a)];
  }

  std::vector<std::vector<int>> groups() {
    std::vector<int> leader_buf(_n), group_size(_n);
    for (int i = 0; i < _n; i++) {
      leader_buf[i] = leader(i);
      group_size[leader_buf[i]]++;
    }
    std::vector<std::vector<int>> result(_n);
    for (int i = 0; i < _n; i++) {
      result[i].reserve(group_size[i]);
    }
    for (int i = 0; i < _n; i++) {
      result[leader_buf[i]].push_back(i);
    }
    result.erase(
        std::remove_if(result.begin(), result.end(),
                       [&](const std::vector<int>& v) { return v.empty(); }),
        result.end());
    return result;
  }

 private:
  int _n;
  // root node: -1 * component size
  // otherwise: parent
  std::vector<int> parent_or_size;

  int _leader(int a) {
    if (parent_or_size[a] < 0) return a;
    return parent_or_size[a] = _leader(parent_or_size[a]);
  }
};

struct DSU {
  std::vector<int> f, siz, l, r;
  DSU(int n) : f(n), siz(n, 1), l(n), r(n) {
    std::iota(f.begin(), f.end(), 0);
    std::iota(l.begin(), l.end(), 0);
    std::iota(r.begin(), r.end(), 0);
  }
  int leader(int x) {
    while (x != f[x]) x = f[x] = f[f[x]];
    return x;
  }
  bool same(int x, int y) { return leader(x) == leader(y); }
  bool merge(int x, int y) {
    x = leader(x);
    y = leader(y);
    if (x == y) return false;
    siz[x] += siz[y];
    f[y] = x;
    l[x] = std::min(l[x], l[y]);
    r[x] = std::max(r[x], r[y]);

    return true;
  }
  int get_left(int x) { return l[leader(x)]; }
  int get_right(int x) { return r[leader(x)]; }
  int size(int x) { return siz[leader(x)]; }
};

// credit 草莓奶昔
struct UnionFindRange {
  int part;
  int _n;
  std::vector<int> _parent;
  std::vector<int> _rank;

  UnionFindRange(int n) : part(n), _n(n), _parent(n), _rank(n, 1) {
    std::iota(_parent.begin(), _parent.end(), 0);
  }

  int find(int x) {
    while (x != _parent[x]) {
      _parent[x] = _parent[_parent[x]];
      x = _parent[x];
    }
    return x;
  }

  bool unionSets(int x, int y,
                 const std::function<void(int, int)>& beforeMerge = nullptr) {
    if (x < y) {
      std::swap(x, y);
    }
    int rootX = find(x);
    int rootY = find(y);
    if (rootX == rootY) {
      return false;
    }
    if (beforeMerge) {
      beforeMerge(rootY, rootX);
    }
    _parent[rootX] = rootY;
    _rank[rootY] += _rank[rootX];
    --part;
    return true;
  }

  int unionRange(int left, int right,
                 const std::function<void(int, int)>& beforeMerge = nullptr) {
    if (left >= right) {
      return 0;
    }
    int leftRoot = find(left);
    int rightRoot = find(right);
    int unionCount = 0;
    while (rightRoot != leftRoot) {
      ++unionCount;
      unionSets(rightRoot, rightRoot - 1, beforeMerge);
      rightRoot = find(rightRoot - 1);
    }
    return unionCount;
  }

  bool same(int x, int y) { return find(x) == find(y); }

  int getSize(int x) { return _rank[find(x)]; }
};