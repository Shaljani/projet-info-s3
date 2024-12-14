#include "avl.h"

// get the height of the tree
int getHeight(AVLNode* node) {
    if (node == NULL)
        return 0;
    return node->height;
}

// get maximum of two integers
int max(int a, int b) {
    return (a > b) ? a : b;
}

// Right rotate subtree rooted with y
AVLNode* rightRotate(AVLNode* y) {
    AVLNode* x = y->left;
    AVLNode* T2 = x->right;

    // Perform rotation
    x->right = y;
    y->left = T2;

    // Update heights
    y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
    x->height = max(getHeight(x->left), getHeight(x->right)) + 1;

    // Return new root
    return x;
}

// Left rotate subtree rooted with x
AVLNode* leftRotate(AVLNode* x) {
    AVLNode* y = x->right;
    AVLNode* T2 = y->left;

    // Perform rotation
    y->left = x;
    x->right = T2;

    // Update heights
    x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
    y->height = max(getHeight(y->left), getHeight(y->right)) + 1;

    // Return new root
    return y;
}

// Get balance factor of node
int getBalance(AVLNode* node) {
    if (node == NULL)
        return 0;
    return getHeight(node->left) - getHeight(node->right);
}

// Create a new AVL node
AVLNode* createNode(Station station) {
    AVLNode* node = (AVLNode*)malloc(sizeof(AVLNode));
    if (!node) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }
    node->station = station;
    node->left = NULL;
    node->right = NULL;
    node->height = 1; // New node is initially at leaf
    return node;
}

// Insert a station into the AVL tree
AVLNode* insertAVL(AVLNode* node, Station station) {
    // Perform the normal BST insertion
    if (node == NULL)
        return createNode(station);

    if (station.id < node->station.id)
        node->left = insertAVL(node->left, station);
    else if (station.id > node->station.id)
        node->right = insertAVL(node->right, station);
    else {
 		if(station.capacity)
 			node->station.capacity += station.capacity;
 		 if(station.consumption)
 			node->station.consumption += station.consumption;
			
        return node;
    }

    // Update height of this ancestor node
    node->height = 1 + max(getHeight(node->left), getHeight(node->right));

    // check whether this node became unbalanced
    int balance = getBalance(node);

    // node is unbalanced: 4 cases

    // Left Left Case
    if (balance > 1 && station.id < node->left->station.id)
        return rightRotate(node);

    // Right Right Case
    if (balance < -1 && station.id > node->right->station.id)
        return leftRotate(node);

    // Left Right Case
    if (balance > 1 && station.id > node->left->station.id) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }

    // Right Left Case
    if (balance < -1 && station.id < node->right->station.id) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}

// In-order traversal to write data to file
void inOrderTraversal(AVLNode* root, FILE* fp) {
    if (root != NULL) {
        inOrderTraversal(root->left, fp);
        fprintf(fp, "%d:%lld:%lld\n", root->station.id, root->station.capacity, root->station.consumption);
        inOrderTraversal(root->right, fp);
    }
}

// Free the AVL tree
void freeAVL(AVLNode* root) {
    if (root != NULL) {
        freeAVL(root->left);
        freeAVL(root->right);
        free(root);
    }
}



