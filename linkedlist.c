/* ============================================================
   linkedlist.c
   Singly linked lists used to keep the running history of:
     - Sales
     - Purchases (restocking)
     - Accounting Transactions (income/expense)
     - Employee Attendance
   Each history simply grows at the head (O(1) insert) and is
   walked front-to-back for reports.
   ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Sale {
    char productId[20];
    char productName[50];
    int quantity;
    float pricePerUnit;
    float total;
    char date[12];
    struct Sale *next;
} Sale;

typedef struct Purchase {
    char productId[20];
    char productName[50];
    int quantity;
    float costPerUnit;
    float total;
    char date[12];
    struct Purchase *next;
} Purchase;

typedef struct Transaction {
    int id;
    char type[10];       /* INCOME or EXPENSE */
    char category[30];
    float amount;
    char date[12];
    char description[100];
    struct Transaction *next;
} Transaction;

typedef struct Attendance {
    char empId[20];
    char empName[50];
    char date[12];
    char status[10];      /* Present / Absent / Late / Leave */
    struct Attendance *next;
} Attendance;

Sale *saleHead = NULL;
Purchase *purchaseHead = NULL;
Transaction *transactionHead = NULL;
Attendance *attendanceHead = NULL;
int nextTransactionId = 1;

/* ---- prototypes ---- */
void addSale(Sale s);
void addPurchase(Purchase p);
int addTransaction(Transaction t); /* returns assigned id */
void addAttendance(Attendance a);
int removeTransactionById(int id);
int removeLastSaleForProduct(const char *productId, int quantity);
int removeLastPurchaseForProduct(const char *productId, int quantity);
void freeSaleList(void);
void freePurchaseList(void);
void freeTransactionList(void);
void freeAttendanceList(void);

void addSale(Sale s) {
    Sale *node = (Sale*)malloc(sizeof(Sale));
    if (!node) { printf("Memory allocation failed!\n"); return; }
    *node = s;
    node->next = saleHead;
    saleHead = node;
}

void addPurchase(Purchase p) {
    Purchase *node = (Purchase*)malloc(sizeof(Purchase));
    if (!node) { printf("Memory allocation failed!\n"); return; }
    *node = p;
    node->next = purchaseHead;
    purchaseHead = node;
}

int addTransaction(Transaction t) {
    Transaction *node = (Transaction*)malloc(sizeof(Transaction));
    if (!node) { printf("Memory allocation failed!\n"); return -1; }
    t.id = nextTransactionId++;
    *node = t;
    node->next = transactionHead;
    transactionHead = node;
    return t.id;
}

void addAttendance(Attendance a) {
    Attendance *node = (Attendance*)malloc(sizeof(Attendance));
    if (!node) { printf("Memory allocation failed!\n"); return; }
    *node = a;
    node->next = attendanceHead;
    attendanceHead = node;
}

/* Used by Undo to reverse an automatic income/expense entry */
int removeTransactionById(int id) {
    Transaction *cur = transactionHead, *prev = NULL;
    while (cur != NULL) {
        if (cur->id == id) {
            if (prev == NULL) transactionHead = cur->next;
            else prev->next = cur->next;
            free(cur);
            return 1;
        }
        prev = cur;
        cur = cur->next;
    }
    return 0;
}

/* Removes the most recent matching sale record (used when undoing a sale) */
int removeLastSaleForProduct(const char *productId, int quantity) {
    Sale *cur = saleHead, *prev = NULL;
    while (cur != NULL) {
        if (strcmp(cur->productId, productId) == 0 && cur->quantity == quantity) {
            if (prev == NULL) saleHead = cur->next;
            else prev->next = cur->next;
            free(cur);
            return 1;
        }
        prev = cur;
        cur = cur->next;
    }
    return 0;
}

/* Removes the most recent matching purchase record (used when undoing a restock) */
int removeLastPurchaseForProduct(const char *productId, int quantity) {
    Purchase *cur = purchaseHead, *prev = NULL;
    while (cur != NULL) {
        if (strcmp(cur->productId, productId) == 0 && cur->quantity == quantity) {
            if (prev == NULL) purchaseHead = cur->next;
            else prev->next = cur->next;
            free(cur);
            return 1;
        }
        prev = cur;
        cur = cur->next;
    }
    return 0;
}

void freeSaleList(void) {
    Sale *cur = saleHead;
    while (cur != NULL) { Sale *tmp = cur; cur = cur->next; free(tmp); }
    saleHead = NULL;
}

void freePurchaseList(void) {
    Purchase *cur = purchaseHead;
    while (cur != NULL) { Purchase *tmp = cur; cur = cur->next; free(tmp); }
    purchaseHead = NULL;
}

void freeTransactionList(void) {
    Transaction *cur = transactionHead;
    while (cur != NULL) { Transaction *tmp = cur; cur = cur->next; free(tmp); }
    transactionHead = NULL;
}

void freeAttendanceList(void) {
    Attendance *cur = attendanceHead;
    while (cur != NULL) { Attendance *tmp = cur; cur = cur->next; free(tmp); }
    attendanceHead = NULL;
}
