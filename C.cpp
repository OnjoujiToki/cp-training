#include <chrono>
#include <cstdint>
#include <iostream>
#include <mutex>
#include <optional>
#include <queue>
#include <string>
#include <thread>
#include <unordered_map>

class HitCounterKV {
private:
    static constexpr int kWindowSeconds = 5;  // 5 minutes; set to 5 for quick demos

    struct Bucket {
        int64_t ts;
        int     cnt;
    };

    std::queue<Bucket> get_events_;
    std::queue<Bucket> put_events_;
    int get_total_ = 0;  // rolling # of GETs in window
    int put_total_ = 0;  // rolling # of PUTs in window

    std::unordered_map<std::string, std::string> kv_;

    static int64_t now_seconds() {
        return std::chrono::duration_cast<std::chrono::seconds>(
                   std::chrono::system_clock::now().time_since_epoch())
            .count();
    }

    static void prune(std::queue<Bucket>& q, int& total, int64_t current) {
        // Evict buckets that are strictly older than [current - window + 1, current]
        while (!q.empty() && q.front().ts <= current - kWindowSeconds) {
            total -= q.front().cnt;
            q.pop();
        }
    }

    static void record(std::queue<Bucket>& q, int& total, int64_t t) {
        // Coalesce multiple events with the same second into the back bucket
        if (!q.empty() && q.back().ts == t) {
            q.back().cnt += 1;
        } else {
            q.push(Bucket{t, 1});
        }
        total += 1;
    }

    void record_get_locked(int64_t t) {
        prune(get_events_, get_total_, t);
        record(get_events_, get_total_, t);
    }
    void record_put_locked(int64_t t) {
        prune(put_events_, put_total_, t);
        record(put_events_, put_total_, t);
    }

public:
    // Insert or overwrite key with value. Returns true if inserted or updated.
    bool put(const std::string& key, std::string value) {
        const int64_t t = now_seconds();
        record_put_locked(t);
        kv_[key] = std::move(value);
        return true;
    }

    // Get value by key. Returns std::nullopt if not found.
    std::optional<std::string> get(const std::string& key) {
        const int64_t t = now_seconds();
        record_get_locked(t);
        auto it = kv_.find(key);
        if (it == kv_.end()) return std::nullopt;
        return it->second;
    }

    // Average calls/second within the last kWindowSeconds.
    double measure_get_load() {
        const int64_t t = now_seconds();
        prune(get_events_, get_total_, t);
        return static_cast<double>(get_total_) / static_cast<double>(kWindowSeconds);
    }

    double measure_put_load() {
        const int64_t t = now_seconds();
        prune(put_events_, put_total_, t);
        return static_cast<double>(put_total_) / static_cast<double>(kWindowSeconds);
    }
};

// ---------------- Demo ----------------
int main() {
    HitCounterKV counter;

    counter.put("a", "1");
    auto v = counter.get("a");
    std::cout << "[T1] a=" << (v ? *v : "<missing>") << "\n";
    std::cout << "[T1] get_load=" << counter.measure_get_load()
              << " put_load=" << counter.measure_put_load() << "\n";

    // Bursty puts: multiple in the same second coalesce into one bucket
    for (int i = 0; i < 5; ++i) counter.put("k" + std::to_string(i), std::to_string(i));
    std::cout << "[T2] put_load=" << counter.measure_put_load() << "\n";

    // Not-found path
    auto miss = counter.get("nonexistent");
    std::cout << "[T3] get(nonexistent)=" << (miss ? *miss : "<missing>") << "\n";

    // Pruning check (window = 5s)
    std::this_thread::sleep_for(std::chrono::seconds(6));
    std::cout << "[T4] get_load=" << counter.measure_get_load()
              << " put_load=" << counter.measure_put_load() << "\n";

    return 0;
}
