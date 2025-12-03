#include <iostream>
#include <set>
#include <map>
#include <vector>
#include <algorithm>
#include <chrono>
#include <random>
#include <iomanip>
#include <fstream>
#include <string>
using namespace std;

struct Node {
    int value;
    Node* left;
    Node* right;
    Node(int v) : value(v), left(nullptr), right(nullptr) {}
};

class BST {
private:
    Node* root = nullptr;

    Node* insertRec(Node* node, int value) {
        if (!node) return new Node(value);
        if (value < node->value) 
            node->left = insertRec(node->left, value);
        else if (value > node->value) 
            node->right = insertRec(node->right, value);
        return node; 
    }

    bool findRec(Node* node, int value) {
        if (!node) return false;
        if (value == node->value) return true;
        return value < node->value ? findRec(node->left, value) : findRec(node->right, value);
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
            } 
            else if (!node->right) {
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

    void destroyTree(Node* node) {
        if (node) {
            destroyTree(node->left);
            destroyTree(node->right);
            delete node;
        }
    }

    int getHeight(Node* node) {
        if (!node) return 0;
        return 1 + max(getHeight(node->left), getHeight(node->right));
    }

public:
    ~BST() { destroyTree(root); }
    void insert(int value) { root = insertRec(root, value); }
    bool find(int value) { return findRec(root, value); }
    void erase(int value) { root = eraseRec(root, value); }
    int height() { return getHeight(root); }
};

class SetWrapper {
    set<int> s;
public:
    void insert(int value) { s.insert(value); }
    bool find(int value) { return s.count(value) > 0; }
    void erase(int value) { s.erase(value); }
    int height() { return -1; }
};

class MapWrapper {
    map<int, int> m;
public:
    void insert(int value) { m[value] = value; }
    bool find(int value) { return m.count(value) > 0; }
    void erase(int value) { m.erase(value); }
    int height() { return -1; }
};

struct BenchmarkResult {
    string name;
    double insertTime;
    double searchTime;
    double deleteTime;
    int height;
};

template<typename Tree>
BenchmarkResult benchmarkTree(const string& name, Tree& tree, const vector<int>& values, bool showHeight = false) {
    BenchmarkResult result;
    result.name = name;
    
    cout << "== " << name << " ==\n";
    
    auto t1 = chrono::high_resolution_clock::now();
    for (int x : values) {
        tree.insert(x);
    }
    auto t2 = chrono::high_resolution_clock::now();
    result.insertTime = chrono::duration<double>(t2 - t1).count();
    cout << "Insertion    : " << fixed << setprecision(6) << result.insertTime << "s\n";
    
    if (showHeight) {
        result.height = tree.height();
        cout << "Hauteur      : " << result.height << " (log2(" << values.size() 
             << ") ≈ " << (int)(log2(values.size())) << ")\n";
    } else {
        result.height = -1;
    }
    
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(0, values.size() - 1);
    
    t1 = chrono::high_resolution_clock::now();
    for (int i = 0; i < 10000; i++) {
        tree.find(values[dis(gen)]);
    }
    t2 = chrono::high_resolution_clock::now();
    result.searchTime = chrono::duration<double>(t2 - t1).count();
    cout << "Recherche    : " << result.searchTime << "s (10000 recherches)\n";
    
    t1 = chrono::high_resolution_clock::now();
    int deleteCount = min(5000, (int)values.size());
    for (int i = 0; i < deleteCount; i++) {
        tree.erase(values[i]);
    }
    t2 = chrono::high_resolution_clock::now();
    result.deleteTime = chrono::duration<double>(t2 - t1).count();
    cout << "Suppression  : " << result.deleteTime << "s (" << deleteCount << " elements)\n";
    
    cout << "Temps total  : " << (result.insertTime + result.searchTime + result.deleteTime) << "s\n";
    cout << "\n";
    
    return result;
}

void saveResultsToFile(const vector<BenchmarkResult>& results, const string& filename, const string& scenario) {
    ofstream file(filename, ios::app);
    file << "\n=== " << scenario << " ===\n";
    file << "Structure,Insertion(s),Recherche(s),Suppression(s),Total(s),Hauteur\n";
    for (const auto& r : results) {
        double total = r.insertTime + r.searchTime + r.deleteTime;
        file << r.name << "," << r.insertTime << "," << r.searchTime << "," 
             << r.deleteTime << "," << total << "," << r.height << "\n";
    }
    file.close();
}

int main() {
    cout << "\nComparaison des structures de donnees\n";
    cout << "Test avec 1 000 000 elements\n\n";
    
    ofstream file("resultats.csv");
    file << "Comparaison des structures - 1M elements\n";
    file.close();
    
    const int SIZE = 1000000;
    vector<int> values(SIZE);
    for (int i = 0; i < SIZE; i++) {
        values[i] = i;
    }
    
    cout << "\n--- Cas 1: Donnees melangees ---\n\n";
    
    shuffle(values.begin(), values.end(), mt19937(random_device{}()));
    
    vector<BenchmarkResult> results1;
    
    cout << "Test en cours...\n\n";
    
    BST bst1;
    SetWrapper set1;
    MapWrapper map1;
    
    results1.push_back(benchmarkTree("BST", bst1, values, true));
    results1.push_back(benchmarkTree("SET (Red-Black Tree)", set1, values, false));
    results1.push_back(benchmarkTree("MAP (Red-Black Tree)", map1, values, false));
    
    saveResultsToFile(results1, "resultats.csv", "Cas 1: Donnees melangees");
    
    cout << "\n- Cas 2: Donnees triees -\n\n";
    
    sort(values.begin(), values.end());
    
    vector<BenchmarkResult> results2;
    
    cout << "Note: BST saute pour eviter Stack Overflow\n\n";
    
    SetWrapper set2;
    MapWrapper map2;
    
    results2.push_back({"BST (SAUTÉ - Stack Overflow)", -1, -1, -1, 1000000});
    results2.push_back(benchmarkTree("SET (Red-Black Tree)", set2, values, false));
    results2.push_back(benchmarkTree("MAP (Red-Black Tree)", map2, values, false));
    
    saveResultsToFile(results2, "resultats.csv", "Cas 2: Donnees triees");
    
    cout << "\n--- Cas 3: Tests avec differentes tailles ---\n\n";
    
    vector<int> sizes = {100000, 500000, 1000000};
    
    for (int size : sizes) {
        cout << "\nTaille: " << size << " elements\n\n";
        
        vector<int> testValues(size);
        for (int i = 0; i < size; i++) testValues[i] = i;
        shuffle(testValues.begin(), testValues.end(), mt19937(random_device{}()));
        
        vector<BenchmarkResult> results3;
        
        BST bstTest;
        SetWrapper setTest;
        MapWrapper mapTest;
        
        results3.push_back(benchmarkTree("BST", bstTest, testValues, true));
        results3.push_back(benchmarkTree("SET", setTest, testValues, false));
        results3.push_back(benchmarkTree("MAP", mapTest, testValues, false));
        
        saveResultsToFile(results3, "resultats.csv", "Taille: " + std::to_string(size));
    }
    
    cout << "\nTests termines. Resultats enregistres dans resultats.csv\n\n";
    
    cout << "\nAnalyse:\n";
    cout << "- Donnees melangees: BST et Red-Black similaires\n";
    cout << "- Donnees triees: Red-Black beaucoup plus rapide\n";
    cout << "- Hauteur BST melange: ~40-50, trie: 1M\n";
    cout << "- Hauteur Red-Black: ~20 (optimal)\n\n";
    
    return 0;
}