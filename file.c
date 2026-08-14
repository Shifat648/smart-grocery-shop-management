/* ============================================================
   file.c
   Permanent storage using plain delimited text files (".dat").
   Fields are separated by '|' so names containing spaces are
   still stored safely, and the files stay human-readable which
   makes them easy to show during a lab evaluation.

   Load order on startup : Products -> Employees -> Sales ->
                            Purchases -> Transactions -> Attendance
   Save order on exit     : same modules, in the same order.
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

typedef struct Employee {
    char id[20];
    char name[50];
    char phone[15];
    char position[30];
    float salary;
    char joinDate[12];
    struct Employee *next;
} Employee;

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
    char type[10];
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
    char status[10];
    struct Attendance *next;
} Attendance;

#define DATA_DIR "data/"

/* externs from avl.c */
extern Product *root;
extern Product* insertProductRecord(Product data);
extern void inorderVisit(Product *node, void (*visit)(Product *p, void *ctx), void *ctx);

/* externs from employee.c */
extern Employee *employeeHead;

/* externs from linkedlist.c */
extern Sale *saleHead;
extern Purchase *purchaseHead;
extern Transaction *transactionHead;
extern Attendance *attendanceHead;
extern int nextTransactionId;
extern void addSale(Sale s);
extern void addPurchase(Purchase p);
extern int addTransaction(Transaction t);
extern void addAttendance(Attendance a);

/* ---- prototypes ---- */
void saveProductCB(Product *p, void *ctx);
void saveProducts(void);
void loadProducts(void);
void saveEmployees(void);
void loadEmployees(void);
void saveSales(void);
void loadSales(void);
void savePurchases(void);
void loadPurchases(void);
void saveTransactions(void);
void loadTransactions(void);
void saveAttendance(void);
void loadAttendance(void);
void loadAllData(void);
void saveAllData(void);

/* ---------------- PRODUCTS ---------------- */
void saveProductCB(Product *p, void *ctx) {
    FILE *fp = (FILE*)ctx;
    fprintf(fp, "%s|%s|%s|%.2f|%.2f|%d|%d|%s|%s\n",
            p->id, p->name, p->category, p->buyPrice, p->sellPrice,
            p->quantity, p->minStock, p->expiryDate, p->supplierId);
}

void saveProducts(void) {
    FILE *fp = fopen(DATA_DIR "products.dat", "w");
    if (fp == NULL) { printf("Warning: could not save products.dat\n"); return; }
    inorderVisit(root, saveProductCB, fp);
    fclose(fp);
}

void loadProducts(void) {
    FILE *fp = fopen(DATA_DIR "products.dat", "r");
    if (fp == NULL) return; /* first run, no data yet */
    char line[300];
    while (fgets(line, sizeof(line), fp) != NULL) {
        Product p;
        memset(&p, 0, sizeof(p));
        int n = sscanf(line, "%19[^|]|%49[^|]|%29[^|]|%f|%f|%d|%d|%11[^|]|%19[^\n]",
                        p.id, p.name, p.category, &p.buyPrice, &p.sellPrice,
                        &p.quantity, &p.minStock, p.expiryDate, p.supplierId);
        if (n == 9) insertProductRecord(p);
    }
    fclose(fp);
}

/* ---------------- EMPLOYEES ---------------- */
void saveEmployees(void) {
    FILE *fp = fopen(DATA_DIR "employees.dat", "w");
    if (fp == NULL) { printf("Warning: could not save employees.dat\n"); return; }
    Employee *cur = employeeHead;
    while (cur != NULL) {
        fprintf(fp, "%s|%s|%s|%s|%.2f|%s\n", cur->id, cur->name, cur->phone, cur->position, cur->salary, cur->joinDate);
        cur = cur->next;
    }
    fclose(fp);
}

void loadEmployees(void) {
    FILE *fp = fopen(DATA_DIR "employees.dat", "r");
    if (fp == NULL) return;
    char line[300];
    while (fgets(line, sizeof(line), fp) != NULL) {
        Employee e;
        memset(&e, 0, sizeof(e));
        int n = sscanf(line, "%19[^|]|%49[^|]|%14[^|]|%29[^|]|%f|%11[^\n]",
                        e.id, e.name, e.phone, e.position, &e.salary, e.joinDate);
        if (n == 6) {
            Employee *node = (Employee*)malloc(sizeof(Employee));
            if (node == NULL) continue;
            *node = e;
            node->next = employeeHead;
            employeeHead = node;
        }
    }
    fclose(fp);
}

/* ---------------- SALES ---------------- */
void saveSales(void) {
    FILE *fp = fopen(DATA_DIR "sales.dat", "w");
    if (fp == NULL) { printf("Warning: could not save sales.dat\n"); return; }
    Sale *cur = saleHead;
    while (cur != NULL) {
        fprintf(fp, "%s|%s|%d|%.2f|%.2f|%s\n", cur->productId, cur->productName, cur->quantity, cur->pricePerUnit, cur->total, cur->date);
        cur = cur->next;
    }
    fclose(fp);
}

void loadSales(void) {
    FILE *fp = fopen(DATA_DIR "sales.dat", "r");
    if (fp == NULL) return;
    char line[300];
    while (fgets(line, sizeof(line), fp) != NULL) {
        Sale s;
        memset(&s, 0, sizeof(s));
        int n = sscanf(line, "%19[^|]|%49[^|]|%d|%f|%f|%11[^\n]",
                        s.productId, s.productName, &s.quantity, &s.pricePerUnit, &s.total, s.date);
        if (n == 6) addSale(s);
    }
    fclose(fp);
}

/* ---------------- PURCHASES ---------------- */
void savePurchases(void) {
    FILE *fp = fopen(DATA_DIR "purchases.dat", "w");
    if (fp == NULL) { printf("Warning: could not save purchases.dat\n"); return; }
    Purchase *cur = purchaseHead;
    while (cur != NULL) {
        fprintf(fp, "%s|%s|%d|%.2f|%.2f|%s\n", cur->productId, cur->productName, cur->quantity, cur->costPerUnit, cur->total, cur->date);
        cur = cur->next;
    }
    fclose(fp);
}

void loadPurchases(void) {
    FILE *fp = fopen(DATA_DIR "purchases.dat", "r");
    if (fp == NULL) return;
    char line[300];
    while (fgets(line, sizeof(line), fp) != NULL) {
        Purchase p;
        memset(&p, 0, sizeof(p));
        int n = sscanf(line, "%19[^|]|%49[^|]|%d|%f|%f|%11[^\n]",
                        p.productId, p.productName, &p.quantity, &p.costPerUnit, &p.total, p.date);
        if (n == 6) addPurchase(p);
    }
    fclose(fp);
}

/* ---------------- TRANSACTIONS ---------------- */
void saveTransactions(void) {
    FILE *fp = fopen(DATA_DIR "transactions.dat", "w");
    if (fp == NULL) { printf("Warning: could not save transactions.dat\n"); return; }
    /* transactionHead is newest-first; write oldest-first for readability */
    Transaction *arr[10000];
    int count = 0;
    Transaction *cur = transactionHead;
    while (cur != NULL && count < 10000) { arr[count++] = cur; cur = cur->next; }
    for (int i = count - 1; i >= 0; i--) {
        Transaction *t = arr[i];
        fprintf(fp, "%d|%s|%s|%.2f|%s|%s\n", t->id, t->type, t->category, t->amount, t->date, t->description);
    }
    fclose(fp);
}

void loadTransactions(void) {
    FILE *fp = fopen(DATA_DIR "transactions.dat", "r");
    if (fp == NULL) return;
    char line[300];
    int maxId = 0;
    while (fgets(line, sizeof(line), fp) != NULL) {
        Transaction t;
        memset(&t, 0, sizeof(t));
        int n = sscanf(line, "%d|%9[^|]|%29[^|]|%f|%11[^|]|%99[^\n]",
                        &t.id, t.type, t.category, &t.amount, t.date, t.description);
        if (n == 6) {
            Transaction *node = (Transaction*)malloc(sizeof(Transaction));
            if (node == NULL) continue;
            *node = t;
            node->next = transactionHead;
            transactionHead = node;
            if (t.id > maxId) maxId = t.id;
        }
    }
    nextTransactionId = maxId + 1;
    fclose(fp);
}

/* ---------------- ATTENDANCE ---------------- */
void saveAttendance(void) {
    FILE *fp = fopen(DATA_DIR "attendance.dat", "w");
    if (fp == NULL) { printf("Warning: could not save attendance.dat\n"); return; }
    Attendance *cur = attendanceHead;
    while (cur != NULL) {
        fprintf(fp, "%s|%s|%s|%s\n", cur->empId, cur->empName, cur->date, cur->status);
        cur = cur->next;
    }
    fclose(fp);
}

void loadAttendance(void) {
    FILE *fp = fopen(DATA_DIR "attendance.dat", "r");
    if (fp == NULL) return;
    char line[300];
    while (fgets(line, sizeof(line), fp) != NULL) {
        Attendance a;
        memset(&a, 0, sizeof(a));
        int n = sscanf(line, "%19[^|]|%49[^|]|%11[^|]|%9[^\n]", a.empId, a.empName, a.date, a.status);
        if (n == 4) addAttendance(a);
    }
    fclose(fp);
}

/* ---------------- COMBINED ---------------- */
void loadAllData(void) {
    loadProducts();
    loadEmployees();
    loadSales();
    loadPurchases();
    loadTransactions();
    loadAttendance();
}

void saveAllData(void) {
    saveProducts();
    saveEmployees();
    saveSales();
    savePurchases();
    saveTransactions();
    saveAttendance();
}
