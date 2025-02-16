//
// Created by yango on 16/02/2025.
//

#ifndef BTREE_H
#define BTREE_H


#include <iostream>
#include <vector>
#include <iomanip>
#include <queue>
using namespace std;

template<typename T>
class BTree {
private:
    struct Node {
        vector<T> keys;
        vector<Node*> children;
        bool leaf;

        Node(bool isLeaf = true) : leaf(isLeaf) {}
        ~Node();
    };

    Node* root;
    int t;  // ordem da árvore (mínimo de chaves = t+1)

    // Métodos auxiliares privados
    void splitChild(Node* parent, int index);
    void insertNonFull(Node* node, const T& key);
    bool searchKey(Node* node, const T& key) const;
    T getPred(Node* node, int index);
    T getSucc(Node* node, int index);
    void fill(Node* node, int index);
    void borrowFromPrev(Node* node, int index);
    void borrowFromNext(Node* node, int index);
    void merge(Node* node, int index);
    bool removeFromNode(Node* node, const T& key);
    void removeFromLeaf(Node* node, int index);
    void removeFromNonLeaf(Node* node, int index);
    void printLevel(Node* node, int level, int indent = 0) const;
    int getHeight(Node* node) const;
    void destroyNode(Node* node);
    int findKey(Node* node, const T& key) const;

public:
    BTree(int order);
    ~BTree();

    void insert(const T& key);
    bool remove(const T& key);
    bool search(const T& key) const;
    void print() const;
};

#endif //BTREE_H
