// modified from emthrm


template <typename CostType>
struct Edge {
  CostType cost;
  int src, dst;

  explicit Edge(const int src, const int dst, const CostType cost = 0)
      : cost(cost), src(src), dst(dst) {}

  auto operator<=>(const Edge& x) const = default;
};

template <typename CostType>
struct HeavyLightDecomposition {
  std::vector<int> parent, subtree, id, inv, head;
  std::vector<CostType> cost;

  explicit HeavyLightDecomposition(
      const std::vector<std::vector<Edge<CostType>>>& graph, const int root = 0)
      : graph(graph) {
    const int n = graph.size();
    parent.assign(n, -1);
    subtree.assign(n, 1);
    dfs1(root);
    id.resize(n);
    inv.resize(n);
    head.assign(n, root);
    int cur_id = 0;
    dfs2(root, &cur_id);
  }

  template <typename Fn>
  void update_v(int u, int v, const Fn f) const {
    while (true) {
      if (id[u] > id[v]) std::swap(u, v);
      f(std::max(id[head[v]], id[u]), id[v] + 1);
      if (head[u] == head[v]) break;
      v = parent[head[v]];
    }
  }
  template <typename F, typename G, typename T>
  T query_v(int u, int v, const F f, const G g, const T id_t) const {
    T left = id_t, right = id_t;
    while (true) {
      if (id[u] > id[v]) {
        std::swap(u, v);
        std::swap(left, right);
      }
      // 在 v 这一侧的链上，查询 [max(id[head[v]], id[u]), id[v]] 这些点
      left = g(left, f(std::max(id[head[v]], id[u]), id[v] + 1));
      if (head[u] == head[v]) break;
      v = parent[head[v]];
    }
    return g(left, right);
  }

  template <typename Fn>
  void update_subtree_v(const int ver, const Fn f) const {
    f(id[ver], id[ver] + subtree[ver]);
  }

  template <typename T, typename Fn>
  T query_subtree_v(const int ver, const Fn f) const {
    return f(id[ver], id[ver] + subtree[ver]);
  }

  template <typename Fn>
  void update_e(int u, int v, const Fn f) const {
    while (true) {
      if (id[u] > id[v]) std::swap(u, v);
      if (head[u] == head[v]) {
        f(id[u], id[v]);
        break;
      } else {
        f(id[head[v]] - 1, id[v]);
        v = parent[head[v]];
      }
    }
  }

  template <typename F, typename G, typename T>
  T query_e(int u, int v, const F f, const G g, const T id_t) const {
    T left = id_t, right = id_t;
    while (true) {
      if (id[u] > id[v]) {
        std::swap(u, v);
        std::swap(left, right);
      }
      if (head[u] == head[v]) {
        left = g(left, f(id[u], id[v]));
        break;
      } else {
        left = g(left, f(id[head[v]] - 1, id[v]));
        v = parent[head[v]];
      }
    }
    return g(left, right);
  }

  template <typename Fn>
  void update_subtree_e(const int ver, const Fn f) const {
    f(id[ver], id[ver] + subtree[ver] - 1);
  }

  template <typename T, typename Fn>
  T query_subtree_e(const int ver, const Fn f) const {
    return f(id[ver], id[ver] + subtree[ver] - 1);
  }

  // 对树边 (u, v) 做一次单边更新：调用 f(pos)
  // 其中 pos 是该边在“边序列”里的唯一位置（本实现为 id[child] - 1）。
  template <typename Fn>
  void update_single_edge(int u, int v, const Fn& f) const {
    if (parent[u] == v) std::swap(u, v);
    const int pos = id[v] - 1;
    f(pos);
  }

  int lowest_common_ancestor(int u, int v) const {
    while (true) {
      if (id[u] > id[v]) std::swap(u, v);
      if (head[u] == head[v]) break;
      v = parent[head[v]];
    }
    return u;
  }

 private:
  std::vector<std::vector<Edge<CostType>>> graph;

  void dfs1(const int ver) {
    for (int i = 0; std::cmp_less(i, graph[ver].size()); ++i) {
      Edge<CostType>& e = graph[ver][i];
      if (e.dst != parent[ver]) {
        parent[e.dst] = ver;
        dfs1(e.dst);
        subtree[ver] += subtree[e.dst];
        if (subtree[e.dst] > subtree[graph[ver].front().dst]) {
          std::swap(e, graph[ver].front());
        }
      }
    }
  }

  void dfs2(const int ver, int* cur_id) {
    id[ver] = (*cur_id)++;
    inv[id[ver]] = ver;
    for (const Edge<CostType>& e : graph[ver]) {
      if (e.dst != parent[ver]) {
        head[e.dst] = (e.dst == graph[ver].front().dst ? head[ver] : e.dst);
        cost.emplace_back(e.cost);
        dfs2(e.dst, cur_id);
      }
    }
  }
};
