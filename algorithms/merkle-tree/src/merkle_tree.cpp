// MIT License
// Copyright (c) 2025 dbjwhs

#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include <cassert>
#include <format>
#include "../../../headers/project_utils.hpp"

// merkle trees, invented by ralph merkle in 1979, are cryptographic data structures that enable efficient and
// secure verification of large data sets. they work by creating a binary tree of hashes where each leaf
// node contains the hash of a data block and each non-leaf node contains the hash of its children. this
// creates a final root hash that serves as a cryptographic fingerprint of all the data.
//
// the structure gained widespread recognition through its implementation in bitcoin (2009), where it enables
// simplified payment verification (spv). in bitcoin, merkle trees allow wallet clients to verify transactions
// without downloading the entire blockchain by only requiring the merkle root and a small merkle proof.
//
// git version control (2005) also leverages a variation of merkle trees in its object model, where each
// commit contains a tree hash that represents the entire state of the repository. this allows git to
// efficiently detect changes and verify data integrity across distributed repositories.
//
// key properties that make merkle trees valuable:
// - verify data integrity without holding the complete dataset (used in bitcoin spv)
// - efficiently identify changes between datasets (used in git)
// - detect data tampering with minimal computational overhead
// - provide cryptographic proofs of data inclusion
// - enable lightweight client verification in distributed systems
//
class MerkleTree {
private:
    // member variables with m_ prefix
    std::vector<std::string> m_leaves;     // stores the leaf node hashes
    std::vector<std::string> m_tree;       // stores the complete tree structure
    std::string m_root_hash;               // stores the root hash value

    // calculate hash of input data using std::hash
    //
    // note: this implementation uses std::hash which is NOT cryptographically secure and should not be used
    // in production systems. std::hash is designed for hash tables and provides no cryptographic guarantees
    // against collision attacks or preimage attacks. in a real implementation, you should use a cryptographic
    // hash function like sha-256 or blake2 from a trusted crypto library such as openssl, botan, or libsodium.
    // I use std::hash here only to demonstrate the merkle tree concept and data structure patterns.
    //
    // vulnerabilities of std::hash:
    // 1. collision attacks: an attacker can easily find different inputs that produce the same hash
    //    example: std::hash may simply add character values, so "ab" and "ba" could produce the same hash
    // 2. predictable outputs: std::hash implementations are often simple algorithms like fnv or multiplicative hash,
    //    making it easy to predict outputs and generate collisions
    // 3. no avalanche effect: small input changes may not significantly change the output
    //    example: "password123" and "password124" might have very similar hashes
    //
    // a cryptographically secure hash function must provide:
    // 1. collision resistance: computationally infeasible to find two different inputs with same hash
    // 2. preimage resistance: given a hash, infeasible to find any input that produces that hash
    // 3. second preimage resistance: given an input, infeasible to find another input with same hash
    // 4. avalanche effect: small input change causes significant change in output
    //
    // example of proper implementation using openssl:
    /*
        #include <openssl/sha.h>

        std::string calculateHash(const std::string& data) {
            unsigned char hash[SHA256_DIGEST_LENGTH];
            SHA256_CTX sha256;
            SHA256_Init(&sha256);
            SHA256_Update(&sha256, data.c_str(), data.size());
            SHA256_Final(hash, &sha256);

            std::stringstream ss;
            for(int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
                ss << std::hex << std::setw(2) << std::setfill('0')
                   << static_cast<int>(hash[i]);
            }
            return ss.str();
        }
    */
    //
    // or using libsodium (modern, preferred):
    /*
        #include <sodium.h>

        std::string calculateHash(const std::string& data) {
            unsigned char hash[crypto_generichash_BYTES];
            crypto_generichash(hash, sizeof(hash),
                              reinterpret_cast<const unsigned char*>(data.c_str()),
                              data.size(),
                              nullptr, 0);

            std::stringstream ss;
            for(int i = 0; i < crypto_generichash_BYTES; i++) {
                ss << std::hex << std::setw(2) << std::setfill('0')
                   << static_cast<int>(hash[i]);
            }
            return ss.str();
        }
    */
    //
    // real-world impact example:
    // in 2017, a collision attack was demonstrated against sha-1 (cost ~$110k to execute).
    // if std::hash were used instead of a cryptographic hash in a blockchain or git:
    // - attackers could create different transactions with same hash
    // - multiple repository states could have identical commit hashes
    // - data integrity verification would be compromised
    // these vulnerabilities would make the system unusable for secure applications
    [[nodiscard]] static std::string calculateHash(const std::string& data) {
        constexpr std::hash<std::string> hasher;
        const size_t hash = hasher(data);

        // convert hash to hex string
        std::stringstream ss;
        ss << std::hex << std::setw(16) << std::setfill('0') << hash;
        return ss.str();
    }

    // build the merkle tree from leaf nodes
    void buildTree() {
        m_tree = m_leaves;

        // continue until we reach the root node
        while (m_tree.size() > 1) {
            std::vector<std::string> new_level;

            // process pairs of nodes
            for (size_t ndx = 0; ndx < m_tree.size(); ndx += 2) {
                if (ndx + 1 < m_tree.size()) {
                    // concatenate and hash a pair of nodes
                    new_level.push_back(calculateHash(m_tree[ndx] + m_tree[ndx + 1]));
                } else {
                    // handle odd number of nodes by duplicating the last node
                    new_level.push_back(m_tree[ndx]);
                }
            }
            m_tree = new_level;
        }

        // set root hash if a tree is not empty
        m_root_hash = m_tree.empty() ? "" : m_tree[0];
    }

public:
    // construct a tree from vector of data blocks
    explicit MerkleTree(const std::vector<std::string>& data) {
        for (const auto& block : data) {
            m_leaves.push_back(calculateHash(block));
        }
        buildTree();
    }

    // get the root hash
    [[nodiscard]] std::string getRootHash() const {
        return m_root_hash;
    }

    // verify if a data block exists in the tree
    [[nodiscard]] bool verifyData(const std::string& data) const {
        const std::string hash = calculateHash(data);
        return std::ranges::find(m_leaves, hash) != m_leaves.end();
    }

    // get number of leaves for testing
    [[nodiscard]] size_t getLeafCount() const {
        return m_leaves.size();
    }
};

// test basic merkle tree properties
void testBasicProperties() {
    Logger::getInstance().log(LogLevel::INFO, "starting basic property tests");

    // test an empty tree
    {
        std::vector<std::string> empty_data;
        MerkleTree tree(empty_data);
        assert(tree.getRootHash().empty());
        assert(tree.getLeafCount() == 0);
        Logger::getInstance().log(LogLevel::INFO, "empty tree test passed");
    }

    // test a single node tree
    {
        std::vector<std::string> single_data = {"test data"};
        MerkleTree tree(single_data);
        assert(!tree.getRootHash().empty());
        assert(tree.getLeafCount() == 1);
        assert(tree.verifyData("test data"));
        assert(!tree.verifyData("wrong data"));
        Logger::getInstance().log(LogLevel::INFO, std::format(
            "single node tree test passed, hash: {}",
            tree.getRootHash()
        ));
    }

    // test consistency
    {
        std::vector<std::string> data = {"test1", "test2"};
        MerkleTree tree1(data);
        MerkleTree tree2(data);
        assert(tree1.getRootHash() == tree2.getRootHash());
        assert(tree1.getLeafCount() == 2);
        Logger::getInstance().log(LogLevel::INFO, "consistency test passed");
    }
}

// generate random string of specified length
std::string generateRandomString(const size_t length) {
    static const std::string chars =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

    RandomGenerator randomRange(0, chars.size() - 1);

    std::string result;
    result.reserve(length);
    for (size_t ndx = 0; ndx < length; ++ndx) {
        result += chars[randomRange.getNumber()];
    }
    return result;
}

// test complex tree scenarios
void testComplexTree() {
    Logger::getInstance().log(LogLevel::INFO, "starting complex tree tests");

    // create a large dataset with random strings
    std::vector<std::string> large_dataset;
    constexpr size_t num_elements = 1000;
    constexpr size_t str_length = 50;

    Logger::getInstance().log(LogLevel::INFO, std::format(
        "generating {} random strings of length {}",
        num_elements,
        str_length
    ));

    // generate random strings
    large_dataset.reserve(num_elements);
    for (size_t ndx = 0; ndx < num_elements; ++ndx) {
        large_dataset.push_back(generateRandomString(str_length));
    }

    // create merkle tree from large dataset
    MerkleTree large_tree(large_dataset);

    // verify all data can be found in a tree
    Logger::getInstance().log(LogLevel::INFO, "verifying all data elements...");
    for (const auto& data : large_dataset) {
        assert(large_tree.verifyData(data));
    }

    // verify tree properties
    assert(large_tree.getLeafCount() == num_elements);
    assert(!large_tree.getRootHash().empty());

    // test data integrity
    Logger::getInstance().log(LogLevel::INFO, "testing data integrity...");

    // modify one element and verify root hash changes
    std::vector<std::string> modified_dataset = large_dataset;
    modified_dataset[num_elements/2] = "modified_data";
    MerkleTree modified_tree(modified_dataset);

    assert(large_tree.getRootHash() != modified_tree.getRootHash());
    assert(!modified_tree.verifyData(large_dataset[num_elements/2]));
    assert(modified_tree.verifyData("modified_data"));

    Logger::getInstance().log(LogLevel::INFO, std::format(
        "complex tree tests passed, "
        "original root hash: {}, "
        "modified root hash: {}",
        large_tree.getRootHash(),
        modified_tree.getRootHash()
    ));

    // test different tree sizes
    Logger::getInstance().log(LogLevel::INFO, "testing different tree sizes...");

    for (std::vector<size_t> test_sizes = {2, 3, 4, 7, 8, 15, 16}; size_t size : test_sizes) {
        std::vector<std::string> data;
        data.reserve(size);
        for (size_t ndx = 0; ndx < size; ++ndx) {
            data.push_back(std::format("test_data_{}", ndx));
        }

        MerkleTree tree(data);
        assert(tree.getLeafCount() == size);

        Logger::getInstance().log(LogLevel::INFO, std::format(
            "tree with {} leaves created successfully",
            size
        ));
    }
}

int main() {
    try {
        Logger::getInstance().log(LogLevel::INFO, "starting merkle tree tests");

        // run basic property tests
        testBasicProperties();

        // run complex tree tests
        testComplexTree();

        Logger::getInstance().log(LogLevel::INFO, "all tests completed successfully");
        return 0;

    } catch (const std::exception& e) {
        Logger::getInstance().log(LogLevel::CRITICAL, std::format(
            "test execution failed: {}",
            e.what()
        ));
        return 1;
    }
}