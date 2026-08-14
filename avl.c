/* ============================================================
   avl.c
   AVL Tree (self-balancing BST) used to store and manage all
   grocery Products, keyed by unique Product ID.
   Implements: insert, search, delete, update, inorder/preorder/
   postorder traversal, height, balance factor and all 4 rotations.
   Search / Insert / Delete are all O(log n).
   ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ---- Product record stored in every AVL node ---- */
typedef struct Product {
    char id[20];
    char name[50];
    char category[30];
    float buyPrice;
    float sellPrice;
    int quantity;
    int minStock;
    char expiryDate[12];   /* DD-MM-YYYY */
    char supplierId[20];
    int height;
    struct Product *left;
    struct Product *right;
} Product;

/* Root of the AVL tree - the single inventory index for the shop */
Product *root = NULL;

/* ---- prototypes ---- */
int heightOf(Product *n);
int maxInt(int a, int b);
int getBalance(Product *n);
Product* rightRotate(Product *y);
Product* leftRotate(Product *x);
Product* insertProduct(Product *node, Product *newNode);
Product* minValueNode(Product *node);
Product* deleteProduct(Product *node, const char *id, int *found, Product *backup);
Product* searchProductById(Product *node, const char *id);
void inorderVisit(Product *node, void (*visit)(Product *p, void *ctx), void *ctx);
void preorderVisit(Product *node, void (*visit)(Product *p, void *ctx), void *ctx);
void postorderVisit(Product *node, void (*visit)(Product *p, void *ctx), void *ctx);
int countNodes(Product *node);
Product* insertProductRecord(Product data);
int deleteProductById(const char *id, Product *backup);
Product* findProduct(const char *id);
int updateProductData(const char *id, Product newData, Product *oldBackup);
void copyProductFields(Product *dest, const Product *src);
void freeTree(Product *node);

int heightOf(Product *n) {
    if (n == NULL) return 0;
    return n->height;
}

int maxInt(int a, int b) {
    return (a > b) ? a : b;
}

int getBalance(Product *n) {
    if (n == NULL) return 0;
    return heightOf(n->left) - heightOf(n->right);
}

/* Standard right rotation used when the tree is left-heavy */
Product* rightRotate(Product *y) {
    Product *x = y->left;
    Product *T2 = x->right;

    x->right = y;
    y->left = T2;

    y->height = maxInt(heightOf(y->left), heightOf(y->right)) + 1;
    x->height = maxInt(heightOf(x->left), heightOf(x->right)) + 1;

    return x;
}

/* Standard left rotation used when the tree is right-heavy */
Product* leftRotate(Product *x) {
    Product *y = x->right;
    Product *T2 = y->left;

    y->left = x;
    x->right = T2;

    x->height = maxInt(heightOf(x->left), heightOf(x->right)) + 1;
    y->height = maxInt(heightOf(y->left), heightOf(y->right)) + 1;

    return y;
}

/* Recursive AVL insert; rebalances using LL, RR, LR, RL cases */
Product* insertProduct(Product *node, Product *newNode) {
    if (node == NULL) return newNode;

    int cmp = strcmp(newNode->id, node->id);
    if (cmp < 0)
        node->left = insertProduct(node->left, newNode);
    else if (cmp > 0)
        node->right = insertProduct(node->right, newNode);
    else
        return node; /* duplicate id, should be prevented by caller */

    node->height = 1 + maxInt(heightOf(node->left), heightOf(node->right));
    int balance = getBalance(node);

    /* Left Left */
    if (balance > 1 && strcmp(newNode->id, node->left->id) < 0)
        return rightRotate(node);
    /* Right Right */
    if (balance < -1 && strcmp(newNode->id, node->right->id) > 0)
        return leftRotate(node);
    /* Left Right */
    if (balance > 1 && strcmp(newNode->id, node->left->id) > 0) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }
    /* Right Left */
    if (balance < -1 && strcmp(newNode->id, node->right->id) < 0) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}

Product* minValueNode(Product *node) {
    Product *current = node;
    while (current->left != NULL)
        current = current->left;
    return current;
}

/* Recursive AVL delete. If backup != NULL and the id is found, the
   product's data is copied into *backup before it is freed, so the
   caller can push it onto the Undo stack. */
Product* deleteProduct(Product *node, const char *id, int *found, Product *backup) {
    if (node == NULL) return NULL;

    int cmp = strcmp(id, node->id);
    if (cmp < 0) {
        node->left = deleteProduct(node->left, id, found, backup);
    } else if (cmp > 0) {
        node->right = deleteProduct(node->right, id, found, backup);
    } else {
        *found = 1;
        if (backup != NULL) copyProductFields(backup, node);

        if (node->left == NULL || node->right == NULL) {
            Product *temp = node->left ? node->left : node->right;
            if (temp == NULL) {
                temp = node;
                node = NULL;
            } else {
                *node = *temp; /* copy child into this node (keeps subtree pointers) */
            }
            free(temp);
        } else {
            Product *temp = minValueNode(node->right);
            copyProductFields(node, temp);
            node->right = deleteProduct(node->right, temp->id, found, NULL);
        }
    }

    if (node == NULL) return NULL;

    node->height = 1 + maxInt(heightOf(node->left), heightOf(node->right));
    int balance = getBalance(node);

    if (balance > 1 && getBalance(node->left) >= 0)
        return rightRotate(node);
    if (balance > 1 && getBalance(node->left) < 0) {
        node->left = leftRotate(node->left);
        return rightRotate(node);
    }
    if (balance < -1 && getBalance(node->right) <= 0)
        return leftRotate(node);
    if (balance < -1 && getBalance(node->right) > 0) {
        node->right = rightRotate(node->right);
        return leftRotate(node);
    }

    return node;
}

Product* searchProductById(Product *node, const char *id) {
    if (node == NULL) return NULL;
    int cmp = strcmp(id, node->id);
    if (cmp == 0) return node;
    if (cmp < 0) return searchProductById(node->left, id);
    return searchProductById(node->right, id);
}

void inorderVisit(Product *node, void (*visit)(Product *p, void *ctx), void *ctx) {
    if (node == NULL) return;
    inorderVisit(node->left, visit, ctx);
    visit(node, ctx);
    inorderVisit(node->right, visit, ctx);
}

void preorderVisit(Product *node, void (*visit)(Product *p, void *ctx), void *ctx) {
    if (node == NULL) return;
    visit(node, ctx);
    preorderVisit(node->left, visit, ctx);
    preorderVisit(node->right, visit, ctx);
}

void postorderVisit(Product *node, void (*visit)(Product *p, void *ctx), void *ctx) {
    if (node == NULL) return;
    postorderVisit(node->left, visit, ctx);
    postorderVisit(node->right, visit, ctx);
    visit(node, ctx);
}

int countNodes(Product *node) {
    if (node == NULL) return 0;
    return 1 + countNodes(node->left) + countNodes(node->right);
}

void copyProductFields(Product *dest, const Product *src) {
    strcpy(dest->id, src->id);
    strcpy(dest->name, src->name);
    strcpy(dest->category, src->category);
    dest->buyPrice = src->buyPrice;
    dest->sellPrice = src->sellPrice;
    dest->quantity = src->quantity;
    dest->minStock = src->minStock;
    strcpy(dest->expiryDate, src->expiryDate);
    strcpy(dest->supplierId, src->supplierId);
}

/* Creates a brand new node from plain data and inserts it into the tree. */
Product* insertProductRecord(Product data) {
    Product *node = (Product*)malloc(sizeof(Product));
    if (node == NULL) {
        printf("Memory allocation failed!\n");
        exit(1);
    }
    copyProductFields(node, &data);
    node->height = 1;
    node->left = NULL;
    node->right = NULL;
    root = insertProduct(root, node);
    return node;
}

int deleteProductById(const char *id, Product *backup) {
    int found = 0;
    root = deleteProduct(root, id, &found, backup);
    return found;
}

Product* findProduct(const char *id) {
    return searchProductById(root, id);
}

/* Overwrites the data fields of an existing node (used by Update Product).
   oldBackup receives the pre-update values for Undo support. */
int updateProductData(const char *id, Product newData, Product *oldBackup) {
    Product *node = findProduct(id);
    if (node == NULL) return 0;
    if (oldBackup != NULL) copyProductFields(oldBackup, node);
    node->buyPrice = newData.buyPrice;
    node->sellPrice = newData.sellPrice;
    node->quantity = newData.quantity;
    node->minStock = newData.minStock;
    strcpy(node->name, newData.name);
    strcpy(node->category, newData.category);
    strcpy(node->expiryDate, newData.expiryDate);
    strcpy(node->supplierId, newData.supplierId);
    return 1;
}

void freeTree(Product *node) {
    if (node == NULL) return;
    freeTree(node->left);
    freeTree(node->right);
    free(node);
}
