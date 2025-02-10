# Compression Algorithms Implementation

## Overview

This project implements three classical lossless compression algorithms: Run-Length Encoding (RLE), Lempel-Ziv-Welch (LZW), and Huffman Coding. RLE, developed in the 1960s, is one of the simplest forms of data compression, working by replacing sequences of identical data elements with a count and the element itself. LZW, published by Abraham Lempel, Jacob Ziv, and Terry Welch in 1984, builds on earlier LZ77 and LZ78 algorithms and creates a dictionary of repeated patterns dynamically during compression. Huffman coding, invented by David Huffman in 1952 while he was a graduate student at MIT, creates optimal prefix codes by building a binary tree based on character frequencies, assigning shorter codes to more frequent characters.

## Performance Characteristics

### Run-Length Encoding (RLE)
- Optimal for:
    - Data with long sequences of repeated characters (e.g., simple bitmap images, screen captures)
    - Binary data with long runs of 1s or 0s
    - Simple graphics with large areas of the same color
- Not suitable for:
    - Text documents with few repeated characters
    - Complex images with gradual color changes
    - Data with frequent alternating patterns

### Lempel-Ziv-Welch (LZW)
- Optimal for:
    - Text files with repeated words or phrases
    - Program source code
    - Data with recurring patterns at any distance
- Not suitable for:
    - Already compressed data (e.g., JPEG images, MP3 files)
    - Random or encrypted data
    - Very small files where dictionary overhead exceeds savings

### Huffman Coding
- Optimal for:
    - Text in natural languages
    - Data with significantly varying character frequencies
    - Files where some symbols occur much more frequently than others
- Not suitable for:
    - Data with uniform character distribution
    - Very small files where tree overhead is significant
    - Binary data with equal frequencies of 0s and 1s

## Usage Examples

### Basic Usage

```cpp
CompressionAlgorithms compressor;

// RLE Compression
string compressed = compressor.compress_rle("AAAAABBBCC");
// Output: "5A3B2C"

// LZW Compression
vector<int> compressed = compressor.compress_lzw("TOBEORNOTTOBEORTOBEORNOT");
// Outputs sequence of dictionary indices

// Huffman Compression
string compressed = compressor.compress_huffman("this is an example");
// Outputs binary string of variable-length codes
```

### File Compression

```cpp
CompressionAlgorithms compressor;

// Compress a file using all three methods
auto [rle_size, rle_ratio] = compressor.compress_file_rle("input.txt");
auto [lzw_size, lzw_ratio] = compressor.compress_file_lzw("input.txt");
auto [huffman_size, huffman_ratio] = compressor.compress_file_huffman("input.txt");
```

## Real-World Applications

### Success Stories
- LZW was widely used in GIF image format and early PDF compression
- Huffman coding is used in JPEG image compression (combined with other techniques)
- RLE is used in PCX image format and fax transmissions

### Notable Implementations
- Unix compress utility (LZW)
- PKZIP (combination of LZ77 and Huffman)
- JPEG (Huffman coding as part of larger compression scheme)

## Further Reading

### Books
1. "Introduction to Data Compression" by Khalid Sayood
    - Comprehensive coverage of all three algorithms with mathematical foundations

2. "Managing Gigabytes" by Witten, Moffat, and Bell
    - Practical implementations and variations of these algorithms

3. "The Data Compression Book" by Mark Nelson
    - Detailed C++ implementations and optimizations

### Academic Papers
- Huffman, D.A. (1952). "A Method for the Construction of Minimum-Redundancy Codes"
- Welch, T.A. (1984). "A Technique for High-Performance Data Compression"
- Ziv, J., Lempel, A. (1977). "A Universal Algorithm for Sequential Data Compression"

## Implementation Notes

This implementation focuses on clarity and educational value rather than maximum performance. Key features include:
- Standard C++ implementation with no external dependencies
- Comprehensive error handling and input validation
- Detailed comments explaining algorithm steps
- Test suite with verification of compression/decompression cycle
- Memory-efficient handling of large files

## License

This project is licensed under the MIT License - see the LICENSE file for details.
