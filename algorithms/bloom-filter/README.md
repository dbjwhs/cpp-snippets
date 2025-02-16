# Bloom Filter Implementation

## Overview

This is a high-performance, memory-efficient Bloom filter implementation in C++, designed for robust probabilistic set
membership testing with minimal computational overhead.

## Background and Motivation

Bloom filters emerged as a revolutionary data structure in Burton H. Bloom's seminal 1970 paper, solving the critical
challenge of efficient set membership testing with extreme memory constraints. The genesis of this implementation draws
inspiration from real-world enterprise challenges, including a notable use case from VMware's data protection ecosystem.

### Enterprise Use Case: VMware VADP Team

When working on the VMware vSphere APIs for Data Protection (VADP) team, Bloom filters proved invaluable as a first-pass
filter before calculating expensive SHA hashes. This approach dramatically reduced computational overhead by quickly
eliminating non-unique data chunks before engaging in more resource-intensive cryptographic comparisons.

## Key Features

- 🚀 **Ultra-Efficient Memory Usage**
    - Constant-time O(k) insertion and lookup
    - Configurable memory-accuracy trade-off
    - Minimal memory footprint

- 🔍 **Probabilistic Set Membership**
    - Guaranteed zero false negatives
    - Controllable false positive rate
    - Ideal for large-scale data processing

- 🧮 **Flexible Configuration**
    - Dynamic sizing based on expected elements
    - Customizable false positive probability
    - Automatic optimization of hash function count

## Performance Characteristics

- **Space Complexity**: O(m), where m is bit array size
- **Time Complexity**: O(k) for insertion and lookup
- **Memory Overhead**: Significantly lower than traditional set data structures

## Use Cases

1. **Caching Systems**
    - Quick pre-check before expensive database lookups
    - Reduce unnecessary backend queries

2. **Network Security**
    - Rapid IP/domain blacklist/whitelist checks
    - Low-latency threat filtering

3. **Data Deduplication**
    - First-pass filtering before detailed comparisons
    - Eliminate redundant data chunk processing

4. **Spell Checking**
    - Preliminary dictionary word existence tests
    - Reduce computational complexity of full dictionary searches


## Example Usage

```cpp
// Create a Bloom filter for 1000 expected elements with 1% false positive rate
BloomFilter bloom(1000, 0.01);

// Add elements
bloom.add("apple");
bloom.add("banana");

// Check membership
bool exists = bloom.contains("apple");  // Returns true
bool notExists = bloom.contains("grape");  // Likely false
```

## Theoretical Foundations

The implementation is grounded in probabilistic set theory, utilizing the fundamental formula:

m = -((n * ln(p)) / (ln(2)²))

Where:
- m: Number of bits in the Bloom filter
- n: Number of expected elements
- p: Desired false positive probability

## References

- [Original Bloom Filter Paper (1970)](https://dl.acm.org/doi/10.1145/362686.362692)
- [Wikipedia: Bloom Filters](https://en.wikipedia.org/wiki/Bloom_filter)
- "Probabilistic Data Structures and Algorithms for Big Data" by Ian Wrigley

## License

This code is provided under the MIT License. Feel free to use, modify, and distribute as needed.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.
