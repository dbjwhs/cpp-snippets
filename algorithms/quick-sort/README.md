# QuickSort Implementation

## First My Personal Discovery of QuickSort

In 1999, Jon Bentley's seminal work [Programming Pearls](https://wiki.c2.com/?ProgrammingPearls) hit the shelves of brick-and-mortar bookstores, back when discovering programming books meant wandering through physical aisles rather than scrolling through online reviews. The book became a touchstone for a generation of programmers, featuring among its many gems an implementation of QuickSort using what we now know as the Lomuto partition scheme. This was during an era when C dominated systems programming and before C++ had achieved its current ubiquity.

## Historical Background

[Tony Hoare](https://www.cs.ox.ac.uk/people/tony.hoare/) *also known as C. A. R. Hoare* developed QuickSort in 1959 while studying at Moscow State University, where he was working on machine translation for the National Physical Laboratory. At the time, he was implementing a translation algorithm that needed to sort word frequencies efficiently. Beyond QuickSort, Hoare made numerous groundbreaking contributions to computer science, including Hoare Logic for verifying program correctness, the Communicating Sequential Processes (CSP) formal language, and the null reference concept (which he later called his "billion-dollar mistake"). Today QuickSort and its variants are estimated to perform billions of sorting operations daily, as it's the standard sorting algorithm in many programming language libraries including C++'s std::sort, Java's Arrays.sort(), and Android's TimSort (a hybrid of QuickSort and MergeSort).

## Algorithm Selection Guide

[QuickSort](https://en.wikipedia.org/wiki/Quicksort) is particularly well-suited for situations where in-memory sorting of large datasets is required and cache efficiency is important. It excels with random-access data structures like arrays, especially when dealing with datasets too large for cache-optimized insertion sort but not large enough to warrant external sorting algorithms. Its average-case time complexity of O(n log n) with excellent cache locality often makes it faster in practice than other O(n log n) algorithms like MergeSort.

However, QuickSort may not be the best choice in several scenarios:
- When stable sorting is required (maintaining relative order of equal elements), MergeSort is preferred
- For small arrays (typically < 10–50 elements), Insertion Sort often performs better due to lower overhead
- When dealing with nearly sorted data, MergeSort or TimSort typically perform better
- For linked lists, MergeSort is usually more efficient due to QuickSort's random access requirements
- When sorting data that doesn't fit in memory, external sorting algorithms like External MergeSort are necessary
- For parallel processing, MergeSort's predictable divide-and-conquer pattern makes it easier to parallelize

Other notable sorting algorithms and their use cases:
- HeapSort: Guaranteed O(n log n) with O(1) extra space, useful for memory-constrained systems
- RadixSort: O(n) for integers or fixed-length strings, excellent for specific data types
- BucketSort: O(n) for uniformly distributed data
- TimSort: Hybrid algorithm, extremely efficient for partially sorted data
- IntroSort: Hybrid of QuickSort, HeapSort, and InsertionSort, used in many standard libraries

## Implementation Details

This implementation features:
- Classic Lomuto partition scheme using last element as pivot

### Understanding the Partition Scheme

The partition scheme is the core mechanism of QuickSort that determines how elements are divided around a pivot. Our implementation uses the Lomuto partition scheme, which works as follows:

1. Select the last element as the pivot
2. Maintain two pointers:
    - A slow pointer (m_i) tracking the boundary between elements smaller and larger than pivot
    - A fast pointer (j) scanning through the array

Example of partitioning [7, 2, 1, 6, 8, 5] with pivot = 5:
```
Initial:
[7, 2, 1, 6, 8, 5]  pivot = 5
 ^              ^
 i              pivot
 j

Step 1: j finds 2 (< pivot)
[2, 7, 1, 6, 8, 5]  i moves up, swap happened
    ^
    i
    j

Step 2: j finds 1 (< pivot)
[2, 1, 7, 6, 8, 5]  i moves up, swap happened
       ^
       i
       j

Step 3: j finds nothing else < pivot
[2, 1, 7, 6, 8, 5]
             ^
       i     j

Final: put pivot in place
[2, 1, 5, 6, 8, 7]  pivot swapped with i+1
```

Alternative partition schemes include:
- Hoare's original scheme: Uses two pointers moving from both ends, more efficient but complex
- Three-way partitioning: Better handling of duplicate elements
- Random pivot selection: Better average case performance
- Median-of-three: Examine first, middle, and last elements for pivot selection

Each scheme has tradeoffs:
- Lomuto (our implementation): Simple but can be inefficient for sorted arrays
- Hoare's: More efficient but more complex implementation
- Random pivot: Better average case but requires random number generation
- Median-of-three: Good balance but requires more initial comparisons
- In-place sorting to minimize memory usage
- Comprehensive test suite covering edge cases
- Exception-safe design following modern C++ practices

## Usage

```cpp
// Create a QuickSort instance with your array
std::vector<int> myArray = {3, 1, 4, 1, 5, 9};
QuickSort sorter(myArray);

// Sort the array
sorter.sort();

// Get the sorted result
std::vector<int> sortedArray = sorter.getSortedArray();
```

## Test Suite

The implementation includes tests for:
- Empty arrays
- Single-element arrays
- Pre-sorted arrays
- Reverse-sorted arrays
- Arrays with duplicate elements
- Large random arrays
- Arrays with negative numbers

## Complexity Analysis

- Time Complexity:
    - Best Case: O(n log n)
    - Average Case: O(n log n)
    - Worst Case: O(n²) (rare with good pivot selection)
- Space Complexity: O(log n) average case for recursion stack
- In-place sorting: No additional array allocation required


## License
This project is licensed under the MIT License - see the LICENSE file for details.

## Contributing
Contributions are welcome! Please feel free to submit pull requests with improvements or bug fixes.
