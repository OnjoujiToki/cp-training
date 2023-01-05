template <typename T>
struct MST {
  struct Edge {
    int u, v;
    T w;
    Edge() {}
    Edge(int _u, int _v, T _w) : u(_u), v(_v), w(_w) {}
    bool operator<(const Edge &rhs) const { return w < rhs.w; }
  };

  int n;
  DSU<int> dsu;
  std::vector<Edge> edges;
  MST(int _n) : n(_n), dsu(_n) {}

  void add_edge(int u, int v, T w) { edges.emplace_back(u, v, w); }

  T get_min() {
    T ret = 0;
    std::sort(edges.begin(), edges.end());
    for (auto [u, v, w] : edges) {
      if (dsu.merge(u, v)) {
        ret += w;
      }
    }
    return ret;
  }
};