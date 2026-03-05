#include <random>
template <class State>
class SA {
 public:
  struct Config {
    double t0 = 100;
    double t_end = 1e-7;
    double alpha = 0.87;
    int steps_per_temp = 40;
    int restarts = 120;
    uint64_t seed = 0;
  };

  using Energy = std::function<double(const State&)>;
  using Neighbor = std::function<State(const State&, double, mt19937_64&)>;
  using Init = std::function<State(mt19937_64&)>;
  using Repair = std::function<void(State&)>;
  struct Event {
    enum class Type {
      RestartBegin,
      TempBegin,
      Proposal,
      Accepted,
      BestImproved,
      RestartEnd,
      Finished
    } type;
    int restart_idx = 0;
    double t = 0.0;
    int step_in_temp = 0;
    double cur_e = 0.0, nxt_e = 0.0, best_e = 0.0, delta = 0.0;
    bool accepted = false;
  };
  using Callback = std::function<bool(const Event&)>;  // 返回 true 表示停止

  explicit SA(Config cfg = {}) : cfg_(cfg) {
    if (cfg_.seed == 0)
      rng_.seed(random_device{}());
    else
      rng_.seed(cfg_.seed);
    repair_ = nullptr;
    callback_ = nullptr;
  }

  void set_energy(Energy e) { energy_ = std::move(e); }
  void set_neighbor(Neighbor n) { neighbor_ = std::move(n); }
  void set_init(Init i) { init_ = std::move(i); }

  // 可选 hook：不调用 set_ 就不会启用
  void set_repair(Repair r) { repair_ = std::move(r); }
  void clear_repair() { repair_ = nullptr; }
  void set_callback(Callback cb) { callback_ = std::move(cb); }
  void clear_callback() { callback_ = nullptr; }

  std::pair<State, double> run() {
    assert(energy_ && neighbor_ && init_);

    State best_state{};
    double best_e = numeric_limits<double>::infinity();
    bool stop = false;

    for (int rep = 0; rep < cfg_.restarts && !stop; ++rep) {
      emit({Event::Type::RestartBegin, rep, cfg_.t0, 0, 0, 0, best_e, 0, false},
           stop);

      State cur = init_(rng_);
      apply_repair(cur);
      double cur_e = energy_(cur);

      if (cur_e < best_e) {
        best_e = cur_e;
        best_state = cur;
        emit({Event::Type::BestImproved, rep, cfg_.t0, 0, cur_e, 0, best_e, 0,
              false},
             stop);
      }

      for (double t = cfg_.t0; t > cfg_.t_end && !stop; t *= cfg_.alpha) {
        emit({Event::Type::TempBegin, rep, t, 0, cur_e, 0, best_e, 0, false},
             stop);

        for (int s = 0; s < cfg_.steps_per_temp && !stop; ++s) {
          State nxt = neighbor_(cur, t, rng_);
          apply_repair(nxt);
          double nxt_e = energy_(nxt);
          double delta = nxt_e - cur_e;

          bool accepted = decide_accept(delta, t);
          emit({Event::Type::Proposal, rep, t, s, cur_e, nxt_e, best_e, delta,
                accepted},
               stop);

          if (accepted) {
            cur = std::move(nxt);
            cur_e = nxt_e;
            emit({Event::Type::Accepted, rep, t, s, cur_e, nxt_e, best_e, delta,
                  true},
                 stop);

            if (cur_e < best_e) {
              best_e = cur_e;
              best_state = cur;
              emit({Event::Type::BestImproved, rep, t, s, cur_e, 0, best_e, 0,
                    false},
                   stop);
            }
          }
        }
      }

      emit({Event::Type::RestartEnd, rep, cfg_.t_end, 0, cur_e, 0, best_e, 0,
            false},
           stop);
    }

    emit({Event::Type::Finished, cfg_.restarts, cfg_.t_end, 0, 0, 0, best_e, 0,
          false},
         stop);
    return {best_state, best_e};
  }

 private:
  Config cfg_;
  mt19937_64 rng_;

  Energy energy_;
  Neighbor neighbor_;
  Init init_;
  Repair repair_;      // 可能为空
  Callback callback_;  // 可能为空

  void apply_repair(State& s) {
    if (repair_) repair_(s);
  }

  bool decide_accept(double delta, double t) {
    if (delta <= 0) return true;
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    return exp(-delta / t) > dist(rng_);
  }

  void emit(const Event& ev, bool& stop) {
    if (!callback_) return;
    if (callback_(ev)) stop = true;
  }
};

class Solution {
 public:
  double getMinDistSum(vector<vector<int>>& positions) {
    // 等价于https://www.acwing.com/problem/content/3170/
    // 模拟退火
    using State = std::pair<double, double>;
    int n = (int)positions.size();
    std::vector<State> pts;
    pts.reserve(n);
    for (auto& v : positions) pts.push_back({(double)v[0], (double)v[1]});
    auto energy = [&](const State& p) -> double {
      double x = p.first, y = p.second;
      double res = 0.0;
      for (auto& q : pts) {
        double dx = x - q.first, dy = y - q.second;
        res += sqrt(dx * dx + dy * dy);
      }
      return res;
    };

    // 初值setup为均值附近扰动
    State mean{0.0, 0.0};
    for (auto& p : pts) {
      mean.first += p.first;
      mean.second += p.second;
    }
    mean.first /= n;
    mean.second /= n;

    auto init = [&](std::mt19937_64& rng) -> State {
      std::uniform_real_distribution<double> d(-100.0, 100.0);
      return {mean.first + d(rng), mean.second + d(rng)};
    };

    auto neighbor = [&](const State& cur, double t,
                        std::mt19937_64& rng) -> State {
      std::normal_distribution<double> nd(0.0, t);
      return {cur.first + nd(rng), cur.second + nd(rng)};
    };

    SA<State>::Config cfg;
    double minx = pts[0].first, maxx = pts[0].first, miny = pts[0].second,
           maxy = pts[0].second;
    for (auto& p : pts) {
      minx = std::min(minx, p.first);
      maxx = std::max(maxx, p.first);
      miny = std::min(miny, p.second);
      maxy = std::max(maxy, p.second);
    }
    double diag = std::hypot(maxx - minx, maxy - miny);
    double pad = std::max(50.0, diag);
    double Lx = minx - pad, Rx = maxx + pad;
    double Ly = miny - pad, Ry = maxy + pad;
    cfg.t0 = std::max(50.0, 1.5 * diag);
    cfg.t_end = 1e-5;
    cfg.alpha = 0.965;
    cfg.steps_per_temp = 25;
    cfg.restarts = 30;

    SA<State> sa(cfg);
    sa.set_energy(energy);
    sa.set_neighbor(neighbor);
    sa.set_init(init);
    sa.set_repair([&](State& p) {
      p.first = max(Lx, min(Rx, p.first));
      p.second = max(Ly, min(Ry, p.second));
    });

    auto [best_state, best_e] = sa.run();
    return best_e;
  }
};