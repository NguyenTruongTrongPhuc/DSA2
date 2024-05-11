#include "kDTree.hpp"

const float MAX_FLOAT = 1e30;  // Large enough for most practical purposes

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

////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////

void kDTree::remove(const vector<int>& point) {
    root = removeRecursive(root, point, 0);
}

kDTreeNode* kDTree::removeRecursive(kDTreeNode* node, const vector<int>& point, int depth) {
    if (node == nullptr) return nullptr;

    int cd = depth % k;

    // Nếu node cần xóa được tìm thấy
    if (node->data == point) {
        // Nếu node có một hoặc không có con
        if (node->left == nullptr) {
            kDTreeNode* temp = node->right;
            delete node;
            return temp;
        }
        else if (node->right == nullptr) {
            kDTreeNode* temp = node->left;
            delete node;
            return temp;
        }

        // Nếu node có cả hai con, tìm successor (node nhỏ nhất từ cây con bên phải)
        kDTreeNode* minNode = findMin(node->right, cd, depth + 1);
        node->data = minNode->data;
        node->right = removeRecursive(node->right, minNode->data, depth + 1);
    }
    else if (point[cd] < node->data[cd]) {
        node->left = removeRecursive(node->left, point, depth + 1);
    }
    else {
        node->right = removeRecursive(node->right, point, depth + 1);
    }

    return node;
}

// Hàm tìm node nhỏ nhất trong cây theo chiều dữ liệu cụ thể
kDTreeNode* kDTree::findMin(kDTreeNode* node, int d, int depth) {
    if (node == nullptr) return nullptr;

    int cd = depth % k;

    if (cd == d) {
        if (node->left == nullptr) return node;
        return findMin(node->left, d, depth + 1);
    }

    return minNode(node, findMin(node->left, d, depth + 1), findMin(node->right, d, depth + 1), d);
}

kDTreeNode* kDTree::minNode(kDTreeNode* x, kDTreeNode* y, kDTreeNode* z, int d) {
    kDTreeNode* res = x;
    if (y != nullptr && y->data[d] < (res == nullptr ? y->data[d] : res->data[d])) res = y;
    if (z != nullptr && z->data[d] < (res == nullptr ? z->data[d] : res->data[d])) res = z;
    return res;
}


//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////

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

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

void kDTree::buildTree(const vector<vector<int>>& pointList) {
    vector<vector<int>> modifiableList = pointList;  // Make a modifiable copy of the input
    root = buildTreeHelper(modifiableList, 0);
}

void kDTree::mergeSort(vector<vector<int>>& points, int l, int r, int cd) {
    if (l < r) {
        int m = l + (r - l) / 2;

        mergeSort(points, l, m, cd);
        mergeSort(points, m + 1, r, cd);
        merge(points, l, m, r, cd);
    }
}

void kDTree::merge(vector<vector<int>>& points, int l, int m, int r, int cd) {
    int n1 = m - l + 1;
    int n2 = r - m;

    // Create temp arrays
    vector<vector<int>> L(n1), R(n2);

    for (int i = 0; i < n1; i++)
        L[i] = points[l + i];
    for (int j = 0; j < n2; j++)
        R[j] = points[m + 1 + j];

    // Merge the temp arrays back into points[l..r]
    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        if (L[i][cd] <= R[j][cd]) {
            points[k] = L[i];
            i++;
        }
        else {
            points[k] = R[j];
            j++;
        }
        k++;
    }

    // Copy the remaining elements of L[], if there are any
    while (i < n1) {
        points[k] = L[i];
        i++;
        k++;
    }

    // Copy the remaining elements of R[], if there are any
    while (j < n2) {
        points[k] = R[j];
        j++;
        k++;
    }
}


kDTreeNode* kDTree::buildTreeHelper(vector<vector<int>>& points, int depth) {
    if (points.empty()) {
        return nullptr;
    }

    int cd = depth % k;  // Calculate the current dimension to sort by
    mergeSort(points, 0, points.size() - 1, cd);  // Sort points along the current dimension

    // Choose median index. For even-sized lists, this approach picks the left of the center.
    size_t medianIndex = (points.size() - 1) / 2;

    // Create a new node with the median element
    kDTreeNode* node = new kDTreeNode(points[medianIndex]);

    // Exclude the median point from the left and right subtrees
    vector<vector<int>> leftPoints(points.begin(), points.begin() + medianIndex);
    vector<vector<int>> rightPoints(points.begin() + medianIndex + 1, points.end());

    node->left = buildTreeHelper(leftPoints, depth + 1);  // Recursive build for left subtree
    node->right = buildTreeHelper(rightPoints, depth + 1);  // Recursive build for right subtree

    return node;
}


/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////

void kDTree::inorderTraversal() const {
    inorderRecursive(root);
    cout << endl;
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
    cout << endl;
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
    cout << endl;
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
    if (node) {
        std::cout << '(';
        for (size_t i = 0; i < node->data.size(); ++i) {
            std::cout << node->data[i];
            if (i < node->data.size() - 1) std::cout << ", ";
        }
        std::cout << ") ";
    }
}

void kDTree::nearestNeighbour(const vector<int>& target, kDTreeNode*& best) {
    nearestNeighbourHelper(root, target, best, 0);
}

void kDTree::nearestNeighbourHelper(kDTreeNode* node, const vector<int>& target, kDTreeNode*& best, int depth) {
    if (node == nullptr) return;

    // Kiểm tra và cập nhật nút gần nhất
    if (best == nullptr || distance(target, node->data) < distance(target, best->data)) {
        best = node;
    }

    // Tính chiều hiện tại để so sánh
    int cd = depth % k;

    // So sánh điểm mục tiêu với node hiện tại để quyết định hướng đi tiếp theo
    if (target[cd] < node->data[cd]) {
        nearestNeighbourHelper(node->left, target, best, depth + 1); // Tìm kiếm bên trái trước
        if (abs(target[cd] - node->data[cd]) < distance(target, best->data)) {
            nearestNeighbourHelper(node->right, target, best, depth + 1); // Sau đó kiểm tra bên phải nếu cần
        }
    }
    else {
        nearestNeighbourHelper(node->right, target, best, depth + 1); // Tìm kiếm bên phải trước
        if (abs(target[cd] - node->data[cd]) < distance(target, best->data)) {
            nearestNeighbourHelper(node->left, target, best, depth + 1); // Sau đó kiểm tra bên trái nếu cần
        }
    }
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

float kDTree::distance(const vector<int>& a, const vector<int>& b) {
    float dist = 0;
    for (size_t i = 0; i < a.size(); i++) {
        dist += (a[i] - b[i]) * (a[i] - b[i]);
    }
    return sqrt(dist);
}

void kDTree::bubbleSort(vector<pair<float, kDTreeNode*>>& nodes) {
    bool swapped;
    for (size_t i = 0; i < nodes.size() - 1; i++) {
        swapped = false;
        for (size_t j = 0; j < nodes.size() - i - 1; j++) {
            if (nodes[j].first > nodes[j + 1].first) {
                swap(nodes[j], nodes[j + 1]);
                swapped = true;
            }
        }
        // If no two elements were swapped by inner loop, then break
        if (!swapped)
            break;
    }
}

void kDTree::kNearestNeighbour(const vector<int>& target, int k, vector<kDTreeNode*>& bestList) {
    vector<pair<float, kDTreeNode*>> bestNodes; // To store nodes along with their distances
    kNearestNeighbourHelper(root, target, k, bestNodes, 0);

    // Sort by distance and pick the top k elements
    bubbleSort(bestNodes);

    // Clear existing bestList and repopulate
    bestList.clear();
    for (int i = 0; i < k && i < bestNodes.size(); i++) {
        bestList.push_back(bestNodes[i].second);
    }
}

vector<pair<float, kDTreeNode*>>::iterator max_element(vector<pair<float, kDTreeNode*>>& elements) {
    if (elements.empty()) {
        return elements.end();
    }

    auto max_it = elements.begin(); // Initialize the iterator to the first element
    for (auto it = elements.begin() + 1; it != elements.end(); ++it) {
        if (it->first > max_it->first) {
            max_it = it; // Update max_it if the current element is greater
        }
    }
    return max_it;
}


// void kDTree::kNearestNeighbourHelper(kDTreeNode* node, const vector<int>& target, int k, vector<pair<float, kDTreeNode*>>& bestNodes, int depth) {
//     if (!node) return;
        
//     float dist = distance(target, node->data);
//     if (bestNodes.size() < k) {
//         bestNodes.emplace_back(dist, node);
//     } else {
//         // Use custom max_element to find the farthest node
//         auto max_it = max_element(bestNodes);
//         if (dist < max_it->first) {
//             *max_it = {dist, node}; // Replace with the closer node
//         }
//     }

//     int cd = depth % k;
//     kDTreeNode* nearer = target[cd] < node->data[cd] ? node->left : node->right;
//     kDTreeNode* further = target[cd] < node->data[cd] ? node->right : node->left;

//     kNearestNeighbourHelper(nearer, target, k, bestNodes, depth + 1);

//     // Check if we need to visit the further node
//     if (further && (bestNodes.size() < k || abs(node->data[cd] - target[cd]) < distance(target, bestNodes.back().second->data))) {
//         kNearestNeighbourHelper(further, target, k, bestNodes, depth + 1);
//     }
// }

vector<pair<float, kDTreeNode*>>::iterator manualLowerBound(vector<pair<float, kDTreeNode*>>& vec, float value) {
    for (auto it = vec.begin(); it != vec.end(); ++it) {
        if (it->first >= value) {
            return it;
        }
    }
    return vec.end();
}

void kDTree::kNearestNeighbourHelper(kDTreeNode* node, const vector<int>& target, int k, vector<pair<float, kDTreeNode*>>& bestNodes, int depth) {
    if (!node) return;

    float dist = distance(target, node->data);

    auto insert_position = manualLowerBound(bestNodes, dist);

    if (bestNodes.size() < k) {
        bestNodes.insert(insert_position, {dist, node});
    } else if (insert_position != bestNodes.end() && dist < bestNodes.back().first) {
        bestNodes.pop_back();
        bestNodes.insert(insert_position, {dist, node});
    }

    int cd = depth % k;
    kDTreeNode* nearer = target[cd] < node->data[cd] ? node->left : node->right;
    kDTreeNode* further = target[cd] < node->data[cd] ? node->right : node->left;

    kNearestNeighbourHelper(nearer, target, k, bestNodes, depth + 1);

    if (further && (bestNodes.size() < k || abs(node->data[cd] - target[cd]) < (bestNodes.size() == k ? bestNodes.back().first : MAX_FLOAT))) {
        kNearestNeighbourHelper(further, target, k, bestNodes, depth + 1);
    }
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

kNN::kNN(int k)
{
    this->k = k;
}

void kDTree::clear() {
    deleteTree(root);  // Helper function to recursively delete all nodes
    root = nullptr;    // Reset the root to signify an empty tree
}

// void kNN::fit(Dataset& X_train, Dataset& y_train) {
//     this->X_train = X_train; 
//     this->y_train = y_train; 

//     tree.clear();  

//     // Iterate over each instance in the training dataset
//     auto it_label = y_train.data.begin();  // Iterator for labels
//     for (auto it = X_train.data.begin(); it != X_train.data.end(); ++it, ++it_label) {
//         if (it_label == y_train.data.end()) {
//             break;
//         }

//         vector<int> target(it->begin(), it->end());
        
//         // Assuming labels are single integers stored in a list of one element
//         int label = it_label->front();  // Get the label from the label dataset
//         target.push_back(label);
//         tree.insert(target);
//     }
// }

void kNN::fit(Dataset& X_train, Dataset& y_train) {
    this->X_train = X_train; 
    this->y_train = y_train;

    tree.clear();  // Ensure the tree is empty before building

    // Collect all data points including labels into a single structure
    vector<vector<int>> points;
    auto it_label = y_train.data.begin();

    for (auto it = X_train.data.begin(); it != X_train.data.end() && it_label != y_train.data.end(); ++it, ++it_label) {
        vector<int> point(it->begin(), it->end());
        int label = it_label->front();  // Assuming labels are stored as single integers
        point.push_back(label);  // Append the label to the end of the features vector
        points.push_back(point);
    }

    // Use buildTree to create a balanced k-D tree
    tree.buildTree(points);
}



// Dataset kNN::predict(Dataset& X_test) {
//     Dataset predictions;
//     predictions.columnName.push_back("label");
//     for (list<list<int>>::iterator it = X_test.data.begin(); it != X_test.data.end(); it++) {
//         vector<int> target(it->begin(), it->end());
//         vector<kDTreeNode*> bestList;
//         tree.kNearestNeighbour(target, k, bestList);
//         int arr[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
//         for (auto it : bestList) {
//             arr[it->label]++;
//         }
//         int max = 0;
//         int label = 0;
//         for (int i = 0; i < 10; i++) {
//             if (arr[i] > max) {
//                 max = arr[i];
//                 label = i;
//             }
//         }
//     }
//     return predictions;
// }

Dataset kNN::predict(Dataset& X_test) {
    Dataset predictions;
    predictions.columnName.push_back("label");

    for (auto it = X_test.data.begin(); it != X_test.data.end(); ++it) {
        vector<int> target(it->begin(), it->end());
        vector<kDTreeNode*> bestList;
        tree.kNearestNeighbour(target, k, bestList);

        vector<int> labelCounts(10, 0);  // Adjust size based on the number of possible labels
        for (auto& node : bestList) {
            if (node) {
                int label = node->data.back();  // Assumes label is the last element in data
                if (label >= 0 && label < labelCounts.size()) {
                    labelCounts[label]++;
                }
            }
        }

        // Manually find the label with the maximum count
        int maxCount = 0, maxLabel = -1;
        for (int i = 0; i < labelCounts.size(); i++) {
            if (labelCounts[i] > maxCount) {
                maxCount = labelCounts[i];
                maxLabel = i;
            }
        }
        predictions.data.push_back({maxLabel});  // Store the predicted label
    }

    return predictions;
}


double kNN::score(const Dataset& y_test, const Dataset& y_pred) {
    double accuracy = 0.0;
    int correctCount = 0;
    int totalCount = 0;

    auto it_pred = y_pred.data.begin();  // Iterator for the predicted labels
    for (auto it_test = y_test.data.begin(); it_test != y_test.data.end(); ++it_test, ++it_pred) {
        if (it_pred == y_pred.data.end()) {
            // If there are no more predictions but still more actual labels, break
            break;
        }
        
        // Assuming labels are stored as the first element in each list
        int actualLabel = it_test->front();   
        int predictedLabel = it_pred->front();

        if (actualLabel == predictedLabel) {
            correctCount++;
        }
        totalCount++;
    }

    // Calculate accuracy if there are any items to count
    if (totalCount > 0) {
        accuracy = static_cast<double>(correctCount) / totalCount;
    }

    return accuracy;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
