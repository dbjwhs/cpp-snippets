// MIT License
// Copyright (c) 2025 dbjwhs

#include <iostream>
#include <stack>
#include <cassert>
#include <functional>
#include <optional>
#include <string>
#include "../../../headers/project_utils.hpp"

// binary search tree implementation
// key properties:
// - works with any type that supports operator< and operator==
// - maintains bst invariants for arbitrary comparable types
// - provides O(log n) search efficiency when balanced
template<typename value_type>
class BinaryTree {
private:
    // internal node structure
    struct Node {
        value_type m_data;
        std::unique_ptr<Node> m_left;
        std::unique_ptr<Node> m_right;

        // note std::move() vs const value_type data; this is better for heavy objects,
        // moving is typically much less expensive than copying.
        // (std::string, classes, etc.), yes I know for primitive types (int, double, etc.),
        // move and copy operations are equivalent.
        explicit Node(value_type  data)
            : m_data(std::move(data)), m_left(nullptr), m_right(nullptr) {}
    };

    std::unique_ptr<Node> m_root;
    size_t m_size;

    // helper function to copy all nodes recursively
    std::unique_ptr<Node> copyTree(const Node *node) {    // NOLINT(misc-no-recursion)
        if (!node) {
            return nullptr;
        }
        auto newNode = std::make_unique<Node>(node->m_data);
        newNode->m_left = copyTree(node->m_left.get());
        newNode->m_right = copyTree(node->m_right.get());
        return newNode;
    }

    // helper function for BST insertion
    // maintains BST property by recursively finding the correct position
    // returns: updated node pointer after insertion
    std::unique_ptr<Node>& insertHelper(std::unique_ptr<Node>& node, const value_type& value) {  // NOLINT(misc-no-recursion)
        if (!node) {
            node = std::make_unique<Node>(value);
            return node;
        }

        if (value < node->m_data) {
            insertHelper(node->m_left, value);
        } else if (node->m_data < value) {  // using operator< for consistency
            insertHelper(node->m_right, value);
        }
        return node;
    }

    // helper function for bst search
    // uses bst property for O(log n) search
    bool searchHelper(const Node* node, const value_type& value) const {  // NOLINT(misc-no-recursion)
        if (!node) return false;
        if (!(node->m_data < value) && !(value < node->m_data)) return true; // equivalent to ==
        if (value < node->m_data) return searchHelper(node->m_left.get(), value);
        return searchHelper(node->m_right.get(), value);
    }

    // helper function to validate bst property
    // uses optional bounds to work with any comparable type
    bool isValidBSTHelper(const Node* node, /* NOLINT(misc-no-recursion) */
                         const std::optional<value_type>& min_value = std::nullopt,
                         const std::optional<value_type>& max_value = std::nullopt) const {  // NOLINT(misc-no-recursion)
        if (!node) {
            return true;
        }

        // check bounds if they exist
        if ((min_value && !(min_value.value() < node->m_data)) ||(max_value && !(node->m_data < max_value.value()))) {
            return false;
        }

        return isValidBSTHelper(node->m_left.get(), min_value, node->m_data) &&
               isValidBSTHelper(node->m_right.get(), node->m_data, max_value);
    }

    // helper to find minimum value in a subtree
    static const Node *findMin(const Node *node) {
        if (!node) {
            return nullptr;
        }
        while (node->m_left) {
            node = node->m_left.get();
        }
        return node;
    }

    // helper to find maximum value in a subtree
    static const Node *findMax(const Node *node) {
        if (!node) {
            return nullptr;
        }
        while (node->m_right) {
            node = node->m_right.get();
        }
        return node;
    }

public:
    // constructors and destructor
    BinaryTree() : m_root(nullptr), m_size(0) {}

    // copy constructor
    BinaryTree(const BinaryTree& other) : m_root(nullptr), m_size(0) {
        m_root = copyTree(other.m_root.get());
        m_size = other.m_size;
    }

    // destructor - std::unique_ptr handles cleanup
    ~BinaryTree() = default;

    // assignment operator
    BinaryTree& operator=(const BinaryTree& other) {
        if (this != &other) {
            m_root = copyTree(other.m_root.get());
            m_size = other.m_size;
        }
        return *this;
    }

    // insert value into BST maintaining BST property
    void insert(const value_type& value) {
        if (!search(value)) {  // only insert if value doesn't exist
            insertHelper(m_root, value);
            m_size++;
        }
    }

    // search for value in BST, O(log n) when balanced
    [[nodiscard]] bool search(const value_type& value) const {
        return searchHelper(m_root.get(), value);
    }

    // find minimum value in a tree
    [[nodiscard]] value_type findMinValue() const {
        const Node *min_node = findMin(m_root.get());
        if (!min_node) {
            throw std::runtime_error("tree is empty");
        }
        return min_node->m_data;
    }

    // find maximum value in a tree
    [[nodiscard]] value_type findMaxValue() const {
        const Node *max_node = findMax(m_root.get());
        if (!max_node) {
            throw std::runtime_error("tree is empty");
        }
        return max_node->m_data;
    }

    // inorder traversal visits nodes in ascending order for a BST
    // algorithm:
    // 1. create stack to track nodes during traversal
    // 2. traverse left subtree to its leftmost node, pushing each node to stack
    // 3. when leftmost reached, pop and process node, then traverse its right child
    // 4. continue until all nodes processed
    //
    // time complexity: O(n) where n is number of nodes
    // space complexity: O(h) where h is height of tree
    //
    // parameters:
    //   visit_func - function pointer to process node values during traversal
    //
    // example for tree:    5
    //                    /   \
    //                   3     7
    //                  / \   / \
    //                 2   4 6   8
    // output: 2 3 4 5 6 7 8 (sorted order for BST)
    void inOrderTraversal(const std::function<void(const value_type&)>& visit_func) const {
        std::stack<const Node *> stack_node;
        const Node *current = m_root.get();

        while (current || !stack_node.empty()) {
            while (current) {
                stack_node.push(current);
                current = current->m_left.get();
            }

            current = stack_node.top();
            stack_node.pop();

            visit_func(current->m_data);

            current = current->m_right.get();
        }
    }

    // preorder traversal visits root before children (root-left-right)
    // algorithm:
    // 1. create stack and push root
    // 2. while stack not empty:
    //    - pop and process current node
    //    - push right child (if exists)
    //    - push left child (if exists)
    // 3. continue until stack empty
    //
    // time complexity: O(n) where n is number of nodes
    // space complexity: O(h) where h is height of tree
    //
    // parameters:
    //   visit_func - function pointer to process node values during traversal
    //
    // example for tree:    5
    //                    /   \
    //                   3     7
    //                  / \   / \
    //                 2   4 6   8
    // output: 5 3 2 4 7 6 8 (root before children)
    void preOrderTraversal(const std::function<void(const value_type&)>& visit_func) const {
        if (!m_root) return;

        std::stack<const Node *> stack_node;
        stack_node.push(m_root.get());

        while (!stack_node.empty()) {
            const Node *current = stack_node.top();
            stack_node.pop();

            visit_func(current->m_data);

            if (current->m_right)
                stack_node.push(current->m_right.get());
            if (current->m_left)
                stack_node.push(current->m_left.get());
        }
    }

    // postorder traversal visits nodes after their children (left-right-root)
    // algorithm:
    // 1. use two stacks: s1 for processing, s2 for final order
    // 2. push root to s1
    // 3. while s1 not empty:
    //    - pop node from s1 and push to s2
    //    - push left child to s1 (if exists)
    //    - push right child to s1 (if exists)
    // 4. process s2 to get postorder traversal
    //
    // time complexity: O(n) where n is number of nodes
    // space complexity: O(n) where n is number of nodes
    //
    // parameters:
    //   visit_func - function pointer to process node values during traversal
    //
    // example for tree:    5
    //                    /   \
    //                   3     7
    //                  / \   / \
    //                 2   4 6   8
    // output: 2 4 3 6 8 7 5 (children before root)
    void postOrderTraversal(const std::function<void(const value_type&)>& visit_func) const {
        if (!m_root) return;

        std::stack<const Node *> s1, s2;
        s1.push(m_root.get());

        while (!s1.empty()) {
            const Node *current = s1.top();
            s1.pop();
            s2.push(current);

            if (current->m_left)
                s1.push(current->m_left.get());
            if (current->m_right)
                s1.push(current->m_right.get());
        }

        while (!s2.empty()) {
            visit_func(s2.top()->m_data);
            s2.pop();
        }
    }

    // utility methods
    [[nodiscard]] size_t size() const { return m_size; }
    [[nodiscard]] bool empty() const { return m_size == 0; }  // NOLINT(readability-container-size-empty)

    // validates that the tree follows binary search tree properties where:
    //
    // 1. bst invariants:
    //    - for any node n, all nodes in n's left subtree have values < n
    //    - for any node n, all nodes in n's right subtree have values > n
    //    - no duplicate values allowed
    //
    // 2. validation approach:
    //    - uses a recursive helper that tracks valid range for each node
    //    - root can be any value
    //    - left children must be less than parent
    //    - right children must be greater than parent
    //    - range narrows as we traverse down the tree
    //
    // time complexity:  O(n) where n is number of nodes (visits each node once)
    // space complexity: O(h) where h is height of tree (recursion stack)
    //
    // example of valid bst:    5           example of invalid bst:    5
    //                        /   \                                  /   \
    //                       3     7                                3     4
    //                      / \   / \                             / \   / \
    //                     2   4 6   8                           1   6 2   7
    //
    // returns:
    //   true  - if tree is empty or follows all bst properties
    //   false - if any bst property is violated
    //
    [[nodiscard]] bool isValidBST() const {
        return isValidBSTHelper(m_root.get());
    }

    // find the maximum depth of the tree
    [[nodiscard]] int maxDepth() const {
        return maxDepthHelper(m_root.get());
    }

private:
    int maxDepthHelper(const Node *node) const {  // NOLINT(misc-no-recursion)
        if (!node) return 0;
        return 1 + std::max(maxDepthHelper(node->m_left.get()), maxDepthHelper(node->m_right.get()));
    }
};

// test implementation with multiple types
void printInt(const int& value) {
    Logger logger("../custom.log");
    logger.log(LogLevel::INFO, std::to_string(value) + " ");
}

void printString(const std::string& value) {
    Logger logger("../custom.log");
    logger.log(LogLevel::INFO, value + " ");
}

int main() {
    Logger logger("../custom.log");

    // test with integers
    BinaryTree<int> tree;

    // note having been exposed to codebases that ship with asserts() I have found
    // this "You shall not pass!" technique solid. e.g., fail at first error so you
    // know exactly where you failed.

    // test an empty tree
    assert(tree.empty());
    assert(tree.size() == 0);   // NOLINT(readability-container-size-empty)
    assert(tree.maxDepth() == 0);
    assert(tree.isValidBST());
    logger.log(LogLevel::INFO, "empty tree tests passed!");

    // test BST insertion and search
    tree.insert(5);  // root
    tree.insert(3);  // left of 5
    tree.insert(7);  // right of 5
    tree.insert(2);  // left of 3
    tree.insert(4);  // right of 3
    tree.insert(6);  // left of 7
    tree.insert(8);  // right of 7

    // verify BST property
    assert(tree.isValidBST());
    logger.log(LogLevel::INFO, "BST property validation passed!");

    // test search functionality
    assert(tree.search(5));  // root
    assert(tree.search(2));  // leaf
    assert(tree.search(7));  // internal node
    assert(tree.search(1) == false); // non-existent value
    assert(tree.search(9) == false); // non-existent value
    logger.log(LogLevel::INFO, "search functionality tests passed!");

    // test duplicate insertion
    const size_t size_before = tree.size();
    tree.insert(5);  // should not insert
    assert(tree.size() == size_before);
    logger.log(LogLevel::INFO, "duplicate handling tests passed!");

    // test min/max functions
    assert(tree.findMinValue() == 2);
    assert(tree.findMaxValue() == 8);
    logger.log(LogLevel::INFO, "min/max value tests passed!");

    // test traversals
    std::vector<int> inorder_int_result;
    std::vector<int> preorder_int_result;
    std::vector<int> postorder_int_result;

    // capture traversal results
    auto captureInorder = [&inorder_int_result](const int& value) {
        inorder_int_result.push_back(value);
    };
    auto capturePreorder = [&preorder_int_result](const int& value) {
        preorder_int_result.push_back(value);
    };
    auto capturePostorder = [&postorder_int_result](const int& value) {
        postorder_int_result.push_back(value);
    };

    // perform traversals
    tree.inOrderTraversal(captureInorder);
    tree.preOrderTraversal(capturePreorder);
    tree.postOrderTraversal(capturePostorder);

    // verify inorder traversal (should be sorted for BST)
    const std::vector<int> expected_inorder = {2, 3, 4, 5, 6, 7, 8};
    assert(inorder_int_result == expected_inorder);
    logger.log(LogLevel::INFO, "inorder traversal verification passed!");

    // verify preorder traversal
    const std::vector<int> expected_preorder = {5, 3, 2, 4, 7, 6, 8};
    assert(preorder_int_result == expected_preorder);
    logger.log(LogLevel::INFO, "preorder traversal verification passed!");

    // verify postorder traversal
    const std::vector<int> expected_postorder = {2, 4, 3, 6, 8, 7, 5};
    assert(postorder_int_result == expected_postorder);
    logger.log(LogLevel::INFO, "postorder traversal verification passed!");

    // test empty tree traversals
    const BinaryTree<int> empty_tree;
    std::vector<int> empty_int_result;

    auto captureEmpty = [&empty_int_result](const int& value) {
        empty_int_result.push_back(value);
    };

    empty_tree.inOrderTraversal(captureEmpty);
    assert(empty_int_result.empty());
    logger.log(LogLevel::INFO, "inOrderTraversal tree traversal tests passed!");

    empty_tree.preOrderTraversal(captureEmpty);
    assert(empty_int_result.empty());
    logger.log(LogLevel::INFO, "preOrderTraversal tree traversal tests passed!");

    empty_tree.postOrderTraversal(captureEmpty);
    assert(empty_int_result.empty());
    logger.log(LogLevel::INFO, "postOrderTraversal tree traversal tests passed!");

    // test single node tree traversals
    BinaryTree<int> single_node_tree;
    single_node_tree.insert(1);
    std::vector<int> single_int_result;

    auto captureSingle = [&single_int_result](const int& value) {
        single_int_result.push_back(value);
    };

    // all traversals should give the same result for a single node
    single_node_tree.inOrderTraversal(captureSingle);
    assert(single_int_result == std::vector<int>{1});
    single_int_result.clear();
    logger.log(LogLevel::INFO, "inOrderTraversal node traversal tests passed!");

    single_node_tree.preOrderTraversal(captureSingle);
    assert(single_int_result == std::vector<int>{1});
    single_int_result.clear();
    logger.log(LogLevel::INFO, "preOrderTraversal node traversal tests passed!");

    single_node_tree.postOrderTraversal(captureSingle);
    assert(single_int_result == std::vector<int>{1});
    logger.log(LogLevel::INFO, "postOrderTraversal node traversal tests passed!");

    // print for visual verification
    logger.log(LogLevel::INFO, "visual verification of traversals:");
    logger.log(LogLevel::INFO, "inorder traversal:");
    tree.inOrderTraversal(printInt);
    logger.log(LogLevel::INFO, "preorder traversal:");
    tree.preOrderTraversal(printInt);
    logger.log(LogLevel::INFO, "postorder traversal:");
    tree.postOrderTraversal(printInt);
    logger.log(LogLevel::INFO, "");

    // test copy constructor
    const BinaryTree<int> tree2 = tree;
    assert(tree2.isValidBST());
    assert(tree2.size() == tree.size());
    assert(tree2.findMinValue() == tree.findMinValue());
    assert(tree2.findMaxValue() == tree.findMaxValue());
    logger.log(LogLevel::INFO, "copy constructor tests passed!");

    // test assignment operator
    const BinaryTree<int> tree3 = tree;
    assert(tree3.isValidBST());
    assert(tree3.size() == tree.size());
    assert(tree3.findMinValue() == tree.findMinValue());
    assert(tree3.findMaxValue() == tree.findMaxValue());
    logger.log(LogLevel::INFO, "assignment operator tests passed!");

    logger.log(LogLevel::INFO, "all int BST tests passed successfully!");

    // test with strings
    BinaryTree<std::string> string_tree;

    // basic string tests
    assert(string_tree.empty());
    string_tree.insert("hello");
    string_tree.insert("abc");
    string_tree.insert("xyz");
    assert(string_tree.isValidBST());
    assert(string_tree.findMinValue() == "abc");
    assert(string_tree.findMaxValue() == "xyz");

    // test string traversals
    std::vector<std::string> inorder_string_result;
    auto captureString = [&inorder_string_result](const std::string& value) {
        inorder_string_result.push_back(value);
    };

    string_tree.inOrderTraversal(captureString);
    std::vector<std::string> expected;
    expected.emplace_back("abc");
    expected.emplace_back("hello");
    expected.emplace_back("xyz");
    assert(inorder_string_result == expected);
    logger.log(LogLevel::INFO, "string tree tests passed!");

    // visual verification
    logger.log(LogLevel::INFO, "string tree inorder traversal:");
    string_tree.inOrderTraversal(printString);
    logger.log(LogLevel::INFO, "");

    logger.log(LogLevel::INFO, "all generic type tests passed successfully!");
    return 0;
}
