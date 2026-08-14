/*
    avl.c

    This file manages the shop's products using an AVL Tree.

    AVL Tree is a self-balancing Binary Search Tree.

    Product ID is used as the key.

    Features:
    1. Insert Product
    2. Search Product
    3. Delete Product
    4. Update Product
    5. Inorder Traversal
    6. Preorder Traversal
    7. Postorder Traversal
    8. Height
    9. Balance Factor
    10. LL, RR, LR and RL Rotations

    Search, Insert and Delete:
    O(log n) in an AVL Tree.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/* =========================================================
   PRODUCT STRUCTURE
   ========================================================= */

typedef struct Product
{
    char id[20];
    char name[50];
    char category[30];

    float buyPrice;
    float sellPrice;

    int quantity;
    int minStock;

    char expiryDate[12];

    char supplierId[20];

    int height;

    struct Product *left;
    struct Product *right;

} Product;


/* Root of the AVL Tree */

Product *root = NULL;


/* =========================================================
   FUNCTION PROTOTYPES
   ========================================================= */

/* Basic AVL functions */

int heightOf(Product *node);

int maxInt(int a, int b);

int getBalance(Product *node);


/* Rotations */

Product *rightRotate(Product *node);

Product *leftRotate(Product *node);


/* Insert and Delete */

Product *insertProduct(
    Product *node,
    Product *newNode
);

Product *deleteProduct(
    Product *node,
    const char *id,
    int *found,
    Product *backup
);


/* Search */

Product *searchProductById(
    Product *node,
    const char *id
);


/* Traversal */

void inorderVisit(
    Product *node,
    void (*visit)(Product *product, void *data),
    void *data
);

void preorderVisit(
    Product *node,
    void (*visit)(Product *product, void *data),
    void *data
);

void postorderVisit(
    Product *node,
    void (*visit)(Product *product, void *data),
    void *data
);


/* Other functions */

int countNodes(Product *node);

Product *insertProductRecord(Product data);

int deleteProductById(
    const char *id,
    Product *backup
);

Product *findProduct(const char *id);

int updateProductData(
    const char *id,
    Product newData,
    Product *oldBackup
);

void copyProductFields(
    Product *destination,
    const Product *source
);

Product *minValueNode(Product *node);

void freeTree(Product *node);


/* =========================================================
   GET HEIGHT
   ========================================================= */

int heightOf(Product *node)
{
    if (node == NULL)
    {
        return 0;
    }

    return node->height;
}


/* =========================================================
   FIND MAXIMUM NUMBER
   ========================================================= */

int maxInt(int a, int b)
{
    if (a > b)
    {
        return a;
    }

    return b;
}


/* =========================================================
   GET BALANCE FACTOR
   =========================================================

   Balance Factor =
   Height of Left Subtree - Height of Right Subtree

   Possible values:

       -1  Balanced
        0  Balanced
        1  Balanced

   If:
       > 1  -> Left Heavy
       < -1 -> Right Heavy
   ========================================================= */

int getBalance(Product *node)
{
    if (node == NULL)
    {
        return 0;
    }

    return heightOf(node->left)
         - heightOf(node->right);
}

Product *rightRotate(Product *y)
{
    Product *x;
    Product *middle;
    /* x becomes the new root */
    x = y->left;
    /* Save x's right subtree */

    middle = x->right;
    /* Perform rotation */

    x->right = y;

    y->left = middle;
    /* Update heights */

    y->height =
        maxInt(
            heightOf(y->left),
            heightOf(y->right)
        ) + 1;


    x->height =
        maxInt(
            heightOf(x->left),
            heightOf(x->right)
        ) + 1;


    return x;
}
Product *leftRotate(Product *x)
{
    Product *y;
    Product *middle;
    /* y becomes the new root */

    y = x->right;
    /* Save y's left subtree */

    middle = y->left;
    /* Perform rotation */

    y->left = x;

    x->right = middle;
    /* Update heights */

    x->height =
        maxInt(
            heightOf(x->left),
            heightOf(x->right)
        ) + 1;
    y->height =
        maxInt(
            heightOf(y->left),
            heightOf(y->right)
        ) + 1;


    return y;
}
/* =========================================================
   INSERT PRODUCT INTO AVL TREE
   ========================================================= */

Product *insertProduct(
    Product *node,
    Product *newNode
)
{
    int comparison;
    int balance;
    /* If tree is empty */
    if (node == NULL)
    {
        return newNode;
    }
    /*
        Compare Product IDs.

        Smaller ID -> Left
        Larger ID  -> Right
    */

    comparison =
        strcmp(
            newNode->id,
            node->id
        );


    if (comparison < 0)
    {
        node->left =
            insertProduct(
                node->left,
                newNode
            );
    }

    else if (comparison > 0)
    {
        node->right =
            insertProduct(
                node->right,
                newNode
            );
    }

    else
    {
        /*
            Product ID already exists.
        */

        return node;
    }
    /* Update node height */

    node->height =
        1 + maxInt(
            heightOf(node->left),
            heightOf(node->right)
        );


    /* Get balance factor */

    balance = getBalance(node);
    if (
        balance > 1 &&
        strcmp(
            newNode->id,
            node->left->id
        ) < 0
    )
    {
        return rightRotate(node);
    }

    if (
        balance < -1 &&
        strcmp(
            newNode->id,
            node->right->id
        ) > 0
    )
    {
        return leftRotate(node);
    }

    if (
        balance > 1 &&
        strcmp(
            newNode->id,
            node->left->id
        ) > 0
    )
    {
        node->left =
            leftRotate(node->left);

        return rightRotate(node);
    }

    if (
        balance < -1 &&
        strcmp(
            newNode->id,
            node->right->id
        ) < 0
    )
    {
        node->right =
            rightRotate(node->right);

        return leftRotate(node);
    }
    /* Tree is already balanced */

    return node;
}
/* =========================================================
   FIND SMALLEST NODE
   ========================================================= */

Product *minValueNode(Product *node)
{
    Product *current;

    current = node;
    /*
        The smallest value is always
        on the left side.
    */

    while (current->left != NULL)
    {
        current = current->left;
    }
    return current;
}
/* =========================================================
   DELETE PRODUCT
   ========================================================= */

Product *deleteProduct(
    Product *node,
    const char *id,
    int *found,
    Product *backup
)
{
    int comparison;
    int balance;

    Product *temporary;
    /* Product not found */

    if (node == NULL)
    {
        return NULL;
    }
    /* Compare Product ID */

    comparison =
        strcmp(id, node->id);


    /* Search in left subtree */

    if (comparison < 0)
    {
        node->left =
            deleteProduct(
                node->left,
                id,
                found,
                backup
            );
    }
    /* Search in right subtree */

    else if (comparison > 0)
    {
        node->right =
            deleteProduct(
                node->right,
                id,
                found,
                backup
            );
    }


    /* Product found */

    else
    {
        *found = 1;


        /* Save old data for Undo */

        if (backup != NULL)
        {
            copyProductFields(
                backup,
                node
            );
        }


        /* =================================================
           CASE 1 AND CASE 2:
           Node has zero or one child
           ================================================= */

        if (
            node->left == NULL ||
            node->right == NULL
        )
        {
            temporary =
                node->left != NULL
                ? node->left
                : node->right;


            /* No child */

            if (temporary == NULL)
            {
                temporary = node;

                node = NULL;
            }


            /* One child */

            else
            {
                /*
                    Copy child data into
                    current node.
                */

                *node = *temporary;
            }
            free(temporary);
        }
        /* =================================================
           CASE 3:
           Node has two children
           ================================================= */
        else
        {
            /*
                Find the smallest node
                from the right subtree.
            */

            temporary =
                minValueNode(
                    node->right
                );


            /* Copy successor data */

            copyProductFields(
                node,
                temporary
            );


            /*
                Delete the duplicate
                successor node.
            */

            node->right =
                deleteProduct(
                    node->right,
                    temporary->id,
                    found,
                    NULL
                );
        }
    }
    /* Tree became empty */

    if (node == NULL)
    {
        return NULL;
    }
    /* Update height */

    node->height =
        1 + maxInt(
            heightOf(node->left),
            heightOf(node->right)
        );
    /* Get balance factor */

    balance = getBalance(node);
    /* =====================================================
       REBALANCE AFTER DELETE
       ===================================================== */
    /* Left Left */

    if (
        balance > 1 &&
        getBalance(node->left) >= 0
    )
    {
        return rightRotate(node);
    }
    /* Left Right */

    if (
        balance > 1 &&
        getBalance(node->left) < 0
    )
    {
        node->left =
            leftRotate(node->left);

        return rightRotate(node);
    }
    /* Right Right */

    if (
        balance < -1 &&
        getBalance(node->right) <= 0
    )
    {
        return leftRotate(node);
    }


    /* Right Left */

    if (
        balance < -1 &&
        getBalance(node->right) > 0
    )
    {
        node->right =
            rightRotate(node->right);

        return leftRotate(node);
    }

    return node;
}
/* =========================================================
   SEARCH PRODUCT
   ========================================================= */

Product *searchProductById(
    Product *node,
    const char *id
)
{
    int comparison;


    /* Product not found */

    if (node == NULL)
    {
        return NULL;
    }


    /* Compare IDs */

    comparison =
        strcmp(id, node->id);


    /* Product found */

    if (comparison == 0)
    {
        return node;
    }


    /* Search left */

    if (comparison < 0)
    {
        return searchProductById(
            node->left,
            id
        );
    }

    /* Search right */

    return searchProductById(
        node->right,
        id
    );
}

/* =========================================================
   INORDER TRAVERSAL
   =========================================================

   Order:

       Left -> Root -> Right

   In a BST/AVL tree this displays
   Product IDs in sorted order.
   ========================================================= */

void inorderVisit(
    Product *node,
    void (*visit)(Product *product, void *data),
    void *data
)
{
    if (node == NULL)
    {
        return;
    }

    inorderVisit(
        node->left,
        visit,
        data
    );


    visit(node, data);


    inorderVisit(
        node->right,
        visit,
        data
    );
}
/* =========================================================
   PREORDER TRAVERSAL
   =========================================================

   Order:

       Root -> Left -> Right
   ========================================================= */

void preorderVisit(
    Product *node,
    void (*visit)(Product *product, void *data),
    void *data
)
{
    if (node == NULL)
    {
        return;
    }


    visit(node, data);


    preorderVisit(
        node->left,
        visit,
        data
    );


    preorderVisit(
        node->right,
        visit,
        data
    );
}


/* =========================================================
   POSTORDER TRAVERSAL
   =========================================================

   Order:

       Left -> Right -> Root
   ========================================================= */

void postorderVisit(
    Product *node,
    void (*visit)(Product *product, void *data),
    void *data
)
{
    if (node == NULL)
    {
        return;
    }


    postorderVisit(
        node->left,
        visit,
        data
    );


    postorderVisit(
        node->right,
        visit,
        data
    );

    visit(node, data);
}

/* =========================================================
   COUNT PRODUCTS
   ========================================================= */

int countNodes(Product *node)
{
    /* Empty tree */

    if (node == NULL)
    {
        return 0;
    }

    /*
        Count:

        1 current node
        + left subtree
        + right subtree
    */

    return
        1
        + countNodes(node->left)
        + countNodes(node->right);
}

/* =========================================================
   COPY PRODUCT DATA
   =========================================================

   Copies only product information.

   Tree pointers and height are not copied.
   ========================================================= */

void copyProductFields(
    Product *destination,
    const Product *source
)
{
    strcpy(
        destination->id,
        source->id
    );

    strcpy(
        destination->name,
        source->name
    );

    strcpy(
        destination->category,
        source->category
    );

    destination->buyPrice =
        source->buyPrice;

    destination->sellPrice =
        source->sellPrice;

    destination->quantity =
        source->quantity;

    destination->minStock =
        source->minStock;

    strcpy(
        destination->expiryDate,
        source->expiryDate
    );

    strcpy(
        destination->supplierId,
        source->supplierId
    );
}

/* =========================================================
   INSERT PRODUCT RECORD
   =========================================================

   Creates a new AVL node and inserts it.
   ========================================================= */

Product *insertProductRecord(Product data)
{
    Product *newNode;


    /* Allocate memory */

    newNode =
        (Product *)malloc(
            sizeof(Product)
        );

    if (newNode == NULL)
    {
        printf(
            "Memory allocation failed!\n"
        );

        exit(1);
    }

    /* Copy product information */

    copyProductFields(
        newNode,
        &data
    );

    /* New node starts with height 1 */

    newNode->height = 1;

    /* New node has no children */

    newNode->left = NULL;
    newNode->right = NULL;


    /* Insert into AVL Tree */

    root =
        insertProduct(
            root,
            newNode
        );


    return newNode;
}

/* =========================================================
   DELETE PRODUCT BY ID
   ========================================================= */

int deleteProductById(
    const char *id,
    Product *backup
)
{
    int found = 0;


    root =
        deleteProduct(
            root,
            id,
            &found,
            backup
        );


    return found;
}

/* =========================================================
   FIND PRODUCT
   ========================================================= */

Product *findProduct(const char *id)
{
    return searchProductById(
        root,
        id
    );
}


/* =========================================================
   UPDATE PRODUCT
   =========================================================

   Updates product information.

   The old data is saved in oldBackup
   so that Undo can restore it.
   ========================================================= */

int updateProductData(
    const char *id,
    Product newData,
    Product *oldBackup
)
{
    Product *product;


    /* Find product */

    product =
        findProduct(id);

    /* Product not found */

    if (product == NULL)
    {
        return 0;
    }

    /* Save old data for Undo */

    if (oldBackup != NULL)
    {
        copyProductFields(
            oldBackup,
            product
        );
    }

    /* Update product information */

    strcpy(
        product->name,
        newData.name
    );

    strcpy(
        product->category,
        newData.category
    );


    product->buyPrice =
        newData.buyPrice;

    product->sellPrice =
        newData.sellPrice;

    product->quantity =
        newData.quantity;

    product->minStock =
        newData.minStock;


    strcpy(
        product->expiryDate,
        newData.expiryDate
    );

    strcpy(
        product->supplierId,
        newData.supplierId
    );


    return 1;
}
/* =========================================================
   FREE AVL TREE
   =========================================================

   Frees all nodes when the program closes.

   We use Postorder because children should be
   freed before their parent.
   ========================================================= */

void freeTree(Product *node)
{
    if (node == NULL)
    {
        return;
    }
    /* Free left subtree */ 

    freeTree(node->left);

    /* Free right subtree */

    freeTree(node->right);

    /* Free current node */
    free(node);
}
