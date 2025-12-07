// MedianFinder.cc

#include "MedianFinder.h"
#include <stdexcept>

void MedianFinder::balanceHeaps()
{
  // Case 1: Max Heap (Low) has two more elements than Min Heap (High)
  // Move the largest element from Max Heap to Min Heap.
  if (max_heap_low.size() > min_heap_high.size() + 1)
  {
    min_heap_high.push(max_heap_low.top());
    max_heap_low.pop();
  }
  // Case 2: Min Heap (High) has more elements than Max Heap (Low)
  // Move the smallest element from Min Heap to Max Heap.
  else if (min_heap_high.size() > max_heap_low.size())
  {
    max_heap_low.push(min_heap_high.top());
    min_heap_high.pop();
  }
}

void MedianFinder::addNum(double num)
{
  // 1. Determine Placement: Insert into the correct heap

  // If the heap is empty, or the number belongs in the lower half (Max Heap)
  if (max_heap_low.empty() || num <= max_heap_low.top())
  {
    max_heap_low.push(num);
  }
  else
  {
    min_heap_high.push(num);
  }

  // 2. Balance the heaps after insertion
  balanceHeaps();
}

double MedianFinder::findMedian()
{
  // Check if the structure is empty
  if (max_heap_low.empty())
  {
    // You might throw an exception or return a special value here
    throw std::runtime_error("Cannot find median of an empty dataset.");
  }

  // Total number of elements
  size_t total_size = max_heap_low.size() + min_heap_high.size();

  // Odd number of elements: Median is the root of the larger heap.
  if (total_size % 2 != 0)
  {
    // We ensure max_heap_low is always the one with the extra element (or equal size)
    return max_heap_low.top();
  }

  return (max_heap_low.top() + min_heap_high.top()) / 2.0;
}
