
template <typename T>
struct DoublingLowestCommonAncestor {
  std::vector<std::vector<std::pair<int, T>>> g;

  std::vector<int> dep;
  std::vector<T> sum;
  std::vector<std::vector<int>> table;
  const int LOG;

  explicit DoublingLowestCommonAncestor(int n)
      : g(n), LOG(32 - __builtin_clz(n)) {}

  void add_edge(int u, int v, T w = 1) {
    g[u].emplace_back(v, w);
    g[v].emplace_back(u, w);
  }

  void build(int root = 0) {
    dep.assign(g.size(), 0);
    sum.assign(g.size(), 0);
    table.assign(LOG, std::vector<int>(g.size(), -1));
    dfs(root, -1, 0);
    for (int k = 0; k + 1 < LOG; k++) {
      for (int i = 0; i < (int)table[k].size(); i++) {
        if (table[k][i] == -1)
          table[k + 1][i] = -1;
        else
          table[k + 1][i] = table[k][table[k][i]];
      }
    }
  }

   /* credit: 草莓奶昔 https://leetcode.cn/problems/minimum-weighted-subgraph-with-the-required-paths-ii/
   *   tree be re-rooted at node r.
   *   This function returns the Lowest lca of u and v
   *   under that new root.
   *
   * 等价于
   *       build(r);
   *       return lca(u, v);
   * 树上三点即为 dist(u, w) + dist(v, w) - dist(new lca, w);
   */

  int rooted_lca(int u, int v, int r) {
    return lca(u, v) ^ lca(u, r) ^ lca(v, r);
  }
  
  int lca(int u, int v) {
    if (dep[u] > dep[v]) std::swap(u, v);
    v = climb(v, dep[v] - dep[u]);
    if (u == v) return u;
    for (int i = LOG - 1; i >= 0; i--) {
      if (table[i][u] != table[i][v]) {
        u = table[i][u];
        v = table[i][v];
      }
    }
    return table[0][u];
  }

  int climb(int u, int k) {
    if (dep[u] < k) return -1;
    for (int i = LOG - 1; i >= 0; i--) {
      if ((k >> i) & 1) u = table[i][u];
    }
    return u;
  }

  T dist(int u, int v) { return sum[u] + sum[v] - 2 * sum[lca(u, v)]; }

  T steiner3(int a, int b, int c) {
    return (dist(a, b) + dist(b, c) + dist(c, a)) / 2;
  }

 private:
  void dfs(int idx, int par, int d) {
    table[0][idx] = par;
    dep[idx] = d;
    for (auto& to : g[idx]) {
      if (to.first != par) {
        sum[to.first] = sum[idx] + to.second;
        dfs(to.first, idx, d + 1);
      }
    }
  }
};