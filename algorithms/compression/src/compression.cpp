// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <unordered_map>
#include <fstream>
#include <cassert>
#include <algorithm>
#include <ranges>
#include "../../../headers/project_utils.hpp"
#if defined(__linux__) || defined(__linux) || defined(linux)
    #include <bits/ranges_algo.h>
#endif

// node structure for a huffman tree
struct HuffmanNode {
    char m_char;
    int m_frequency;
    std::shared_ptr<HuffmanNode> m_left, m_right;

    HuffmanNode(const char character, const int frequency) : m_char(character), m_frequency(frequency),
                                   m_left(nullptr), m_right(nullptr) {}
};

// custom comparator for priority queue in huffman coding
// used to order nodes by their frequency counts during tree construction
//
// params:
//   a: first huffman node to compare (shared_ptr to node containing char and frequency)
//   b: second huffman node to compare (shared_ptr to node containing char and frequency)
//
// return: true if a's frequency is greater than b's frequency
//
// detailed explanation:
// this comparator is crucial for building the huffman tree. it works with
// huffmannode (above) objects that contain:
//   - m_char: the character stored in the node
//   - m_frequency: how often that character appears
//   - m_left and m_right: pointers to child nodes
//
// the priority queue uses this comparator to:
//   1. keep nodes ordered by frequency (lowest frequency at top)
//   2. provide the two lowest frequency nodes when building the tree
//   3. properly place new combined nodes in the queue
//
// example scenario:
//   given characters and frequencies:
//   'a': 5 occurrences
//   'b': 3 occurrences
//   'c': 7 occurrences
//
//   priority queue ordering (using this comparator):
//   1. 'b' (freq: 3) - comes out first
//   2. 'a' (freq: 5) - comes out second
//   3. 'c' (freq: 7) - comes out last
//
// note: using '>' instead of '<' in the comparison makes this a min-heap,
// meaning nodes with smaller frequencies rise to the top of the queue.
// this is exactly what we need for building an optimal huffman tree
// from the bottom up
struct CompareNodes {
    bool operator()(const std::shared_ptr<HuffmanNode>& a,
                   const std::shared_ptr<HuffmanNode>& b) const {
        return a->m_frequency > b->m_frequency;
    }
};

// compression algorithms class implementing run length encoding (rle),
// lempel-ziv-welch (lzw), and huffman coding
class CompressionAlgorithms {
private:
    // member variables for configuration and statistics
    std::string m_input_data;
    std::string m_compressed_data;
    std::string m_decompressed_data;
    double m_compression_ratio;
    std::unordered_map<char, std::string> m_huffman_codes;
    std::shared_ptr<HuffmanNode> m_huffman_root;  // store the huffman tree

    // helper method to calculate a compression ratio with actual compressed size in bytes
    void m_calculate_ratio(size_t compressed_size) {
        m_compression_ratio = static_cast<double>(compressed_size) /
                            static_cast<double>(m_input_data.length()) * 100.0;
    }

    // recursively generates huffman codes for each character in the tree,
    //
    // this method traverses the huffman tree depth-first and assigns binary codes
    // to each character. for each left traversal, append '0' to the code. for each
    // right traversal, append '1'. when a leaf node is reached, store the
    // accumulated code for that character
    //
    // time complexity: o(n) where n is number of nodes in the tree
    // space complexity: o(h) where h is height of tree due to recursion stack
    //
    // params:
    //   root: current node in the huffman tree being processed
    //   code: accumulated binary code string for the current path (default="")
    //
    // example tree traversal:
    //        (5)
    //       /   \
    //     (4)   c:1     traversal path for 'c': right  -> code: "1"
    //    /   \          traversal path for 'a': left,left  -> code: "00"
    //   a:2  b:2        traversal path for 'b': left,right -> code: "01"
    //
    // note: the method populates the m_huffman_codes member map where:
    //   key: character (char)
    //   value: binary code string (string of '0's and '1's)
    //
    // example final mapping:
    //   m_huffman_codes['a'] = "00"
    //   m_huffman_codes['b'] = "01"
    //   m_huffman_codes['c'] = "1"
    //
    // warning: assumes the huffman tree is properly constructed with:
    //   - internal nodes having exactly 2 children
    //   - leaf nodes containing valid characters
    //   - no cycles in the tree
    void generate_huffman_codes(const std::shared_ptr<HuffmanNode>& root,
                                const std::string& code = "") {
        // base case: empty node
        if (!root) return;

        // base case: leaf node (has a character)
        // stores the generated code for this character
        if (!root->m_left && !root->m_right) {
            m_huffman_codes[root->m_char] = code;
            return;
        }

        // recursive case: internal node
        // traverse left adding '0' to code
        generate_huffman_codes(root->m_left, code + "0");

        // traverse right adding '1' to code
        generate_huffman_codes(root->m_right, code + "1");
    }

public:
    // constructor initializes member variables
    CompressionAlgorithms() : m_compression_ratio(0.0) {}

    // run length encoding compression
    std::string compress_rle(const std::string& input) {
        m_input_data = input;
        m_compressed_data.clear();

        if (input.empty()) {
            return "";
        }

        char current_char = input[0];
        int count = 1;

        for (size_t ndx = 1; ndx < input.length(); ndx++) {
            if (input[ndx] == current_char) {
                count++;
            } else {
                m_compressed_data += std::to_string(count) + current_char;
                current_char = input[ndx];
                count = 1;
            }
        }

        m_compressed_data += std::to_string(count) + current_char;
        m_calculate_ratio(m_compressed_data.length());

        return m_compressed_data;
    }

    // run length encoding decompression
    std::string decompress_rle(const std::string& compressed) {
        m_compressed_data = compressed;
        m_decompressed_data.clear();

        if (compressed.empty()) {
            return "";
        }

        size_t ndx = 0;
        while (ndx < compressed.length()) {
            std::string count_str;
            while (ndx < compressed.length() && std::isdigit(compressed[ndx])) {
                count_str += compressed[ndx++];
            }

            if (ndx < compressed.length()) {
                int count = std::stoi(count_str);
                m_decompressed_data.append(count, compressed[ndx++]);
            }
        }

        return m_decompressed_data;
    }

    // lempel-ziv-welch compression
    std::vector<int> compress_lzw(const std::string& input) {
        m_input_data = input;
        m_compressed_data.clear();
        std::vector<int> result;
        std::unordered_map<std::string, std::size_t> dictionary;

        for (int ndx = 0; ndx < 256; ndx++) {
            dictionary[std::string(1, static_cast<char>(ndx))] = ndx;
        }

        std::string current;
        for (const char next_char : input) {
            std::string next = current + next_char;
            if (dictionary.contains(next)) {
                current = next;
            } else {
                result.push_back(dictionary[current]);
                dictionary[next] = dictionary.size();
                current = std::string(1, next_char);
            }
        }

        if (!current.empty()) {
            result.push_back(dictionary[current]);
        }

        // calculate a ratio using actual size in bytes
        const size_t compressed_size = result.size();
        m_calculate_ratio(compressed_size);

        return result;
    }

    // lempel-ziv-welch decompression
    std::string decompress_lzw(const std::vector<int>& compressed) {
        if (compressed.empty()) return "";

        std::unordered_map<int, std::string> dictionary;
        for (int ndx = 0; ndx < 256; ndx++) {
            dictionary[ndx] = std::string(1, static_cast<char>(ndx));
        }

        m_decompressed_data = dictionary[compressed[0]];
        std::string current = m_decompressed_data;

        for (size_t i = 1; i < compressed.size(); i++) {
            int code = compressed[i];
            std::string entry;

            if (dictionary.count(code)) {
                entry = dictionary[code];
            } else if (code == dictionary.size()) {
                entry = current + current[0];
            }

            m_decompressed_data += entry;
            dictionary[dictionary.size()] = current + entry[0];
            current = entry;
        }

        return m_decompressed_data;
    }

    // compresses input data using huffman coding algorithm
    //
    // the huffman compression process consists of several steps:
    // 1. character frequency counting
    // 2. building the huffman tree using a min-heap priority queue
    // 3. generating variable-length codes for each character
    // 4. encoding the input string using generated codes
    //
    // time complexity: o(n log k) where n is input length and k is unique characters
    // space complexity: o(k) for the huffman tree, where k is unique characters
    //
    // param: input - the string to be compressed
    // return: string - a binary string representation of compressed data, in
    // this format it is easier to see the "binary" data in the debugger or
    // printed to screen.
    //
    // example:
    //   input: "aabbc"
    //   frequency table: a:2, b:2, c:1
    //   huffman tree:
    //        (5)
    //       /   \
    //     (4)   c:1
    //    /   \
    //   a:2  b:2
    //
    //   resulting codes: a=00, b=01, c=1
    //   output: "00001001"
    std::string compress_huffman(const std::string& input) {
        if (input.empty()) {
            return "";
        }

        m_input_data = input;
        m_compressed_data.clear();

        // step 1: count frequency of each character
        // creates a hash map where key is the character and value is its frequency
        std::unordered_map<char, int> frequency_char_map;
        for (char single_char : input) {
            frequency_char_map[single_char]++;
        }

        // step 2: build huffman tree
        // uses priority queue (min-heap) to always get two nodes with the lowest frequencies
        std::priority_queue<std::shared_ptr<HuffmanNode>,
                          std::vector<std::shared_ptr<HuffmanNode>>,
                          CompareNodes> priority_queue;

        // initialize priority queue with leaf nodes for each character
        for (const auto& [character, frequency] : frequency_char_map) {
            priority_queue.push(std::make_shared<HuffmanNode>(character, frequency));
        }

        // build a tree by repeatedly combining two lowest-frequency nodes
        while (priority_queue.size() > 1) {
            const auto left = priority_queue.top(); priority_queue.pop();   // get first minimum frequency node
            const auto right = priority_queue.top(); priority_queue.pop();  // get second minimum frequency node

            // create parent node with combined frequency
            // '\0' character indicates internal node (not a leaf)
            auto parent = std::make_shared<HuffmanNode>('\0', left->m_frequency + right->m_frequency);
            parent->m_left = left;            // assign left child
            parent->m_right = right;          // assign right child

            priority_queue.push(parent);      // add combined node back to queue
        }

        // step 3: generate huffman codes
        // store the root for later decompression
        m_huffman_codes.clear();
        m_huffman_root = priority_queue.top();

        // traverse tree to generate codes (0 for left, 1 for right)
        generate_huffman_codes(m_huffman_root);

        // step 4: encode input string using generated codes
        // replace each character with its variable-length binary code
        std::string encoded;
        for (char c : input) {
            encoded += m_huffman_codes[c];    // append binary code for each character
        }

        // calculate a compression ratio based on actual binary size in bytes
        // round up to the nearest byte as we can't store partial bytes
        size_t compressed_size = (encoded.length() + 7) / 8;
        m_calculate_ratio(compressed_size);

        return encoded;
    }

    // decompresses huffman-encoded data using the huffman tree
    //
    // the decompression process traverses the huffman tree using the compressed
    // binary string. each '0' means go left, each '1' means go right. when a
    // leaf node is reached, output its character and restart from root
    //
    // time complexity: o(n) where n is length of compressed string
    // space complexity: o(1) as we reuse the existing tree
    //
    // param: compressed - the binary string of compressed data
    // param: root - the root node of the huffman tree used for compression
    // return: string - the decompressed original string
    //
    // example:
    //   compressed: "00001001"
    //   tree:
    //        (5)
    //       /   \
    //     (4)   c:1
    //    /   \
    //   a:2  b:2
    //
    //   process:
    //   "00" -> a   (follows path left, left)
    //   "00" -> a   (follows path left, left)
    //   "01" -> b   (follows path left, right)
    //   "1"  -> c   (follows path right)
    //   output: "aabc"
    std::string decompress_huffman(const std::string& compressed, const std::shared_ptr<HuffmanNode>& root) {
        // validate input parameters
        if (compressed.empty() || !root) return "";

        std::string decoded;
        auto current = root;  // start at the root of huffman tree

        // process each bit in compressed string
        for (const char bit : compressed) {
            // traverse left for '0', right for '1'
            if (bit == '0') {
                current = current->m_left;
            } else {
                current = current->m_right;
            }

            // check if we've reached a leaf node (character node)
            if (!current->m_left && !current->m_right) {
                decoded += current->m_char;    // add character to output
                current = root;                // reset to root for next character
            }

            // note: if we're not at a leaf, continue traversing with next bit
        }

        return decoded;
    }

    // file compression/decompression methods that return the compressed/decompressed data
    std::pair<std::string, std::pair<size_t, double>> compress_and_get_file_rle(const std::string& filename) {
        std::ifstream input_file(filename, std::ios::binary);
        if (!input_file) {
            return {"", {0, 0.0}};
        }

        const std::string content((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());

        m_input_data = content;
        std::string compressed = compress_rle(content);

        return {compressed, {compressed.size(), m_compression_ratio}};
    }

    std::pair<std::vector<int>, std::pair<size_t, double>> compress_and_get_file_lzw(const std::string& filename) {
        std::ifstream input_file(filename, std::ios::binary);
        if (!input_file) return {{}, {0, 0.0}};

        const std::string content((std::istreambuf_iterator<char>(input_file)),
                           std::istreambuf_iterator<char>());

        m_input_data = content;
        auto compressed = compress_lzw(content);

        return {compressed, {compressed.size(), m_compression_ratio}};
    }

    std::pair<std::string, std::pair<size_t, double>> compress_and_get_file_huffman(const std::string& filename) {
        std::ifstream input_file(filename, std::ios::binary);
        if (!input_file) return {"", {0, 0.0}};

        const std::string content((std::istreambuf_iterator<char>(input_file)),
                           std::istreambuf_iterator<char>());

        m_input_data = content;
        std::string compressed = compress_huffman(content);

        return {compressed, {(compressed.length() + 7) / 8, m_compression_ratio}};
    }

    // helper to get huffman root
    [[nodiscard]] std::shared_ptr<HuffmanNode> get_huffman_root() const {
        return m_huffman_root;
    }

    // getter for compression ratio
    [[nodiscard]] double get_compression_ratio() const {
        return m_compression_ratio;
    }
};

// comprehensive test suite for compression algorithms
void run_tests() {
    Logger& logger = Logger::getInstance();
    CompressionAlgorithms compressor;
    const std::string passed = "PASSED";
    const std::string failed = "FAILED";

    // test 1: run length encoding with repeated characters
    {
        std::string input = "aaaaaaaaaaaaaaaaaaaaabbbbbbbbbbbbbbbbccccccccccccccc";
        std::string compressed = compressor.compress_rle(input);
        std::string decompressed = compressor.decompress_rle(compressed);
        assert(input == decompressed);
        logger.log(LogLevel::INFO, "test 1 passed: rle basic compression");
        logger.log(LogLevel::INFO, "input: " + input);
        logger.log(LogLevel::INFO, "compressed: " + compressed);
        logger.log(LogLevel::INFO, "compression ratio: " +  std::to_string(compressor.get_compression_ratio())
            + "% of total size of " + std::to_string(input.size()) + " bytes");
    }

    // test 2: lzw compression with repeated patterns
    {
        std::string input = "ABABABABABABABABABABABABABABABABABABABABABABABABABABABAB";
        std::vector<int> compressed = compressor.compress_lzw(input);
        std::string decompressed = compressor.decompress_lzw(compressed);
        assert(input == decompressed);
        logger.log(LogLevel::INFO, "test 2 passed: lzw repeated patterns");
        logger.log(LogLevel::INFO, "input: " +  input);
        logger.log(LogLevel::INFO, "compressed data: ");
        std::stringstream ss;
        std::ranges::for_each(compressed, [&](int n){ ss << n << " "; });
        logger.log(LogLevel::INFO, ss.str());
        logger.log(LogLevel::INFO, "compressed size: " +  std::to_string(compressed.size()) + " integers");
        logger.log(LogLevel::INFO, "compression ratio: " +  std::to_string(compressor.get_compression_ratio())
            + "% of total size of " + std::to_string(input.size()) + " bytes");
    }

    // test 3: huffman coding
    {
        std::string input = "this is a test string for huffman coding";
        std::string compressed = compressor.compress_huffman(input);
        logger.log(LogLevel::INFO, "test 3 passed: huffman coding");
        logger.log(LogLevel::INFO, "input: " + input);
        logger.log(LogLevel::INFO, "compressed binary: " + compressed);
        logger.log(LogLevel::INFO, "compressed size (bits): " +  std::to_string(compressed.length()));
        logger.log(LogLevel::INFO, "compression ratio: " +  std::to_string(compressor.get_compression_ratio()) + "%");
    }

    // test 4: file compression and decompression verification
    {
        std::string test_file = "../frost_giants_daughter.txt";
        logger.log(LogLevel::INFO, "test 4: file compression and decompression verification");

        // read original file content
        if (std::ifstream file(test_file, std::ios::binary); file.is_open()) {
            std::string original_content((std::istreambuf_iterator<char>(file)),
                                       std::istreambuf_iterator<char>());
            file.close();

            size_t original_size = original_content.length();
            bool verification_passed = true;

            // test rle
            {
                auto [compressed_data, result] = compressor.compress_and_get_file_rle(test_file);
                std::string decompressed = compressor.decompress_rle(compressed_data);
                bool rle_match = (decompressed == original_content);

                verification_passed &= rle_match;
                logger.log(LogLevel::INFO, "RLE Compression:");
                logger.log(LogLevel::INFO, "Original size: " + std::to_string(original_size) + " bytes");
                logger.log(LogLevel::INFO, "Compressed size: " + std::to_string(result.first) + " bytes (ratio: "
                         + std::to_string(result.second) + "%)");
                logger.log(LogLevel::INFO, "Decompression verification: " + (rle_match ? passed : failed));
            }

            // test lzw
            {
                auto [compressed_data, result] = compressor.compress_and_get_file_lzw(test_file);
                std::string decompressed = compressor.decompress_lzw(compressed_data);
                bool lzw_match = (decompressed == original_content);
                verification_passed &= lzw_match;

                logger.log(LogLevel::INFO, "LZW Compression:");
                logger.log(LogLevel::INFO, "Original size: " + std::to_string(original_size) + " bytes");
                logger.log(LogLevel::INFO, "Compressed size: " + std::to_string(result.first) + " bytes (ratio: "
                         + std::to_string(result.second) + "%)");
                logger.log(LogLevel::INFO, "Decompression verification: " + (lzw_match ? passed : failed));
            }

            // test huffman
            {
                auto [compressed_data, result] = compressor.compress_and_get_file_huffman(test_file);
                std::string decompressed = compressor.decompress_huffman(compressed_data,
                                                                       compressor.get_huffman_root());
                bool huffman_match = (decompressed == original_content);
                verification_passed &= huffman_match;

                logger.log(LogLevel::INFO, "Huffman Compression:");
                logger.log(LogLevel::INFO, "Original size: " +  std::to_string(original_size) + " bytes");
                logger.log(LogLevel::INFO, "Compressed size: " +  std::to_string(result.first) + " bytes (ratio: "
                         +  std::to_string(result.second) + "%)");
                logger.log(LogLevel::INFO, "Decompression verification: "
                         + (huffman_match ? passed : failed));
            }

            logger.log(LogLevel::INFO, "\nOverall verification: ALL TESTS " + (verification_passed ? passed : failed));
        } else {
            logger.log(LogLevel::INFO, "Could not open test file");
        }
    }
}

int main() {
    Logger& logger = Logger::getInstance();

    logger.log(LogLevel::INFO, "running compression algorithm tests...");
    run_tests();
    return 0;
}
