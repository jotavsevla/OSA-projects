//
// Created by yango on 16/02/2025.
//

#include "../include/BTree.h"




template<typename T>
BTree<T>::BTree(int order) : root(nullptr), t(order) {
    if (order < 1) throw invalid_argument("Ordem da árvore deve ser maior que 0");
}

template<typename T>
BTree<T>::Node::~Node() {
    if (!leaf) {
        for (auto child : children) {
            delete child;
        }
    }
}

template<typename T>
BTree<T>::~BTree() {
    if (root) delete root;
}

template<typename T>
bool BTree<T>::search(const T& key) const {
    return root && searchKey(root, key);
}

template<typename T>
bool BTree<T>::searchKey(Node* node, const T& key) const {
    int i = 0;
    while (i < node->keys.size() && key > node->keys[i]) {
        i++;
    }

    if (i < node->keys.size() && key == node->keys[i]) {
        return true;
    }

    if (node->leaf) {
        return false;
    }

    return searchKey(node->children[i], key);
}

template<typename T>
void BTree<T>::insert(const T& key) {
    if (root == nullptr) {
        root = new Node(true);
        root->keys.push_back(key);
        return;
    }

    if (root->keys.size() == 2*t) {
        Node* newRoot = new Node(false);
        newRoot->children.push_back(root);
        splitChild(newRoot, 0);
        root = newRoot;
    }

    insertNonFull(root, key);
}

template<typename T>
void BTree<T>::insertNonFull(Node* node, const T& key) {
    int i = node->keys.size() - 1;

    if (node->leaf) {
        node->keys.push_back(T{});
        while (i >= 0 && key < node->keys[i]) {
            node->keys[i + 1] = node->keys[i];
            i--;
        }
        node->keys[i + 1] = key;
    }
    else {
        while (i >= 0 && key < node->keys[i]) {
            i--;
        }
        i++;

        if (node->children[i]->keys.size() == 2*t) {
            splitChild(node, i);
            if (key > node->keys[i]) {
                i++;
            }
        }
        insertNonFull(node->children[i], key);
    }
}

template<typename T>
bool BTree<T>::remove(const T& key) {
    if (!root) return false;

    bool result = removeFromNode(root, key);

    if (root->keys.empty()) {
        Node* oldRoot = root;
        root = root->leaf ? nullptr : root->children[0];
        oldRoot->children.clear();
        delete oldRoot;
    }

    return result;
}

template<typename T>
bool BTree<T>::removeFromNode(Node* node, const T& key) {
    int idx = findKey(node, key);

    if (idx < node->keys.size() && node->keys[idx] == key) {
        if (node->leaf) {
            removeFromLeaf(node, idx);
        }
        else {
            removeFromNonLeaf(node, idx);
        }
        return true;
    }

    if (node->leaf) {
        return false;
    }

    bool lastChild = (idx == node->keys.size());

    if (node->children[idx]->keys.size() < t) {
        fill(node, idx);
    }

    if (lastChild && idx > node->keys.size()) {
        return removeFromNode(node->children[idx-1], key);
    }
    else {
        return removeFromNode(node->children[idx], key);
    }
}

template<typename T>
void BTree<T>::removeFromLeaf(Node* node, int idx) {
    for (int i = idx + 1; i < node->keys.size(); ++i) {
        node->keys[i-1] = node->keys[i];
    }
    node->keys.pop_back();
}

template<typename T>
void BTree<T>::removeFromNonLeaf(Node* node, int idx) {
    T k = node->keys[idx];

    if (node->children[idx]->keys.size() >= t) {
        T pred = getPred(node, idx);
        node->keys[idx] = pred;
        removeFromNode(node->children[idx], pred);
    }
    else if (node->children[idx+1]->keys.size() >= t) {
        T succ = getSucc(node, idx);
        node->keys[idx] = succ;
        removeFromNode(node->children[idx+1], succ);
    }
    else {
        merge(node, idx);
        removeFromNode(node->children[idx], k);
    }
}

template<typename T>
T BTree<T>::getPred(Node* node, int idx) {
    Node* cur = node->children[idx];
    while (!cur->leaf) {
        cur = cur->children[cur->keys.size()];
    }
    return cur->keys[cur->keys.size()-1];
}

template<typename T>
T BTree<T>::getSucc(Node* node, int idx) {
    Node* cur = node->children[idx+1];
    while (!cur->leaf) {
        cur = cur->children[0];
    }
    return cur->keys[0];
}

template<typename T>
void BTree<T>::fill(Node* node, int idx) {
    if (idx != 0 && node->children[idx-1]->keys.size() >= t) {
        borrowFromPrev(node, idx);
    }
    else if (idx != node->keys.size() && node->children[idx+1]->keys.size() >= t) {
        borrowFromNext(node, idx);
    }
    else {
        if (idx != node->keys.size()) {
            merge(node, idx);
        }
        else {
            merge(node, idx-1);
        }
    }
}

template<typename T>
void BTree<T>::borrowFromPrev(Node* node, int idx) {
    Node* child = node->children[idx];
    Node* sibling = node->children[idx-1];

    for (int i = child->keys.size()-1; i >= 0; --i) {
        child->keys[i+1] = child->keys[i];
    }

    if (!child->leaf) {
        for (int i = child->children.size()-1; i >= 0; --i) {
            child->children[i+1] = child->children[i];
        }
    }

    child->keys[0] = node->keys[idx-1];
    node->keys[idx-1] = sibling->keys[sibling->keys.size()-1];

    if (!child->leaf) {
        child->children[0] = sibling->children[sibling->children.size()-1];
        sibling->children.pop_back();
    }

    sibling->keys.pop_back();
}

template<typename T>
void BTree<T>::borrowFromNext(Node* node, int idx) {
    Node* child = node->children[idx];
    Node* sibling = node->children[idx+1];

    child->keys.push_back(node->keys[idx]);
    node->keys[idx] = sibling->keys[0];

    if (!child->leaf) {
        child->children.push_back(sibling->children[0]);
        sibling->children.erase(sibling->children.begin());
    }

    sibling->keys.erase(sibling->keys.begin());
}

template<typename T>
void BTree<T>::merge(Node* node, int idx) {
    Node* child = node->children[idx];
    Node* sibling = node->children[idx+1];

    child->keys.push_back(node->keys[idx]);

    for (int i = 0; i < sibling->keys.size(); ++i) {
        child->keys.push_back(sibling->keys[i]);
    }

    if (!child->leaf) {
        for (int i = 0; i < sibling->children.size(); ++i) {
            child->children.push_back(sibling->children[i]);
        }
    }

    node->keys.erase(node->keys.begin() + idx);
    node->children.erase(node->children.begin() + idx + 1);

    delete sibling;
}

template<typename T>
void BTree<T>::print() const {
    if (!root) {
        cout << "Árvore vazia" << endl;
        return;
    }

    int height = getHeight(root);
    for (int level = 0; level < height; level++) {
        cout << "Nível " << level << ": ";
        printLevel(root, level);
        cout << endl;
    }
}

template<typename T>
void BTree<T>::printLevel(Node* node, int level, int indent) const {
    if (!node) return;

    if (level == 0) {
        cout << string(indent, ' ') << "[ ";
        for (const T& key : node->keys) {
            cout << key << " ";
        }
        cout << "] ";
    }
    else {
        for (Node* child : node->children) {
            printLevel(child, level - 1, indent + 4);
        }
    }
}

template<typename T>
int BTree<T>::getHeight(Node* node) const {
    if (!node) return 0;

    int height = 1;
    if (!node->leaf) {
        height += getHeight(node->children[0]);
    }
    return height;
}

template<typename T>
int BTree<T>::findKey(Node* node, const T& key) const {
    int idx = 0;
    while (idx < node->keys.size() && node->keys[idx] < key) {
        ++idx;
    }
    return idx;
}

// Implementação inicial dos métodos principais - outros métodos serão adicionados conforme necessário