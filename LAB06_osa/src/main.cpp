//
// Created by yango on 16/02/2025.
//
// src/main.cpp
#include <iostream>
#include "../include/BTree.h"

int main() {
    try {
        // Cria uma árvore B de ordem 2 (mínimo 3 chaves por nó)
        BTree<int> tree(2);

        // Insere alguns valores
        cout << "Inserindo valores..." << endl;
        int valores[] = {3, 7, 1, 5, 11, 17, 13, 19, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20};
        for (int valor : valores) {
            tree.insert(valor);
            cout << "\nApós inserir " << valor << ":" << endl;
            tree.print();
        }

        // Testa a busca
        cout << "\nTestando busca:" << endl;
        cout << "Busca 11: " << (tree.search(11) ? "Encontrado" : "Não encontrado") << endl;
        cout << "Busca 15: " << (tree.search(15) ? "Encontrado" : "Não encontrado") << endl;

        // Testa a remoção
        cout << "\nTestando remoção:" << endl;
        cout << "Removendo 11..." << endl;
        tree.remove(11);
        tree.print();

        cout << "\nRemovendo 3..." << endl;
        tree.remove(3);
        tree.print();

        return 0;
    }
    catch (const exception& e) {
        cerr << "Erro: " << e.what() << endl;
        return 1;
    }
}