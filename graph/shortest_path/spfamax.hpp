template <typename T>
struct SpfaMax {
  using edge = std::pair<T, int>;  // weight & vertex id
  const T NINF = std::numeric_limits<T>::lowest() / 2;
  const T INF = std::numeric_limits<T>::max() / 2;
  int n;
  std::vector<std::vector<edge>> edges;

  SpfaMax(int _n) : n(_n), edges(n) {}

  void add_edge(int u, int v, T weight) { edges[u].emplace_back(weight, v); }

  // 返回 {距离数组, 是否受正环影响数组}
  std::pair<std::vector<T>, std::vector<bool>> solve(int s) {
    std::vector<T> dist(n, NINF);
    std::vector<int> cnt(n, 0);
    std::vector<bool> in_q(n, false);
    std::vector<bool> on_cyc(n, false);
    std::queue<int> q;

    dist[s] = 0;
    q.emplace(s);
    in_q[s] = true;

    while (!q.empty()) {
      int u = q.front();
      q.pop();
      in_q[u] = false;

      for (auto [w, v] : edges[u]) {
        if (on_cyc[v]) continue;

        // 最长路松弛
        if (dist[v] < dist[u] + w) {
          dist[v] = dist[u] + w;
          cnt[v] = cnt[u] + 1;

          // 记录正环
          if (cnt[v] >= n) {
            on_cyc[v] = true;
          }

          if (!in_q[v] && !on_cyc[v]) {
            q.emplace(v);
            in_q[v] = true;
          }
        }
      }
    }

    // 标记所有能从正环到达的点
    std::queue<int> cq;
    for (int i = 0; i < n; ++i) {
      if (on_cyc[i]) cq.push(i);
    }

    while (!cq.empty()) {
      int u = cq.front();
      cq.pop();
      for (auto [w, v] : edges[u]) {
        if (!on_cyc[v]) {
          on_cyc[v] = true;
          cq.push(v);
        }
      }
    }

    return {dist, on_cyc};
  }
};