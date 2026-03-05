struct TupleHash {
    template <class T>
    static void hash_combine(std::size_t& seed, const T& v) {
      seed ^= std::hash<T>{}(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
  
    template <class... Ts>
    std::size_t operator()(const std::tuple<Ts...>& t) const noexcept {
      std::size_t seed = 0;
      std::apply([&](const auto&... xs) { (hash_combine(seed, xs), ...); }, t);
      return seed;
    }
  };


// unordered_map<tuple<int, long long>, int, TupleHash> cnt;