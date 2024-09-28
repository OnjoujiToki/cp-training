#include <iostream>
#include <vector>

int lomutoPartition(std::vector<int>& arr, int low, int high) {
  int pivot = 4;
  int i = (low - 1);
  std::cout << "Lomuto partition called with low: " << low
            << " and high: " << high << std::endl;
  std::cout << "choosing pivot: " << pivot << "\n";
  std::cout << "i: " << i << std::endl;
  for (int j = low; j <= high - 1; j++) {
    std::cout << "arr[j]: " << arr[j] << " and pivot: " << pivot << std::endl;
    if (arr[j] <= pivot) {
      std::cout << "swapping arr[i]: " << arr[i] << " and arr[j]: " << arr[j]
                << std::endl;
      i++;  // Increment index of smaller element
      std::swap(arr[i], arr[j]);
    }
  }
  std::swap(arr[i + 1], arr[high]);
  return (i + 1);
}

void quickSortLomuto(std::vector<int>& arr, int low, int high) {
  if (low < high) {
    int pi = lomutoPartition(arr, low, high);

    quickSortLomuto(arr, low, pi - 1);
    quickSortLomuto(arr, pi + 1, high);
  }
}
int hoarePartition(std::vector<int>& arr, int low, int high) {
  int pivot = 4;
  int i = low - 1, j = high + 1;
  std::cout << "Hoare partition called with low: " << low
            << " and high: " << high << std::endl;
  std::cout << "choosing pivot: " << pivot << "\n";
  std::cout << "i: " << i << " j: " << j << std::endl;
  while (i < j) {
    do i++, std::cout << "i: " << i << "  and arr[i]: " << arr[i] << std::endl;
    while (arr[i] < pivot);

    do j--, std::cout << "j: " << j << "  and arr[j]: " << arr[j] << std::endl;
    while (arr[j] > pivot);

    if (i >= j) {
      std::cout << "Because i >= j, returning j: " << j << std::endl;
      std::cout << "returning j: " << j << std::endl;
      return j;
    }

    std::cout << "swapping arr[i]: " << arr[i] << " and arr[j]: " << arr[j]
              << std::endl;
    std::swap(arr[i], arr[j]);
  }
}

void quickSortHoare(std::vector<int>& arr, int low, int high) {
  if (low < high) {
    int pi = hoarePartition(arr, low, high);

    quickSortHoare(arr, low, pi);
    quickSortHoare(arr, pi + 1, high);
  }
}

int main() {
  std::vector<int> arr = {2, 8, 7, 1, 3, 5, 6, 4};
  std::vector<int> brr = {2, 8, 7, 1, 3, 5, 6, 4};
  // quickSortLomuto(arr, 0, arr.size() - 1);
  hoarePartition(arr, 0, arr.size() - 1);
  lomutoPartition(brr, 0, brr.size() - 1);
  // Or for Hoare
  // quickSortHoare(arr, 0, arr.size() - 1);

  for (int i = 0; i < arr.size(); i++) {
    std::cout << arr[i] << " ";
  }
  std::cout << std::endl;
  for (int i = 0; i < brr.size(); i++) {
    std::cout << brr[i] << " ";
  }

  return 0;
}
