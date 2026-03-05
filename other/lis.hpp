template <class T>
int LIS(std::vector<T>& a, bool is_strict = true) {
  const T INF = 1 << 30;
  int n = (int)a.size();
  std::vector<T> dp(n, INF);
  for (int i = 0; i < n; ++i) {
    if (is_strict)
      *lower_bound(dp.begin(), dp.end(), a[i]) = a[i];
    else
      *upper_bound(dp.begin(), dp.end(), a[i]) = a[i];
  }
  return lower_bound(dp.begin(), dp.end(), INF) - dp.begin();
}