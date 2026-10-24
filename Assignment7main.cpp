// Assignment 07: Binary Search Tree Operations
// Author: Talan Jewett
// Build:  g++ -std=c++17 -Wall -Wextra -pedantic -o bst main.cpp
// Run:    ./bst
//
// A binary search tree of distinct ints implemented from scratch.
// No std::set / std::map / other tree containers are used; std::vector is
// used only to collect traversal results for printing and checking.

#include <algorithm>
#include <climits>
#include <cstddef>
#include <iostream>
#include <string>
#include <vector>

// ---------------------------------------------------------------------------
// Node type
// ---------------------------------------------------------------------------
struct Node {
    int key;
    Node* left = nullptr;
    Node* right = nullptr;
    explicit Node(int k) : key(k) {}
};

// ---------------------------------------------------------------------------
// BinarySearchTree
// Invariant: for every node, all keys in its left subtree are smaller and all
// keys in its right subtree are larger. Duplicates are rejected.
// ---------------------------------------------------------------------------
class BinarySearchTree {
public:
    BinarySearchTree() = default;
    ~BinarySearchTree() { destroy(root_); }

    // The tree owns raw pointers, so copying is disabled to prevent
    // double-free (rule of three).
    BinarySearchTree(const BinarySearchTree&) = delete;
    BinarySearchTree& operator=(const BinarySearchTree&) = delete;

    // Insert key. Returns false (and changes nothing) if key already exists.
    bool insert(int key) {
        Node** link = &root_;  // address of the pointer we may overwrite
        while (*link != nullptr) {
            if (key < (*link)->key)      link = &(*link)->left;
            else if (key > (*link)->key) link = &(*link)->right;
            else                         return false;  // duplicate
        }
        *link = new Node(key);
        ++size_;
        return true;
    }

    // Search for key. Returns true if present.
    bool contains(int key) const {
        const Node* cur = root_;
        while (cur != nullptr) {
            if (key == cur->key) return true;
            cur = (key < cur->key) ? cur->left : cur->right;
        }
        return false;
    }

    // Number of children of the node holding key, or -1 if key is absent.
    // Used by the demo to label which removal case is being exercised.
    int childCount(int key) const {
        const Node* cur = root_;
        while (cur != nullptr && cur->key != key)
            cur = (key < cur->key) ? cur->left : cur->right;
        if (cur == nullptr) return -1;
        return (cur->left != nullptr) + (cur->right != nullptr);
    }

    // Remove key. Returns false if key was not in the tree.
    bool remove(int key) {
        // 1. Find the link (parent's child pointer, or root_) to the target.
        Node** link = &root_;
        while (*link != nullptr && (*link)->key != key)
            link = (key < (*link)->key) ? &(*link)->left : &(*link)->right;

        Node* target = *link;
        if (target == nullptr) return false;  // not found

        if (target->left != nullptr && target->right != nullptr) {
            // Case 3: two children.
            // Find the inorder successor = minimum of the right subtree.
            Node** succLink = &target->right;
            while ((*succLink)->left != nullptr) succLink = &(*succLink)->left;
            Node* succ = *succLink;

            target->key = succ->key;  // copy successor's key into target
            *succLink = succ->right;  // splice successor out (it has no left child)
            delete succ;
        } else {
            // Case 1 (leaf) and Case 2 (one child):
            // replace the target with its only child, or nullptr for a leaf.
            *link = (target->left != nullptr) ? target->left : target->right;
            delete target;
        }
        --size_;
        return true;
    }

    std::vector<int> inorder() const   { std::vector<int> v; inorder(root_, v);   return v; }
    std::vector<int> preorder() const  { std::vector<int> v; preorder(root_, v);  return v; }
    std::vector<int> postorder() const { std::vector<int> v; postorder(root_, v); return v; }

    // Height measured in edges: empty tree = -1, single node = 0.
    int height() const { return height(root_); }

    std::size_t size() const { return size_; }
    bool empty() const { return root_ == nullptr; }

    // Verifies the BST invariant over the whole tree (used as a test oracle).
    bool isValidBST() const { return isValid(root_, LLONG_MIN, LLONG_MAX); }

    // Prints the tree rotated 90 degrees (root on the left, right subtree up).
    void printSideways(std::ostream& out) const {
        if (root_ == nullptr) { out << "    (empty)\n"; return; }
        printSideways(out, root_, 0);
    }

private:
    Node* root_ = nullptr;
    std::size_t size_ = 0;

    static void destroy(Node* n) {  // postorder delete
        if (n == nullptr) return;
        destroy(n->left);
        destroy(n->right);
        delete n;
    }

    static void inorder(const Node* n, std::vector<int>& out) {
        if (n == nullptr) return;
        inorder(n->left, out);
        out.push_back(n->key);
        inorder(n->right, out);
    }

    static void preorder(const Node* n, std::vector<int>& out) {
        if (n == nullptr) return;
        out.push_back(n->key);
        preorder(n->left, out);
        preorder(n->right, out);
    }

    static void postorder(const Node* n, std::vector<int>& out) {
        if (n == nullptr) return;
        postorder(n->left, out);
        postorder(n->right, out);
        out.push_back(n->key);
    }

    static int height(const Node* n) {
        if (n == nullptr) return -1;
        return 1 + std::max(height(n->left), height(n->right));
    }

    static bool isValid(const Node* n, long long lo, long long hi) {
        if (n == nullptr) return true;
        if (n->key <= lo || n->key >= hi) return false;
        return isValid(n->left, lo, n->key) && isValid(n->right, n->key, hi);
    }

    static void printSideways(std::ostream& out, const Node* n, int depth) {
        if (n == nullptr) return;
        printSideways(out, n->right, depth + 1);
        out << "    " << std::string(static_cast<std::size_t>(depth) * 6, ' ')
            << n->key << '\n';
        printSideways(out, n->left, depth + 1);
    }
};

// ---------------------------------------------------------------------------
// Demo / test helpers
// ---------------------------------------------------------------------------
static std::string toString(const std::vector<int>& v) {
    std::string s = "[";
    for (std::size_t i = 0; i < v.size(); ++i) {
        if (i > 0) s += ' ';
        s += std::to_string(v[i]);
    }
    return s + "]";
}

static bool isStrictlySorted(const std::vector<int>& v) {
    for (std::size_t i = 1; i < v.size(); ++i)
        if (v[i - 1] >= v[i]) return false;
    return true;
}

static int failures = 0;

static void check(bool condition, const std::string& label) {
    std::cout << "  [" << (condition ? "PASS" : "FAIL") << "] " << label << '\n';
    if (!condition) ++failures;
}

static void printTraversals(const BinarySearchTree& t) {
    std::cout << "  Inorder:   " << toString(t.inorder()) << '\n'
              << "  Preorder:  " << toString(t.preorder()) << '\n'
              << "  Postorder: " << toString(t.postorder()) << '\n'
              << "  Height: " << t.height() << "   Size: " << t.size() << '\n';
}

static void demoRemoval(BinarySearchTree& t, int key, int expectedChildren,
                        const std::string& caseName) {
    std::cout << "\n--- Remove " << key << " (" << caseName << ") ---\n";
    int children = t.childCount(key);
    std::cout << "  Node " << key << " has " << children << " child(ren) before removal\n";
    check(children == expectedChildren, "node is the expected removal case");

    std::size_t before = t.size();
    bool removed = t.remove(key);
    check(removed, "remove(" + std::to_string(key) + ") returned true");
    check(!t.contains(key), std::to_string(key) + " is no longer found");
    check(t.size() == before - 1, "size decreased by 1");

    std::vector<int> in = t.inorder();
    std::cout << "  Inorder after removal: " << toString(in) << '\n';
    check(isStrictlySorted(in), "inorder traversal is still sorted");
    check(t.isValidBST(), "BST invariant holds for every node");
    std::cout << "  Tree now (rotated, root at left):\n";
    t.printSideways(std::cout);
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
int main() {
    std::cout << "=== Assignment 07: Binary Search Tree Operations ===\n";

    BinarySearchTree tree;
    check(tree.empty() && tree.height() == -1, "empty tree has height -1");

    // 1. Insert 11 distinct integers (order chosen to give a balanced-ish tree).
    const std::vector<int> keys = {50, 30, 70, 20, 40, 60, 80, 35, 45, 65, 85};
    std::cout << "\n--- Insert " << keys.size() << " distinct keys ---\n";
    std::cout << "  Insertion order: " << toString(keys) << '\n';
    for (int k : keys) tree.insert(k);
    check(tree.size() == keys.size(), "size equals number of keys inserted");
    check(!tree.insert(40), "duplicate insert(40) is rejected");
    check(tree.size() == keys.size(), "size unchanged after duplicate");

    std::cout << "\n  Tree (rotated, root at left):\n";
    tree.printSideways(std::cout);
    std::cout << '\n';
    printTraversals(tree);
    check(isStrictlySorted(tree.inorder()), "inorder traversal is sorted");
    check(tree.isValidBST(), "BST invariant holds");
    check(tree.height() == 3, "height is 3 for this insertion order");

    // 2. Search: one present key, one absent key.
    std::cout << "\n--- Search ---\n";
    std::cout << "  contains(45) -> " << std::boolalpha << tree.contains(45) << '\n';
    check(tree.contains(45), "present key 45 is found");
    std::cout << "  contains(99) -> " << tree.contains(99) << '\n';
    check(!tree.contains(99), "absent key 99 is not found");

    // 3. Removal: all three cases.
    demoRemoval(tree, 20, 0, "Case 1: leaf");
    demoRemoval(tree, 60, 1, "Case 2: one child");
    demoRemoval(tree, 50, 2, "Case 3: two children, root; successor 65 replaces it");

    std::cout << "\n--- Remove absent key ---\n";
    std::size_t before = tree.size();
    check(!tree.remove(999), "remove(999) on absent key returns false");
    check(tree.size() == before, "size unchanged");

    std::cout << "\n--- Final state ---\n";
    printTraversals(tree);

    // 4. Worst case: sorted insertion produces a degenerate (linked-list) tree.
    std::cout << "\n--- Worst case: insert 1..10 in sorted order ---\n";
    BinarySearchTree chain;
    for (int k = 1; k <= 10; ++k) chain.insert(k);
    std::cout << "  Preorder:  " << toString(chain.preorder()) << '\n'
              << "  Height: " << chain.height() << "   Size: " << chain.size() << '\n';
    check(chain.height() == 9, "height is n - 1 = 9 (degenerate tree)");
    check(chain.contains(10), "search for 10 must visit all 10 nodes");

    std::cout << "\n=== " << (failures == 0 ? "ALL TESTS PASSED" : "SOME TESTS FAILED")
              << " (" << failures << " failure(s)) ===\n";
    return failures == 0 ? 0 : 1;
}
