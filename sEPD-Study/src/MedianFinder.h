// MedianFinder.h

#ifndef MEDIAN_FINDER_H
#define MEDIAN_FINDER_H

#include <functional>
#include <queue>
#include <vector>

/**
 * @class MedianFinder
 * @brief Implements an online median finding algorithm using two balanced heaps.
 * * This approach allows for O(log n) insertion time and O(1) median retrieval time.
 */
class MedianFinder
{
 public:
  /**
   * @brief Constructor.
   */
  MedianFinder() = default;

  /**
   * @brief Adds a new double to the data stream in O(log n) time.
   * @param num The number to add.
   */
  void addNum(double num);

  /**
   * @brief Returns the current median of all numbers seen so far in O(1) time.
   * @return The median value (double).
   */
  double findMedian();

 private:
  // Max Heap (Lower Half)
  // Stores the smaller half of the numbers.
  // The top() is the largest element of the small half.
  std::priority_queue<double, std::vector<double>, std::less<double>> max_heap_low;

  // Min Heap (Upper Half)
  // Stores the larger half of the numbers.
  // The top() is the smallest element of the large half.
  std::priority_queue<double, std::vector<double>, std::greater<double>> min_heap_high;

  /**
   * @brief Balances the two heaps so their sizes differ by at most one.
   */
  void balanceHeaps();
};

#endif  // MEDIAN_FINDER_H
