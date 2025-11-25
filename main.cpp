#include <iostream>
#include <set>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>
using namespace std;

struct Node {
    int value;
    Node* left;
    Node* right;

    Node(int v) : value(v), left(nullptr), right(nullptr) {}
};

// Arbre binaire de recherche non equilibré

class BST {
private:
    Node* root = nullptr;

    Node* insertRec(Node* node, int value) {
        if (!node) return new Node(value);
        if (value < node->value) node->left = insertRec(node->left, value);
        else if (value > node->value) node->right = insertRec(node->right, value);
        return node; 
    }

    bool findRec(Node* node, int value) {
        if (!node) return false;
        if (value == node->value) return true;
        return value < node->value
            ? findRec(node->left, value)
            : findRec(node->right, value);
    }

    Node* eraseRec(Node* node, int value) {
        if (!node) return nullptr;

        if (value < node->value) {
            node->left = eraseRec(node->left, value);
        } else if (value > node->value) {
            node->right = eraseRec(node->right, value);
        } else {
            if (!node->left) {
                Node* r = node->right;
                delete node;
                return r;
            } else if (!node->right) {
                Node* l = node->left;
                delete node;
                return l;
            }
            Node* temp = node->right;
            while (temp->left) temp = temp->left;
            node->value = temp->value;
            node->right = eraseRec(node->right, temp->value);
        }
        return node;
    }

public:
    void insert(int value) { root = insertRec(root, value); }
    bool find(int value) { return findRec(root, value); }
    void erase(int value) { root = eraseRec(root, value); }
};

// Wrapper pour std::set arbre rouge-noir
class RBTreeWrapper {
    set<int> s;
public:
    void insert(int value) { s.insert(value); }
    bool find(int value) { return s.count(value); }
    void erase(int value) { s.erase(value); }
};

template<typename Tree>
void benchmarkTree(const string& name, Tree& tree, const vector<int>& values) {

    cout << "= " << name << " =\n";

    auto t1 = chrono::high_resolution_clock::now();
    for (int x : values) tree.insert(x);
    auto t2 = chrono::high_resolution_clock::now();
    cout << "Insertion : " << chrono::duration<double>(t2 - t1).count() << "s\n";

    t1 = chrono::high_resolution_clock::now();
    for (int i = 0; i < 1000; i++)
        tree.find(values[rand() % values.size()]);
    t2 = chrono::high_resolution_clock::now();
    cout << "Recherche  : " << chrono::duration<double>(t2 - t1).count() << "s\n";

    t1 = chrono::high_resolution_clock::now();
    for (int i = 0; i < 5000; i++)
        tree.erase(values[i]);
    t2 = chrono::high_resolution_clock::now();
    cout << "Suppression  : " << chrono::duration<double>(t2 - t1).count() << "s\n";

    cout << "\n";
}

int main() {
    const int SIZE = 1000000;

    vector<int> values(SIZE);
    for (int i = 0; i < SIZE; i++) values[i] = i;

    shuffle(values.begin(), values.end(), mt19937(random_device{}()));

    BST bst;
    RBTreeWrapper rbt;

    benchmarkTree("BTS non equilibre", bst, values);
    benchmarkTree("ARBRE rouge-noir", rbt, values);

    return 0;
}
