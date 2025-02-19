# LRU Cache

## Overview
An LRU (Least Recently Used) cache is a data structure that stores a limited number of items and discards the least recently used item when the cache becomes full. This implementation strategy is based on the principle that recently used items are likely to be used again soon.

## History
The LRU cache algorithm was first introduced in the 1960s during the early days of computer memory management. It was developed to address the challenge of managing limited, fast-access memory resources (like CPU cache) effectively.

Key historical developments:
- 1965: First documented use in IBM's OS/360
- 1970s: Widely adopted in virtual memory systems
- 1980s: Became standard in CPU cache designs
- 1990s-present: Essential component in web browsers, database systems, and operating systems

## How It Works
LRU cache operates on these core principles:
1. Fixed Capacity: The cache has a maximum number of items it can hold
2. Fast Access: Both reads and writes should be O(1) operations
3. Tracking Usage: Each access to a cached item marks it as "most recently used"
4. Eviction Policy: When full, the cache removes the least recently used item

## Common Use Cases
- CPU caches
- Database management systems
- Web browsers (page caching)
- File system caching
- Image caching in applications
- Network packet buffers

## Implementation Details
Modern LRU cache implementations typically use a combination of:
- Hash Map: For O(1) key-value lookups
- Doubly Linked List: For O(1) insertion/deletion and maintaining usage order

## Performance
- Time Complexity:
    - Get: O(1)
    - Put: O(1)
- Space Complexity: O(n), where n is the cache capacity

## Advantages and Limitations
Advantages:
- Constant time operations
- Simple to implement
- Effective for many workloads

Limitations:
- No awareness of access frequency
- Memory overhead due to tracking structure
- May not be optimal for all access patterns

## Related Algorithms
- LFU (Least Frequently Used)
- FIFO (First In First Out)
- MRU (Most Recently Used)
- ARC (Adaptive Replacement Cache)

## License

This code is provided under the MIT License. Feel free to use, modify, and distribute as needed.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.
