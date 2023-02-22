template <int char_size, int base>
struct Tri {
  struct Node {             // 頂点を表す構造体
    std::vector<int> next;  // 子の頂点番号を格納。存在しなければ-1
    std::vector<int> accept;  // 末端がこの頂点になる単語の word_id を保存
    int c;  // base からの間隔をint型で表現したもの
    int common;  // いくつの単語がこの頂点を共有しているか prefix only..
    Node(int c_) : c(c_), common(0) { next.assign(char_size, -1); }
  };
  std::vector<Node> nodes;  // trie 木本体
  int root;
  Tri() : root(0) { nodes.push_back(Node(root)); }
  // 単語の挿入
  void insert(const std::vector<int> &word, int word_id) {
    int node_id = 0;
    for (int i = 0; i < (int)word.size(); i++) {
      int c = word[i] - base;
      int &next_id = nodes[node_id].next[c];
      if (next_id == -1) {  // 次の頂点が存在しなければ追加
        next_id = (int)nodes.size();
        nodes.push_back(Node(c));
      }
      ++nodes[node_id].common;
      node_id = next_id;
    }
    ++nodes[node_id].common;
    nodes[node_id].accept.push_back(word_id);
  }
  void insert(const std::vector<int> &word) { insert(word, nodes[0].common); }
  // 単語とprefixの検索
  int find_lcp(const std::vector<int> &word) {
    int node_id = 0;
    int ans = 0;
    for (int i = 0; i < (int)word.size(); i++) {
      int c = word[i] - base;
      int &next_id = nodes[node_id].next[c];
      if (next_id == -1) {  // 次の頂点が存在しなければ終了
        break;
      }
      ans++;

      node_id = next_id;
    }

    return ans;
  }
  int search(const std::vector<int> &word, bool prefix = false) {
    int node_id = 0;
    for (int i = 0; i < (int)word.size(); i++) {
      int c = word[i] - base;
      int &next_id = nodes[node_id].next[c];
      if (next_id == -1) {  // 次の頂点が存在しなければ終了
        return -1;
      }
      node_id = next_id;
    }
    bool ok = (prefix) ? true : nodes[node_id].accept.size() > 0;
    return ok ? node_id : -1;
  }
  // prefix を持つ単語が存在するかの検索
  int start_with(const std::vector<int> &prefix) {
    return search(prefix, true);
  }
  // 挿入した単語の数
  int count() const { return (nodes[0].common); }
  // Trie木のノード数
  int size() const { return ((int)nodes.size()); }
};
