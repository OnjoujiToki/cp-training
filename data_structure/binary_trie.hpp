template <int B = 32, typename T = std::uint32_t>
struct BinaryTrie {
  struct Node {
    Node* nxt[2];
    int child;                       // # elements in this subtree
    Node() : nxt{nullptr, nullptr}, child(0) {}
  };

  Node* root;

  BinaryTrie() : root(nullptr) {}

  void clear() {   
    root = nullptr;
  }

  bool empty() const { return root == nullptr; }
  int  size()  const { return root ? root->child : 0; }

  Node* insert(const T& x) {
    if (!root) root = new Node();
    Node* node = root;
    ++node->child;
    for (int b = B - 1; b >= 0; --b) {
      const bool d = (x >> b) & 1;
      if (!node->nxt[d]) node->nxt[d] = new Node();
      node = node->nxt[d];
      ++node->child;
    }
    return node;
  }

  void erase(const T& x) {
    if (!root) return;
    Node* node = root;
    for (int b = B - 1; b >= 0; --b) {
      const bool d = (x >> b) & 1;
      if (!node->nxt[d]) return;     
      node = node->nxt[d];
    }
    erase_counts(root, x, B - 1);
    if (root && root->child == 0) root = nullptr;  
  }

  Node* find(const T& x) const {
    if (!root) return nullptr;
    Node* node = root;
    for (int b = B - 1; b >= 0; --b) {
      const bool d = (x >> b) & 1;
      if (!node->nxt[d]) return nullptr;
      node = node->nxt[d];
    }
    return node;
  }

  int less_than(const T& x) const {
    int res = 0;
    Node* node = root;
    for (int b = B - 1; node && b >= 0; --b) {
      const bool d = (x >> b) & 1;
      if (d && node->nxt[0]) res += node->nxt[0]->child;
      node = node->nxt[d];
    }
    return res;
  }

  int count(const T& l, const T& r) const { return less_than(r) - less_than(l); }

  int count(const T& x) const {
    Node* p = find(x);
    return p ? p->child : 0;
  }

  std::pair<Node*, T> operator[](int n) const { return find_nth(n, 0); }

  std::pair<Node*, T> find_nth(int n, const T& x) const {
    assert(0 <= n && n < size());
    Node* node = root;
    T res = 0;
    for (int b = B - 1; b >= 0; --b) {
      bool d = (x >> b) & 1;
      const int left = node->nxt[d] ? node->nxt[d]->child : 0;
      if (n >= left) { n -= left; d ^= 1; }
      node = node->nxt[d];
      if (d) res |= (T(1) << b);
    }
    return {node, res};
  }

  std::pair<Node*, std::optional<T>> lower_bound(const T& x) const {
    const int lt = less_than(x);
    if (lt == size()) return {nullptr, std::nullopt};
    auto res = find_nth(lt, 0);
    return {res.first, std::optional<T>(res.second)};
  }

  std::pair<Node*, std::optional<T>> upper_bound(const T& x) const {
    return lower_bound(x + 1);
  }

  std::pair<Node*, T> max_element(const T& x = 0) const { return min_element(~x); }

  std::pair<Node*, T> min_element(const T& x = 0) const {
    assert(root);
    Node* node = root;
    T res = 0;
    for (int b = B - 1; b >= 0; --b) {
      bool d = (x >> b) & 1;
      if (!node->nxt[d]) d ^= 1;
      node = node->nxt[d];
      if (d) res |= (T(1) << b);
    }
    return {node, res};
  }

  // Count y in trie such that (x ^ y) < K
  int count_xor_less_with(T x, T K) const {
    if (!root) return 0;
    Node* node = root;
    int res = 0;
    for (int b = B - 1; b >= 0 && node; --b) {
      const int xb = (x >> b) & 1;
      const int kb = (K >> b) & 1;
      if (kb == 1) {
        Node* same = node->nxt[xb];
        if (same) res += same->child;
        node = node->nxt[xb ^ 1];
      } else {
        node = node->nxt[xb];
      }
    }
    return res;
  }

private:
  static void erase_counts(Node* node, const T& x, int b) {
    // node is non-null and path exists
    --node->child;
    if (b == -1) return;
    const bool d = (x >> b) & 1;
    erase_counts(node->nxt[d], x, b - 1);
  }
};

