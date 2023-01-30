#include <cassert>
#include <cmath>
#include <vector>

std::pair<std::vector<int>, std::vector<int>> get_prime_factor_with_kinds(
    int n) {
  std::vector<int> prime_factors;
  std::vector<int> cnt;  // number of i_th factor
  for (int i = 2; i <= sqrt(n); i++) {
    if (n % i == 0) {
      prime_factors.push_back(i);
      cnt.push_back(0);
      while (n % i == 0) n /= i, cnt[(int)prime_factors.size() - 1]++;
    }
  }
  if (n > 1) prime_factors.push_back(n), cnt.push_back(1);
  assert(prime_factors.size() == cnt.size());
  return {prime_factors, cnt};
}

std::vector<int> get_prime_factor_without_kinds(int n) {
  std::vector<int> prime_factors;
  for (int i = 2; i <= sqrt(n); i++) {
    if (n % i == 0) {
      while (n % i == 0) n /= i, prime_factors.push_back(i);
    }
  }
  if (n > 1) prime_factors.push_back(n);
  return prime_factors;
}

template <typename T>
std::pair<std::vector<T>, std::vector<T>> get_prime_factor_with_kinds(T n) {
  std::vector<T> prime_factors;
  std::vector<T> cnt;  // number of i_th factor
  for (T i = 2; i * i <= n; i++) {
    if (n % i == 0) {
      prime_factors.push_back(i);
      cnt.push_back(0);
      while (n % i == 0) n /= i, cnt[(int)prime_factors.size() - 1]++;
    }
  }
  if (n > 1) prime_factors.push_back(n), cnt.push_back(1);
  assert(prime_factors.size() == cnt.size());
  return {prime_factors, cnt};
}

std::pair<std::vector<long long>, std::vector<int>> get_prime_factor_with_kinds(
    long long n) {
  std::vector<long long> prime_factors;
  std::vector<int> cnt;  // number of i_th factor
  for (long long i = 2; i <= sqrt(n); i++) {
    if (n % i == 0) {
      prime_factors.push_back(i);
      cnt.push_back(0);
      while (n % i == 0) n /= i, cnt[(int)prime_factors.size() - 1]++;
    }
  }
  if (n > 1) prime_factors.push_back(n), cnt.push_back(1);
  assert(prime_factors.size() == cnt.size());
  return {prime_factors, cnt};
}