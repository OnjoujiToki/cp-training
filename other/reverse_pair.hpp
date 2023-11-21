int merge(std::vector<int>& nums, int left, int mid, int right) {
  std::vector<int> temp(right - left + 1);
  int count = 0;
  int p1 = left;
  int p2 = mid + 1;
  int i = 0;

  while (p1 <= mid && p2 <= right) {
    if (nums[p1] <= nums[p2]) {
      temp[i++] = nums[p1++];
    } else {
      temp[i++] = nums[p2++];
      // 加上左边的数，就逆序对数
      count += mid - p1 + 1;
    }
  }

  while (p1 <= mid) {
    temp[i++] = nums[p1++];
  }
  while (p2 <= right) {
    temp[i++] = nums[p2++];
  }

  for (int j = 0; j < (int)temp.size(); j++) {
    nums[left + j] = temp[j];
  }
  return count;
}
int mergeSort(std::vector<int>& nums, int left, int right) {
  if (left == right) return 0;
  int mid = (left + right) / 2;
  int leftCount = mergeSort(nums, left, mid);
  int rightCount = mergeSort(nums, mid + 1, right);
  return leftCount + rightCount + merge(nums, left, mid, right);
}

class Solution {
 public:
  std::vector<int> Bit;
  int lowbit(int x) { return x & -x; }
  int getSum(int x) {
    int ans = 0;
    for (int i = x; i >= 1; i -= lowbit(i)) ans += Bit[i];
    return ans;
  }
  void add(int k, int v) {
    for (int i = k; i < Bit.size(); i += lowbit(i)) Bit[i] += v;
  }

  int reversePairs(std::vector<int>& nums) {
    int res = 0;
    std::set<long long> s;
    for (auto& x : nums) {
      s.insert(x);
      s.insert((long long)x * 2);
    }
    std::unordered_map<long long, int> m;
    int maxElement = std::numeric_limits<int>::min();
    int idx = 0;
    for (auto& x : s) {
      m[x] = ++idx;
    }

    Bit.resize(s.size() * 2);

    int maxIndex = m[maxElement];

    for (int i = 0; i < nums.size(); i++) {
      int left = m[(long long)nums[i] * 2];
      res += getSum(m.size()) - getSum(left);
      add(m[nums[i]], 1);
    }
    return res;
  }
};