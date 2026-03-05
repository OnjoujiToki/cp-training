
namespace internal {

template <class E>
struct csr {
  std::vector<int> start;
  std::vector<E> elist;
  explicit csr(int n, const std::vector<std::pair<int, E>>& edges)
      : start(n + 1), elist(edges.size()) {
    for (auto e : edges) {
      start[e.first + 1]++;
    }
    for (int i = 1; i <= n; i++) {
      start[i] += start[i - 1];
    }
    auto counter = start;
    for (auto e : edges) {
      elist[counter[e.first]++] = e.second;
    }
  }
};

}  // namespace internal

struct scc_graph {
 public:
  explicit scc_graph(int n) : _n(n) {}

  int num_vertices() { return _n; }

  void add_edge(int from, int to) { edges.push_back({from, {to}}); }

  // @return pair of (# of scc, scc id)
  std::pair<int, std::vector<int>> scc_ids() {
    auto g = internal::csr<edge>(_n, edges);
    int now_ord = 0, group_num = 0;
    std::vector<int> visited, low(_n), ord(_n, -1), ids(_n);
    visited.reserve(_n);
    auto dfs = [&](auto self, int v) -> void {
      low[v] = ord[v] = now_ord++;
      visited.push_back(v);
      for (int i = g.start[v]; i < g.start[v + 1]; i++) {
        auto to = g.elist[i].to;
        if (ord[to] == -1) {
          self(self, to);
          low[v] = std::min(low[v], low[to]);
        } else {
          low[v] = std::min(low[v], ord[to]);
        }
      }
      if (low[v] == ord[v]) {
        while (true) {
          int u = visited.back();
          visited.pop_back();
          ord[u] = _n;
          ids[u] = group_num;
          if (u == v) break;
        }
        group_num++;
      }
    };
    for (int i = 0; i < _n; i++) {
      if (ord[i] == -1) dfs(dfs, i);
    }
    for (auto& x : ids) {
      x = group_num - 1 - x;
    }
    return {group_num, ids};
  }

  std::vector<std::vector<int>> scc() {
    auto ids = scc_ids();
    int group_num = ids.first;
    std::vector<int> counts(group_num);
    for (auto x : ids.second) counts[x]++;
    std::vector<std::vector<int>> groups(ids.first);
    for (int i = 0; i < group_num; i++) {
      groups[i].reserve(counts[i]);
    }
    for (int i = 0; i < _n; i++) {
      groups[ids.second[i]].push_back(i);
    }
    return groups;
  }

  // Build SCC DAG directly.
  // Return: (scc_num, comp[v], dag[comp] adjacency). SCC ids are topo-sorted.
  std::tuple<int, std::vector<int>, std::vector<std::vector<int>>>
  scc_dag_ids() {
    auto [group_num, comp] = scc_ids();

    std::vector<std::vector<int>> tmp(group_num);
    tmp.assign(group_num, {});
    for (auto& e : edges) {
      int u = e.first;
      int v = e.second.to;
      int cu = comp[u], cv = comp[v];
      if (cu == cv) continue;
      tmp[cu].push_back(cv);
    }

    std::vector<std::vector<int>> dag(group_num);
    for (int c = 0; c < group_num; c++) {
      auto& t = tmp[c];
      std::sort(t.begin(), t.end());
      t.erase(std::unique(t.begin(), t.end()), t.end());
      dag[c] = std::move(t);
    }
    return {group_num, comp, dag};
  }

  // also compute indegree of SCC DAG (after dedup).
  std::tuple<int, std::vector<int>, std::vector<std::vector<int>>,
             std::vector<int>>
  scc_dag_ids_with_indeg() {
    auto [group_num, comp, dag] = scc_dag_ids();
    std::vector<int> indeg(group_num, 0);
    for (int u = 0; u < group_num; u++) {
      for (int v : dag[u]) indeg[v]++;
    }
    return {group_num, comp, dag, indeg};
  }

 private:
  int _n;
  struct edge {
    int to;
  };
  std::vector<std::pair<int, edge>> edges;
};
