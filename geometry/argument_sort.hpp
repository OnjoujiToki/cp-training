#ifndef ARGUMENT_SORT_HPP
#define ARGUMENT_SORT_HPP

#include <algorithm>
#include <tuple>
#include <vector>

// follow yosupo judge
void arg_sort(std::vector<std::tuple<long long, long long, int>>& a) {
  // 讨论 (0, 0)
  auto get_region = [](long long x, long long y) -> int {
    if (y < 0) return 0;             // (-pi, 0) : 第三、四象限及负 Y 轴
    if (y == 0 && x >= 0) return 1;  // 0        : 正 X 轴及原点 (0,0)
    if (y > 0) return 2;             // (0, pi)  : 第一、二象限及正 Y 轴
    return 3;                        // pi       : 负 X 轴
  };

  std::ranges::sort(a, [get_region](const auto& u, const auto& v) {
    auto [ux, uy, uidx] = u;
    auto [vx, vy, vidx] = v;
    int ru = get_region(ux, uy);
    int rv = get_region(vx, vy);

    // 1. 区域不同
    if (ru != rv) return ru < rv;
    // 2. 区域相同
    long long cross = ux * vy - uy * vx;
    if (cross != 0) return cross > 0;

    // 3. 严格共线时（包括同为 (0,0)），按距离原点的长度升序
    return ux * ux + uy * uy < vx * vx + vy * vy;
  });
}

/*
long long cross(long long ax, long long ay, long long bx, long long by) {
  return ax * by - ay * bx;
}

long long dot(long long ax, long long ay, long long bx, long long by) {
  return ax * bx + ay * by;
}

// 从 a 逆时针转到 b 的角是否严格小于 pi
bool ok(long long ax, long long ay, long long bx, long long by) {
  long long c = cross(ax, ay, bx, by);
  if (c > 0) return true;
  if (c < 0) return false;
  return dot(ax, ay, bx, by) > 0;  // 同向可以，反向(=pi)不行
}
*/

/*
叉积/点积判断两点共线/象限
  auto is_same_line = [](const auto& u, const auto& v) {
    auto [ux, uy, uidx] = u;
    auto [vx, vy, vidx] = v;
    return ux * vy == uy * vx && ux * vx + uy * vy > 0;
  };




*/
#endif