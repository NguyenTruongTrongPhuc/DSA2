#include "kDTree.hpp"

/* TODO: You can implement methods, functions that support your data structures here.
 * */

kDTree::kDTree(int k) : k(k), root(nullptr) {}

kDTree::~kDTree() {
    // Properly delete all nodes to avoid memory leaks
    deleteTree(root);
}

void kDTree::deleteTree(kDTreeNode* node) {
    if (node != nullptr) {
        deleteTree(node->left);
        deleteTree(node->right);
        delete node;
    }
}

kDTree::kDTree(const kDTree& other) : k(other.k), root(copyNodes(other.root)) {}

kDTreeNode* kDTree::copyNodes(kDTreeNode* node) {
    if (node == nullptr) return nullptr;
    return new kDTreeNode(node->data, copyNodes(node->left), copyNodes(node->right));
}

kDTree& kDTree::operator=(const kDTree& other) {
    if (this != &other) {
        deleteTree(root);
        root = copyNodes(other.root);
        k = other.k;
    }
    return *this;
}

void kDTree::insert(const std::vector<int>& point) {
    root = insertRecursive(root, point, 0);
}

kDTreeNode* kDTree::insertRecursive(kDTreeNode* node, const std::vector<int>& point, int depth) {
    if (!node) {
        return new kDTreeNode(point);
    }
    int cd = depth % k;
    if (point[cd] < node->data[cd]) {
        node->left = insertRecursive(node->left, point, depth + 1);
    } else {
        node->right = insertRecursive(node->right, point, depth + 1);
    }
    return node;
}

void kDTree::remove(const std::vector<int>& point) {
    root = removeRecursive(root, point, 0);
}

kDTreeNode* kDTree::findMin(kDTreeNode* node, int d, int depth) {
    if (node == nullptr) return nullptr;
    int cd = depth % k;

    if (cd == d) {
        if (node->left == nullptr) return node;
        else return findMin(node->left, d, depth + 1);
    }
    return minNode(node, findMin(node->left, d, depth + 1), findMin(node->right, d, depth + 1), d);
}

kDTreeNode* kDTree::minNode(kDTreeNode* x, kDTreeNode* y, kDTreeNode* z, int d) {
    kDTreeNode* res = x;
    if (y != nullptr && y->data[d] < res->data[d]) res = y;
    if (z != nullptr && z->data[d] < res->data[d]) res = z;
    return res;
}

kDTreeNode* kDTree::removeRecursive(kDTreeNode* node, const std::vector<int>& point, int depth) {
    // To be implemented: Deletion logic considering dimensions
    if (node == nullptr) return nullptr;

    int cd = depth % k;
    if (point == node->data) {
        if (node->right != nullptr) {
            kDTreeNode* minNode = findMin(node->right, cd, 0);
            node->data = minNode->data;
            node->right = removeRecursive(node->right, minNode->data, depth + 1);
        } else if (node->left != nullptr) {
            kDTreeNode* minNode = findMin(node->left, cd, 0);
            node->data = minNode->data;
            node->right = removeRecursive(node->left, minNode->data, depth + 1);
            node->left = nullptr;
        } else {
            delete node;
            return nullptr;
        }
    } else if (point[cd] < node->data[cd]) {
        node->left = removeRecursive(node->left, point, depth + 1);
    } else {
        node->right = removeRecursive(node->right, point, depth + 1);
    }
    return node;
}

bool kDTree::search(const std::vector<int>& point) const {
    return searchRecursive(root, point, 0);
}

bool kDTree::searchRecursive(kDTreeNode* node, const std::vector<int>& point, int depth) const {
    // To be implemented: Search logic considering dimensions
    if (node == nullptr) return false;
    if (point == node->data) return true;

    int cd = depth % k;
    if (point[cd] < node->data[cd]) {
        return searchRecursive(node->left, point, depth + 1);
    } else {
        return searchRecursive(node->right, point, depth + 1);
    }
}

void kDTree::buildTree(const std::vector<std::vector<int>>& pointList) {
    for (const auto& point : pointList) {
        insert(point);
    }
}

void kDTree::inorderTraversal() const {
    inorderRecursive(root);
}

void kDTree::inorderRecursive(kDTreeNode* node) const {
    if (node) {
        inorderRecursive(node->left);
        printNode(node);
        inorderRecursive(node->right);
    }
}

void kDTree::preorderTraversal() const {
    preorderRecursive(root);
}

void kDTree::preorderRecursive(kDTreeNode* node) const {
    if (node) {
        printNode(node);
        preorderRecursive(node->left);
        preorderRecursive(node->right);
    }
}

void kDTree::postorderTraversal() const {
    postorderRecursive(root);
}

void kDTree::postorderRecursive(kDTreeNode* node) const {
    if (node) {
        postorderRecursive(node->left);
        postorderRecursive(node->right);
        printNode(node);
    }
}

int kDTree::height() const {
    return heightRecursive(root);
}

int kDTree::heightRecursive(kDTreeNode* node) const {
    if (!node) {
        return 0;
    } else {
        int lHeight = heightRecursive(node->left);
        int rHeight = heightRecursive(node->right);
        return std::max(lHeight, rHeight) + 1;
    }
}

int kDTree::nodeCount() const {
    return nodeCountRecursive(root);
}

int kDTree::nodeCountRecursive(kDTreeNode* node) const {
    if (!node) {
        return 0;
    } else {
        return 1 + nodeCountRecursive(node->left) + nodeCountRecursive(node->right);
    }
}

int kDTree::leafCount() const {
    return leafCountRecursive(root);
}

int kDTree::leafCountRecursive(kDTreeNode* node) const {
    if (!node) {
        return 0;
    } else if (!node->left && !node->right) {
        return 1;
    } else {
        return leafCountRecursive(node->left) + leafCountRecursive(node->right);
    }
}

void kDTree::printNode(kDTreeNode* node) const {
    std::cout << '(';
    for (size_t i = 0; i < node->data.size(); ++i) {
        std::cout << node->data[i];
        if (i < node->data.size() - 1) std::cout << ", ";
    }
    std::cout << ')' << std::endl;
}