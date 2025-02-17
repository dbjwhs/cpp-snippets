# Binary Search Implementation

Binary Search is a fundamental divide-and-conquer algorithm that emerged in the early days of computer science, with its
first known implementation dating back to 1946 when John Mauchly suggested the idea for searching sorted tape data. However,
the first published binary search algorithm was by D.H. Lehmer in 1944, but didn't gain widespread attention until John W. Tukey
implemented it in 1957. Interestingly, the first binary search implementation was shown to have a bug that went undetected
for 20 years until 1985, highlighting the subtle complexity behind this seemingly simple algorithm. The algorithm works by
repeatedly dividing a sorted search space in half, eliminating half of the remaining elements with each comparison, resulting
in a logarithmic time complexity of O(log n).

### The Bug Found In 1985 (*for the curious*)

The bug in binary search's first implementation is actually a fascinating piece of computer science history. The bug was
discovered by Jon Bentley, who pointed out that the original binary search implementation had an integer overflow bug that
had gone unnoticed for 20 years.

Specifically, in 1985, Bentley gave a binary search problem as an assignment in a programming course. He found that almost
all solutions, including the one in K&P (Kernighan and Plauger) implementation, had the same bug. The problematic line was
calculating the midpoint:

```cpp
mid = (low + high) / 2;
```

This seemingly correct calculation can cause integer overflow when `low + high` exceeds the maximum value for an integer
(typically 2^31 - 1 for 32-bit integers). For very large arrays, when `low` and `high` are large enough, their sum would
overflow before the division, leading to incorrect behavior or crashes.

The correct implementation should be:
```cpp
mid = low + (high - low) / 2;
```

This bug persisted in textbooks, research papers, and production code for decades. It's particularly notable because:
1. It only manifests with very large arrays (near the size of the maximum integer value)
2. It passed all standard test cases
3. It was present in widely-used standard library implementations
4. It appeared in numerous published books and papers

This discovery led to a broader discussion in computer science about the importance of careful boundary case analysis and
the limitations of testing. It's often cited as an example of why even seemingly simple algorithms need rigorous verification
and why understanding hardware limitations (like integer overflow) is crucial.

The bug was so significant that Joshua Bloch (Java architect) mentioned it in his book "Effective Java" as a lesson in
careful API design and implementation.

Now back to our normal README...

## Use Cases and Problem Solving

Binary Search excels in scenarios where we need efficient search capabilities on sorted data structures. Common applications include:

- Dictionary implementations for fast word lookups
- Database indexing and query optimization
- IP routing table lookups
- Finding insertion points in sorted collections
- Rate limiting and resource allocation in distributed systems
- Solving optimization problems through binary search on answer space
- Finding roots of continuous monotonic functions
- Version control systems for finding bug introductions (git bisect)

The pattern particularly shines in solving problems where:
- Data is sorted or can be sorted once and searched many times
- Random access to elements is O(1)
- Memory constraints prevent linear scanning
- Quick response times are critical
- Binary decisions need to be made on ranges

## Implementation Details

### Key Components

1. **Sorted Data Structure**: Binary search requires sorted data
2. **Comparison Function**: Defines the ordering relation
3. **Search Window**: Tracks the current search space
4. **Midpoint Calculation**: Safely computes the middle element

## Best Practices

### Do's
- Always verify input data is sorted
- Use safe midpoint calculation to prevent integer overflow
- Consider duplicates in the implementation
- Test edge cases (empty container, single element)
- Use templated comparators for flexibility
- Implement robust error handling
- Add logging for debugging

### Don'ts
- Don't assume input is unique
- Don't use naive midpoint calculation (can overflow)
- Don't forget to handle edge cases
- Don't assume exact index returns with duplicates
- Don't modify collection during search


## Notable Implementations

1. C++ Standard Library
```cpp
std::binary_search
std::lower_bound
std::upper_bound
std::equal_range
```

2. Java Collections Framework
```java
Collections.binarySearch()
Arrays.binarySearch()
```

3. Python Standard Library
```python
bisect.bisect_left()
bisect.bisect_right()
```

### Books
1. "Introduction to Algorithms" (CLRS) - Chapter 2.3
2. "The Art of Computer Programming" by Donald Knuth - Section 6.2.1
3. "Programming Pearls" by Jon Bentley - Column 4
4. "Algorithms" by Robert Sedgewick - Chapter 4.1

## Common Interview Questions

1. Implement binary search without using loops
2. Find first/last occurrence of element
3. Search in rotated sorted array
4. Find peak element
5. Search in 2D sorted matrix
6. Find smallest letter greater than target
7. Find missing number in arithmetic progression

## License

This code is provided under the MIT License. Feel free to use, modify, and distribute as needed.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.
