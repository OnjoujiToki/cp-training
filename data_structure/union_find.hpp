template <typename T>
struct DSU {
  std::vector<T> f, siz;
  DSU(int n) : f(n), siz(n, 1) { std::iota(f.begin(), f.end(), 0); }
  T leader(T x) {
    while (x != f[x]) x = f[x] = f[f[x]];
    return x;
  }
  bool same(T x, T y) { return leader(x) == leader(y); }
  bool merge(T x, T y) {
    x = leader(x);
    y = leader(y);
    if (x == y) return false;
    siz[x] += siz[y];
    f[y] = x;
    return true;
  }
  T size(int x) { return siz[leader(x)]; }
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
                 const std::function<void(int, int)> &beforeMerge = nullptr) {
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
                 const std::function<void(int, int)> &beforeMerge = nullptr) {
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