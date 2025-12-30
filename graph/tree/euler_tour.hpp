struct EulerTour {
  int n;
  std::vector<std::vector<int>> g;
  std::vector<int> tin, tout, parent;
  int timer = 0;

  explicit EulerTour(int n)
      : n(n), g(n + 1), tin(n + 1), tout(n + 1), parent(n + 1, 0) {}

  void add_edge(int u, int v) {
    g[u].push_back(v);
    g[v].push_back(u);
  }

  void build(int root = 0) {
    timer = 0;
    dfs(root, 0);
  }

  // [l, r)
  std::pair<int, int> subtree_range(int u) const { return {tin[u], tout[u]}; }

  // a 是否是 b 的祖先（包含 a==b的情况）
  bool is_ancestor(int a, int b) const {
    return tin[a] <= tin[b] && tout[b] <= tout[a];
  }

  // 无向边 (u, v) 中，返回子节点那一端（assume u,v 相邻）
  int child_of_edge(int u, int v) const {
    if (parent[u] == v) return u;
    if (parent[v] == u) return v;

    throw std::logic_error("child_of_edge: (u,v) is not a tree edge");
  }

 private:
  void dfs(int u, int p) {
    parent[u] = p;
    tin[u] = timer++;
    for (int v : g[u])
      if (v != p) dfs(v, u);
    tout[u] = timer;
  }
};