#ifndef AVL_H
#define AVL_H

#include <stdio.h>
#include <stdlib.h>

typedef struct Station {
    int id;
    long long capacity;
    long long consumption;
} Station;

// AVL Tree Node
typedef struct AVLNode {
    Station station;
    struct AVLNode* left;
    struct AVLNode* right;
    int height;
} AVLNode;

AVLNode* insertAVL(AVLNode* node, Station station);
int getHeight(AVLNode* node);
int max(int a, int b);
AVLNode* rightRotate(AVLNode* y);
AVLNode* leftRotate(AVLNode* x);
int getBalance(AVLNode* node);
void inOrderTraversal(AVLNode* root, FILE* fp);
void freeAVL(AVLNode* root);

#endif // AVL_H

