/* ============================================================
   product.c
   Implements main menu options 1-8:
     1 Add Product   2 Search Product   4 Update Product
     5 Delete Product 6 Show Inventory  7 Low Stock Alert
     8 Expired Products
   Also implements Restocking/Purchasing (reached from the
   Update Product submenu) and the "9. Undo Last Operation"
   orchestration, since most undoable actions revolve around
   the Product AVL tree.
   ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

typedef struct Purchase {
    char productId[20];
    char productName[50];
    int quantity;
    float costPerUnit;
    float total;
    char date[12];
    struct Purchase *next;
} Purchase;

/* externs from avl.c */
extern Product *root;
extern Product* insertProductRecord(Product data);
extern int deleteProductById(const char *id, Product *backup);
extern Product* findProduct(const char *id);
extern int updateProductData(const char *id, Product newData, Product *oldBackup);
extern void inorderVisit(Product *node, void (*visit)(Product *p, void *ctx), void *ctx);
extern int countNodes(Product *node);

/* externs from linkedlist.c */
extern void addPurchase(Purchase p);
extern int removeLastPurchaseForProduct(const char *productId, int quantity);

/* externs from stack.c */
extern void pushUndoAdd(Product p);
extern void pushUndoDelete(Product p);
extern void pushUndoUpdate(Product oldData);
extern void pushUndoRestock(Product p, int qty, int transactionId);

/* externs from accounting.c */
extern int addExpenseAuto(const char *category, float amount, const char *description);
extern int removeTransactionById(int id);

/* externs from utils.c */
extern void readString(const char *prompt, char *buf, int size);
extern int readInt(const char *prompt);
extern float readFloat(const char *prompt);
extern void getCurrentDate(char *buf);
extern int isValidDateFormat(const char *date);
extern int isDateBeforeToday(const char *date);
extern int daysBetweenAndToday(const char *date);

/* ---- prototypes ---- */
void addProduct(void);
void searchProduct(void);
void updateProduct(void);
void restockProduct(void);
void restockProductById(const char *id);
void deleteProductMenu(void);
void showInventory(void);
void lowStockAlert(void);
void expiredProducts(void);
void printProductLineCB(Product *p, void *ctx);
void inventoryStatsCB(Product *p, void *ctx);
void lowStockCB(Product *p, void *ctx);
void expiredCB(Product *p, void *ctx);
void inventoryRowCB(Product *p, void *ctx);

typedef struct NameSearchCtx {
    char name[50];
    int found;
} NameSearchCtx;
void nameSearchCB(Product *p, void *ctx);

/* ------------------------------------------------------------ */
void addProduct(void) {
    Product p;
    memset(&p, 0, sizeof(p));

    printf("\n--- Add Product ---\n");
    readString("Product ID: ", p.id, sizeof(p.id));
    if (strlen(p.id) == 0) { printf("Product ID cannot be empty.\n"); return; }

    if (findProduct(p.id) != NULL) {
        printf("Error: Product ID '%s' already exists!\n", p.id);
        return;
    }

    readString("Product Name: ", p.name, sizeof(p.name));
    readString("Category: ", p.category, sizeof(p.category));
    p.buyPrice = readFloat("Buying Price: ");
    p.sellPrice = readFloat("Selling Price: ");
    if (p.buyPrice < 0 || p.sellPrice < 0) { printf("Prices cannot be negative.\n"); return; }
    p.quantity = readInt("Quantity: ");
    if (p.quantity < 0) { printf("Quantity cannot be negative.\n"); return; }
    p.minStock = readInt("Minimum Stock Level: ");

    do {
        readString("Expiry Date (DD-MM-YYYY): ", p.expiryDate, sizeof(p.expiryDate));
        if (feof(stdin)) { printf("Input ended. Add Product cancelled.\n"); return; }
        if (!isValidDateFormat(p.expiryDate)) printf("Invalid date format. Use DD-MM-YYYY.\n");
    } while (!isValidDateFormat(p.expiryDate));

    readString("Supplier ID: ", p.supplierId, sizeof(p.supplierId));

    insertProductRecord(p);
    pushUndoAdd(p);
    printf("Product '%s' added successfully!\n", p.name);
}

void searchProduct(void) {
    int choice;
    printf("\n--- Search Product ---\n");
    printf("1. Search by Product ID (AVL Tree)\n");
    printf("2. Search by Product Name\n");
    choice = readInt("Enter choice: ");

    if (choice == 1) {
        char id[20];
        readString("Enter Product ID: ", id, sizeof(id));
        Product *p = findProduct(id);
        if (p == NULL) { printf("Product with ID '%s' not found.\n", id); return; }
        printProductLineCB(p, NULL);
    } else if (choice == 2) {
        NameSearchCtx ctx;
        ctx.found = 0;
        readString("Enter Product Name: ", ctx.name, sizeof(ctx.name));
        /* linear inorder scan comparing name - AVL is keyed by ID only */
        inorderVisit(root, nameSearchCB, &ctx);
        if (ctx.found == 0) printf("No product found with name containing '%s'.\n", ctx.name);
    } else {
        printf("Invalid choice.\n");
    }
}

void nameSearchCB(Product *p, void *ctx) {
    NameSearchCtx *sc = (NameSearchCtx*)ctx;
    if (strstr(p->name, sc->name) != NULL) {
        printProductLineCB(p, NULL);
        sc->found++;
    }
}

void printProductLineCB(Product *p, void *ctx) {
    (void)ctx;
    printf("----------------------------------------\n");
    printf("ID            : %s\n", p->id);
    printf("Name          : %s\n", p->name);
    printf("Category      : %s\n", p->category);
    printf("Buying Price  : %.2f BDT\n", p->buyPrice);
    printf("Selling Price : %.2f BDT\n", p->sellPrice);
    printf("Quantity      : %d\n", p->quantity);
    printf("Minimum Stock : %d\n", p->minStock);
    printf("Expiry Date   : %s\n", p->expiryDate);
    printf("Supplier ID   : %s\n", p->supplierId);
}

void updateProduct(void) {
    char id[20];
    printf("\n--- Update Product ---\n");
    readString("Enter Product ID to update: ", id, sizeof(id));
    Product *p = findProduct(id);
    if (p == NULL) { printf("Product with ID '%s' not found.\n", id); return; }

    printProductLineCB(p, NULL);
    printf("\n1. Update Fields\n2. Restock (Purchase more stock)\n3. Cancel\n");
    int choice = readInt("Enter choice: ");

    if (choice == 1) {
        Product newData = *p;
        printf("Leave blank to keep current value.\n");

        char buf[80];
        readString("New Name: ", buf, sizeof(buf));
        if (strlen(buf) > 0) strcpy(newData.name, buf);

        readString("New Category: ", buf, sizeof(buf));
        if (strlen(buf) > 0) strcpy(newData.category, buf);

        readString("New Buying Price (or blank): ", buf, sizeof(buf));
        if (strlen(buf) > 0) newData.buyPrice = (float)atof(buf);

        readString("New Selling Price (or blank): ", buf, sizeof(buf));
        if (strlen(buf) > 0) newData.sellPrice = (float)atof(buf);

        readString("New Quantity (or blank): ", buf, sizeof(buf));
        if (strlen(buf) > 0) newData.quantity = atoi(buf);

        readString("New Minimum Stock (or blank): ", buf, sizeof(buf));
        if (strlen(buf) > 0) newData.minStock = atoi(buf);

        readString("New Expiry Date DD-MM-YYYY (or blank): ", buf, sizeof(buf));
        if (strlen(buf) > 0) {
            if (isValidDateFormat(buf)) strcpy(newData.expiryDate, buf);
            else printf("Invalid date ignored, keeping old expiry date.\n");
        }

        readString("New Supplier ID (or blank): ", buf, sizeof(buf));
        if (strlen(buf) > 0) strcpy(newData.supplierId, buf);

        Product oldBackup;
        updateProductData(id, newData, &oldBackup);
        pushUndoUpdate(oldBackup);
        printf("Product updated successfully!\n");
    } else if (choice == 2) {
        printf("\n--- Restock Product (Purchase) ---\n");
        restockProductById(id);
    } else {
        printf("Update cancelled.\n");
    }
}

void restockProduct(void) {
    char id[20];
    printf("\n--- Restock Product (Purchase) ---\n");
    readString("Enter Product ID to restock: ", id, sizeof(id));
    restockProductById(id);
}

/* Restocking / Purchasing: increases stock and automatically records
   a Purchase entry + an Expense transaction. */
void restockProductById(const char *id) {
    Product *p = findProduct(id);
    if (p == NULL) { printf("Product with ID '%s' not found.\n", id); return; }

    int qty = readInt("Enter quantity to purchase: ");
    if (qty <= 0) { printf("Quantity must be positive.\n"); return; }

    float costPerUnit = readFloat("Enter cost per unit (buying price): ");
    if (costPerUnit < 0) { printf("Cost cannot be negative.\n"); return; }

    Product before = *p;
    p->quantity += qty;
    p->buyPrice = costPerUnit; /* keep buying price current */

    Purchase pur;
    strcpy(pur.productId, p->id);
    strcpy(pur.productName, p->name);
    pur.quantity = qty;
    pur.costPerUnit = costPerUnit;
    pur.total = qty * costPerUnit;
    getCurrentDate(pur.date);
    addPurchase(pur);

    char desc[100];
    sprintf(desc, "Restock of %s (%d units)", p->name, qty);
    int txId = addExpenseAuto("Restocking", pur.total, desc);

    pushUndoRestock(before, qty, txId);

    printf("Restocked %d units of '%s'. Total cost: %.2f BDT (recorded as Expense)\n", qty, p->name, pur.total);
}

void deleteProductMenu(void) {
    char id[20];
    printf("\n--- Delete Product ---\n");
    readString("Enter Product ID to delete: ", id, sizeof(id));
    if (findProduct(id) == NULL) { printf("Product with ID '%s' not found.\n", id); return; }

    Product backup;
    int found = deleteProductById(id, &backup);
    if (found) {
        pushUndoDelete(backup);
        printf("Product '%s' deleted successfully.\n", backup.name);
    } else {
        printf("Delete failed.\n");
    }
}

void inventoryRowCB(Product *p, void *ctx) {
    (void)ctx;
    printf("%-8s %-20s %-12s %-10.2f %-10.2f %-6d %-6d %-12s\n",
           p->id, p->name, p->category, p->buyPrice, p->sellPrice, p->quantity, p->minStock, p->expiryDate);
}

void inventoryStatsCB(Product *p, void *ctx) {
    float *totals = (float*)ctx; /* [0]=count [1]=totalQty [2]=totalValue */
    totals[0] += 1;
    totals[1] += p->quantity;
    totals[2] += p->quantity * p->sellPrice;
}

void showInventory(void) {
    printf("\n============================== INVENTORY (sorted by ID) ==============================\n");
    printf("%-8s %-20s %-12s %-10s %-10s %-6s %-6s %-12s\n",
           "ID", "Name", "Category", "BuyPrice", "SellPrice", "Qty", "MinStk", "Expiry");
    printf("----------------------------------------------------------------------------------------\n");

    inorderVisit(root, inventoryRowCB, NULL);

    float totals[3] = {0, 0, 0};
    inorderVisit(root, inventoryStatsCB, totals);
    printf("----------------------------------------------------------------------------------------\n");
    printf("Total Products      : %.0f\n", totals[0]);
    printf("Total Stock Quantity : %.0f\n", totals[1]);
    printf("Total Inventory Value: %.2f BDT\n", totals[2]);
}

void lowStockCB(Product *p, void *ctx) {
    int *count = (int*)ctx;
    if (p->quantity <= p->minStock) {
        printf("Product: %-20s Current Stock: %-5d Minimum Stock: %-5d  ** LOW STOCK - RESTOCK REQUIRED **\n",
               p->name, p->quantity, p->minStock);
        (*count)++;
    }
}

void lowStockAlert(void) {
    printf("\n--- Low Stock Alert ---\n");
    int count = 0;
    inorderVisit(root, lowStockCB, &count);
    if (count == 0) printf("No products are currently low on stock.\n");
}

void expiredCB(Product *p, void *ctx) {
    int *counts = (int*)ctx; /* [0]=expired [1]=expiringSoon */
    int daysLeft = daysBetweenAndToday(p->expiryDate);
    if (daysLeft < 0) {
        printf("EXPIRED   : %-20s Expiry Date: %s (%d days ago)\n", p->name, p->expiryDate, -daysLeft);
        counts[0]++;
    } else if (daysLeft <= 7) {
        printf("EXPIRING SOON: %-20s Expiry Date: %s (in %d day%s)\n",
               p->name, p->expiryDate, daysLeft, daysLeft == 1 ? "" : "s");
        counts[1]++;
    }
}

void expiredProducts(void) {
    printf("\n--- Expired / Expiring Soon Products (within 7 days) ---\n");
    int counts[2] = {0, 0};
    inorderVisit(root, expiredCB, counts);
    if (counts[0] == 0 && counts[1] == 0)
        printf("No expired or soon-to-expire products found.\n");
    else
        printf("Total Expired: %d | Total Expiring Soon: %d\n", counts[0], counts[1]);
}
