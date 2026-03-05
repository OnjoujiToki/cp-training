struct SplitMix64 {
  static inline uint64_t mix(uint64_t x) {
    x += 0x9e3779b97f4a7c15ULL;
    x = (x ^ (x >> 30)) * 0xbf58476d1ce4e5b9ULL;
    x = (x ^ (x >> 27)) * 0x94d049bb133111ebULL;
    return x ^ (x >> 31);
  }
};

template <std::size_t N>
struct ArrayHasher {
  std::size_t operator()(const std::array<int, N>& a) const noexcept {
    uint64_t h = 0x243f6a8885a308d3ULL ^ SplitMix64::mix(N);
    for (std::size_t i = 0; i < N; ++i) {
      uint64_t x = static_cast<uint32_t>(a[i]);  // avoid sign-extension issues
      uint64_t v = x ^ (uint64_t(i) * 0x9e3779b97f4a7c15ULL);
      h = SplitMix64::mix(h ^ SplitMix64::mix(v + 0x9e3779b97f4a7c15ULL));
    }
    return static_cast<std::size_t>(h);
  }
};

/* usage
using Cnt = std::array<int, 50>;
std::unordered_set<Cnt, ArrayHasher<50>> vis;
*/