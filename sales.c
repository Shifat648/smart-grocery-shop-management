/* ============================================================
   sales.c
   Implements main menu option 3: Sell Product

   Flow: Enter Product ID -> Search AVL Tree -> Check Stock ->
   Enter Quantity -> Calculate Total -> Reduce Stock ->
   Record Sale -> Automatically Record Income -> Push Undo -> Save
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

typedef struct Sale {
    char productId[20];
    char productName[50];
    int quantity;
    float pricePerUnit;
    float total;
    char date[12];
    struct Sale *next;
} Sale;

/* externs from avl.c */
extern Product* findProduct(const char *id);

/* externs from linkedlist.c */
extern void addSale(Sale s);

/* externs from stack.c */
extern void pushUndoSell(Product p, int qty, int transactionId);

/* externs from accounting.c */
extern int addIncomeAuto(const char *category, float amount, const char *description);

/* externs from utils.c */
extern void readString(const char *prompt, char *buf, int size);
extern int readInt(const char *prompt);
extern void getCurrentDate(char *buf);

/* ---- prototypes ---- */
void sellProduct(void);

void sellProduct(void) {
    char id[20];
    printf("\n--- Sell Product ---\n");
    readString("Enter Product ID: ", id, sizeof(id));

    Product *p = findProduct(id);
    if (p == NULL) {
        printf("Error: Product with ID '%s' not found.\n", id);
        return;
    }

    printf("Product: %s | Available Stock: %d | Price: %.2f BDT\n", p->name, p->quantity, p->sellPrice);

    if (p->quantity <= 0) {
        printf("Error: This product is out of stock.\n");
        return;
    }

    int qty = readInt("Enter quantity to sell: ");
    if (qty <= 0) {
        printf("Error: Quantity must be positive.\n");
        return;
    }
    if (qty > p->quantity) {
        printf("Error: Insufficient stock! Only %d units available.\n", p->quantity);
        return;
    }

    float total = qty * p->sellPrice;
    Product before = *p;   /* snapshot before reducing stock, for Undo */

    p->quantity -= qty;

    Sale s;
    strcpy(s.productId, p->id);
    strcpy(s.productName, p->name);
    s.quantity = qty;
    s.pricePerUnit = p->sellPrice;
    s.total = total;
    getCurrentDate(s.date);
    addSale(s);

    char desc[100];
    sprintf(desc, "Sale of %s (%d units)", p->name, qty);
    int txId = addIncomeAuto("Product Sale", total, desc);

    pushUndoSell(before, qty, txId);

    printf("Sale successful! %d x %s sold for a total of %.2f BDT.\n", qty, p->name, total);
    printf("Remaining stock: %d\n", p->quantity);
}
