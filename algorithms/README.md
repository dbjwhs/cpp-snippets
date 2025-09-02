# C++ Algorithm Implementations

This repository contains production-ready implementations of fundamental computer science algorithms, each carefully crafted with detailed documentation, comprehensive testing, and real-world application insights.

## 📁 Repository Structure

### [Binary Search](./binary-search/)
**Divide and conquer search algorithm with O(log n) complexity**

**✅ Good Features:**
- Logarithmic time complexity for sorted data
- Memory efficient with O(1) space complexity
- Excellent cache locality
- Safe overflow-protected midpoint calculation

**❌ Limitations:**
- Requires pre-sorted data
- Poor performance on unsorted collections
- Not cache-friendly for very large datasets that don't fit in memory

**🏭 Industry Applications:**
- Database indexing and query optimization
- IP routing table lookups in networking
- Version control systems (git bisect)
- Rate limiting in distributed systems
- Dictionary implementations and spell checkers

---

### [Bloom Filter](./bloom-filter/)
**Memory-efficient probabilistic set membership data structure**

**✅ Good Features:**
- Extreme memory efficiency compared to traditional sets
- Constant-time O(k) insertions and lookups
- Zero false negatives guaranteed
- Configurable false positive rate

**❌ Limitations:**
- Cannot handle deletions (without complex extensions)
- Probabilistic nature means some false positives
- Cannot retrieve actual stored elements
- Performance degrades as filter becomes saturated

**🏭 Industry Applications:**
- Caching systems for pre-filtering expensive database queries
- Network security for rapid IP/domain blacklist checks
- Data deduplication in storage systems (e.g., VMware VADP)
- Spell checkers for preliminary dictionary lookups
- Content delivery networks for cache optimization

---

### [Compression Algorithms](./compression/)
**Three classical lossless compression implementations: RLE, LZW, and Huffman**

**✅ Good Features:**
- **RLE**: Excellent for data with long repeated sequences
- **LZW**: Adaptive dictionary compression, good for text with patterns
- **Huffman**: Optimal prefix codes for varying character frequencies
- All algorithms provide perfect reconstruction of original data

**❌ Limitations:**
- **RLE**: Poor compression on data without repetition
- **LZW**: Dictionary overhead can exceed savings for small files
- **Huffman**: Requires two passes and tree storage overhead
- Not suitable for already compressed or encrypted data

**🏭 Industry Applications:**
- **RLE**: Fax transmissions, simple graphics, screen captures
- **LZW**: GIF images, PDF compression, Unix compress utility
- **Huffman**: JPEG compression, ZIP files, network protocols
- Data archival and backup systems
- Streaming media optimization

---

### [Hash Table](./hash-table/)
**Associative array implementation with multiple hashing strategies**

**✅ Good Features:**
- Average O(1) time complexity for all operations
- Multiple hash function support (std::hash, FNV-1a, Murmur3)
- Dynamic resizing with load factor management
- Strategy pattern for runtime hash function selection
- Comprehensive collision handling via separate chaining

**❌ Limitations:**
- Worst-case O(n) performance with many collisions
- No guaranteed iteration order
- Memory overhead for pointers and empty buckets
- Hash function quality critical for performance

**🏭 Industry Applications:**
- Database indexing and caching systems
- Symbol tables in programming language compilers
- De-duplication systems for large datasets
- In-memory key-value stores (Redis-style systems)
- Spell checkers and dictionary implementations

---

### [HyperLogLog](./hyperloglog/)
**Probabilistic cardinality estimation for massive datasets**

**✅ Good Features:**
- Fixed memory usage regardless of dataset size
- Fast O(1) insertions and cardinality estimates
- Typically within 2% accuracy of actual unique counts
- Excellent for streaming data analysis

**❌ Limitations:**
- Provides approximations, not exact counts
- Cannot retrieve individual elements
- Accuracy depends on register count configuration
- Not suitable for applications requiring exact counts

**🏭 Industry Applications:**
- Website unique visitor tracking at scale
- Database query optimization and statistics
- Streaming analytics in big data platforms
- Network traffic analysis and monitoring
- Social media engagement metrics

---

### [Longest Common Subsequence](./longest_common_subsequence-aka-diff/)
**Dynamic programming solution for sequence comparison**

**✅ Good Features:**
- Optimal solution using dynamic programming
- Foundation for diff algorithms and version control
- Handles sequences of any comparable type
- Provides both LCS length and actual subsequence

**❌ Limitations:**
- O(n×m) time and space complexity
- Memory intensive for very long sequences
- Not suitable for real-time applications with large inputs
- Poor cache performance for large 2D arrays

**🏭 Industry Applications:**
- Version control systems (Git diff algorithms)
- DNA sequence alignment in bioinformatics
- File comparison utilities and merge tools
- Plagiarism detection systems
- Data synchronization protocols

---

### [Merkle Tree](./merkle-tree/)
**Cryptographic hash tree for efficient data verification**

**✅ Good Features:**
- Efficient verification of large datasets
- Tamper detection and data integrity guarantees
- Supports partial verification without full dataset
- Hierarchical structure enables parallel processing

**❌ Limitations:**
- Requires cryptographically secure hash functions
- Tree reconstruction needed for updates
- Memory overhead for tree structure storage
- Vulnerability to collision attacks with weak hash functions

**🏭 Industry Applications:**
- Blockchain and cryptocurrency transaction verification
- Peer-to-peer file sharing (BitTorrent verification)
- Distributed version control systems (Git)
- Certificate transparency logs
- Database replication and consistency checking

---

### [Quick Sort](./quick-sort/)
**Efficient in-place divide-and-conquer sorting algorithm**

**✅ Good Features:**
- Average O(n log n) time complexity
- In-place sorting with minimal memory overhead
- Excellent cache performance and locality
- Widely adopted in standard library implementations

**❌ Limitations:**
- Worst-case O(n²) performance on already sorted data
- Not stable (doesn't preserve relative order of equal elements)
- Recursive implementation can cause stack overflow
- Performance sensitive to pivot selection strategy

**🏭 Industry Applications:**
- Standard library sorting implementations (C++ std::sort)
- Database query processing and indexing
- Graphics rendering and 3D object sorting
- Scientific computing and numerical analysis
- Real-time systems requiring fast in-place sorting

---

## 🛠️ Building and Testing

Each algorithm directory contains:
- Comprehensive source code with detailed comments
- CMake build configuration for easy compilation
- Extensive test suites covering edge cases
- Performance benchmarks and analysis
- Real-world usage examples

To build any algorithm:
```bash
cd <algorithm-directory>
mkdir build && cd build
cmake ..
make
./tests
```

## 📚 Educational Value

This repository serves as both a learning resource and production reference, featuring:
- Historical context and algorithm evolution
- Complexity analysis and performance characteristics
- Common pitfalls and implementation gotchas
- Industry applications and real-world use cases
- Security considerations where applicable

## 🤝 Contributing

Contributions are welcome! Please ensure:
- Comprehensive documentation following existing patterns
- Complete test coverage for new features
- Performance analysis for algorithmic changes
- Security review for cryptographic components

## 📄 License

All implementations are provided under the MIT License, ensuring free use, modification, and distribution for both academic and commercial purposes.