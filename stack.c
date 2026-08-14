/* ============================================================
   stack.c
   A Stack (LIFO - Last In First Out) used to implement the
   "Undo Last Operation" feature. Every reversible action in the
   shop (Add/Update/Delete Product, Sell, Restock, Accounting
   entry) pushes an UndoOp record here. Push / Pop / Peek are O(1).
   ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Same layout as the Product struct in avl.c - duplicated here
   because this project intentionally avoids shared header files.
   Only the plain data fields are needed for a backup snapshot. */
typedef struct Product {
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

/* One Undo record. Depending on opType, different fields are used:
   ADD      -> after holds the product that was added (undo = delete it)
   DELETE   -> before holds the deleted product (undo = re-insert it)
   UPDATE   -> before holds the pre-update values (undo = restore them)
   SELL     -> before.id = product id, quantity = qty sold,
               transactionId = the auto income transaction to remove
   RESTOCK  -> before.id = product id, quantity = qty added,
               transactionId = the auto expense transaction to remove
   ACCOUNT  -> transactionId = manual income/expense entry to remove
   SALARY   -> transactionId = salary expense entry to remove          */
typedef struct UndoOp {
    char opType[20];
    Product before;
    Product after;
    int quantity;
    int transactionId;
    struct UndoOp *next;
} UndoOp;

UndoOp *undoTop = NULL;

/* ---- prototypes ---- */
void pushUndo(UndoOp op);
int popUndo(UndoOp *out);
int peekUndo(UndoOp *out);
int isStackEmpty(void);
void clearUndoStack(void);
void pushUndoAdd(Product p);
void pushUndoDelete(Product p);
void pushUndoUpdate(Product oldData);
void pushUndoSell(Product p, int qty, int transactionId);
void pushUndoRestock(Product p, int qty, int transactionId);
void pushUndoAccount(const char *opType, int transactionId);

/* ---- convenience constructors so other modules never touch UndoOp fields directly ---- */
void pushUndoAdd(Product p) {
    UndoOp op; memset(&op, 0, sizeof(op));
    strcpy(op.opType, "ADD");
    op.after = p;
    pushUndo(op);
}

void pushUndoDelete(Product p) {
    UndoOp op; memset(&op, 0, sizeof(op));
    strcpy(op.opType, "DELETE");
    op.before = p;
    pushUndo(op);
}

void pushUndoUpdate(Product oldData) {
    UndoOp op; memset(&op, 0, sizeof(op));
    strcpy(op.opType, "UPDATE");
    op.before = oldData;
    pushUndo(op);
}

void pushUndoSell(Product p, int qty, int transactionId) {
    UndoOp op; memset(&op, 0, sizeof(op));
    strcpy(op.opType, "SELL");
    op.before = p;
    op.quantity = qty;
    op.transactionId = transactionId;
    pushUndo(op);
}

void pushUndoRestock(Product p, int qty, int transactionId) {
    UndoOp op; memset(&op, 0, sizeof(op));
    strcpy(op.opType, "RESTOCK");
    op.before = p;
    op.quantity = qty;
    op.transactionId = transactionId;
    pushUndo(op);
}

void pushUndoAccount(const char *opType, int transactionId) {
    UndoOp op; memset(&op, 0, sizeof(op));
    strcpy(op.opType, opType);
    op.transactionId = transactionId;
    pushUndo(op);
}

void pushUndo(UndoOp op) {
    UndoOp *node = (UndoOp*)malloc(sizeof(UndoOp));
    if (node == NULL) {
        printf("Memory allocation failed while pushing undo operation!\n");
        return;
    }
    *node = op;
    node->next = undoTop;
    undoTop = node;
}

int isStackEmpty(void) {
    return undoTop == NULL;
}

int popUndo(UndoOp *out) {
    if (isStackEmpty()) return 0;
    UndoOp *top = undoTop;
    *out = *top;
    undoTop = undoTop->next;
    free(top);
    return 1;
}

int peekUndo(UndoOp *out) {
    if (isStackEmpty()) return 0;
    *out = *undoTop;
    return 1;
}

void clearUndoStack(void) {
    UndoOp tmp;
    while (popUndo(&tmp)) { /* discard */ }
}

/* ---- externs needed to actually reverse operations ---- */
extern Product* insertProductRecord(Product data);
extern int deleteProductById(const char *id, Product *backup);
extern Product* findProduct(const char *id);
extern int updateProductData(const char *id, Product newData, Product *oldBackup);
extern int removeTransactionById(int id);
extern int removeLastSaleForProduct(const char *productId, int quantity);
extern int removeLastPurchaseForProduct(const char *productId, int quantity);

int performUndo(void) {
    UndoOp op;
    if (!popUndo(&op)) {
        printf("Nothing to undo. The undo stack is empty.\n");
        return 0;
    }

    if (strcmp(op.opType, "ADD") == 0) {
        Product backup;
        deleteProductById(op.after.id, &backup);
        printf("UNDO: Removed product '%s' that was just added.\n", op.after.name);

    } else if (strcmp(op.opType, "DELETE") == 0) {
        insertProductRecord(op.before);
        printf("UNDO: Restored deleted product '%s'.\n", op.before.name);

    } else if (strcmp(op.opType, "UPDATE") == 0) {
        Product discard;
        updateProductData(op.before.id, op.before, &discard);
        printf("UNDO: Reverted product '%s' to its previous values.\n", op.before.name);

    } else if (strcmp(op.opType, "SELL") == 0) {
        Product *p = findProduct(op.before.id);
        if (p != NULL) {
            p->quantity += op.quantity; /* restore stock */
        }
        removeLastSaleForProduct(op.before.id, op.quantity);
        removeTransactionById(op.transactionId);
        printf("UNDO: Sale reversed. Stock restored (+%d) and income transaction #%d removed.\n",
               op.quantity, op.transactionId);

    } else if (strcmp(op.opType, "RESTOCK") == 0) {
        Product *p = findProduct(op.before.id);
        if (p != NULL) {
            p->quantity -= op.quantity; /* undo the stock increase */
            p->buyPrice = op.before.buyPrice; /* restore old buying price */
        }
        removeLastPurchaseForProduct(op.before.id, op.quantity);
        removeTransactionById(op.transactionId);
        printf("UNDO: Restock reversed. Stock reduced (-%d) and expense transaction #%d removed.\n",
               op.quantity, op.transactionId);

    } else if (strcmp(op.opType, "ACCOUNT_INCOME") == 0 || strcmp(op.opType, "ACCOUNT_EXPENSE") == 0 ||
               strcmp(op.opType, "SALARY") == 0) {
        removeTransactionById(op.transactionId);
        printf("UNDO: Removed transaction #%d.\n", op.transactionId);

    } else {
        printf("UNDO: Unknown operation type, nothing was reversed.\n");
        return 0;
    }

    return 1;
}
