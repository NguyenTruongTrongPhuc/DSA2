#include "main.hpp"
#include "Dataset.hpp"
/* TODO: Please design your data structure carefully so that you can work with the given dataset
 *       in this assignment. The below structures are just some suggestions.
 */
struct kDTreeNode
{
    vector<int> data;
    kDTreeNode *left;
    kDTreeNode *right;
    int label;
    kDTreeNode(vector<int> data, kDTreeNode *left = nullptr, kDTreeNode *right = nullptr)
    {
        this->data = data;
        this->left = left;
        this->right = right;
    }

    friend ostream &operator<<(ostream &os, const kDTreeNode &node)
    {
        os << "(";
        for (int i = 0; i < node.data.size(); i++)
        {
            os << node.data[i];
            if (i != node.data.size() - 1)
            {
                os << ", ";
            }
        }
        os << ")";
        return os;
    }
};

class kDTree
{
private:
    int k;
    kDTreeNode *root;

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    kDTreeNode* minNode(kDTreeNode* x, kDTreeNode* y, kDTreeNode* z, int d);
    kDTreeNode* findMin(kDTreeNode* node, int d, int depth);
    kDTreeNode* copyNodes(kDTreeNode* node);
    kDTreeNode* insertRecursive(kDTreeNode* node, const std::vector<int>& point, int depth);
    kDTreeNode* removeRecursive(kDTreeNode* node, const std::vector<int>& point, int depth);
    bool searchRecursive(kDTreeNode* node, const std::vector<int>& point, int depth) const;
    void inorderRecursive(kDTreeNode* node) const;
    void preorderRecursive(kDTreeNode* node) const;
    void postorderRecursive(kDTreeNode* node) const;
    int heightRecursive(kDTreeNode* node) const;
    int nodeCountRecursive(kDTreeNode* node) const;
    int leafCountRecursive(kDTreeNode* node) const;
    void deleteTree(kDTreeNode* node);
    void printNode(kDTreeNode* node) const;
    int partitionSort(vector<vector<int>>& points, int start, int end, int cd, int mid);
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

public:
    kDTree(int k = 2);
    ~kDTree();

    kDTree &operator=(const kDTree &other);
    kDTree(const kDTree &other);

    void inorderTraversal() const;
    void preorderTraversal() const;
    void postorderTraversal() const;
    int height() const;
    int nodeCount() const;
    int leafCount() const;

    void insert(const vector<int> &point);
    void remove(const vector<int> &point);
    bool search(const vector<int> &point) const;
    void buildTree(const vector<vector<int>> &pointList);
    void nearestNeighbour(const vector<int> &target, kDTreeNode *&best);
    void kNearestNeighbour(const vector<int> &target, int k, vector<kDTreeNode *> &bestList);

    kDTreeNode* findNodeWithData(const vector<int>& data);
    kDTreeNode* findNodeWithDataRecursive(kDTreeNode* node, const vector<int>& data);

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    kDTreeNode* buildTreeHelper(vector<vector<int>>& points, int depth);
    void mergeSort(vector<vector<int>>& points, int l, int r, int cd);
    void merge(vector<vector<int>>& points, int l, int m, int r, int cd);

    void nearestNeighbourHelper(kDTreeNode* node, const vector<int>& target, kDTreeNode*& best, int depth);
    float distance(const vector<int>& a, const vector<int>& b);

    void bubbleSort(vector<pair<float, kDTreeNode*>>& nodes);
    void kNearestNeighbourHelper(kDTreeNode* node, const vector<int>& target, int k, vector<pair<float, kDTreeNode*>>& bestNodes, int depth);

    void clear();
};

class kNN
{
private:
    int k;
    kDTree tree;
    Dataset X_train;
    Dataset y_train;
public:
    kNN(int k = 5);
    void fit(Dataset &X_train, Dataset &y_train);
    Dataset predict(Dataset &X_test);
    double score(const Dataset &y_test, const Dataset &y_pred);
};

// Please add more or modify as needed
