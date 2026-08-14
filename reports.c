/* ============================================================
   reports.c
   Implements main menu option 10: Reports
   Pulls data from the AVL tree, the linked lists and the
   employee list to build Inventory / Sales / Purchase /
   Accounting / Employee / Attendance reports.
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

typedef struct Purchase {
    char productId[20];
    char productName[50];
    int quantity;
    float costPerUnit;
    float total;
    char date[12];
    struct Purchase *next;
} Purchase;

typedef struct Employee {
    char id[20];
    char name[50];
    char phone[15];
    char position[30];
    float salary;
    char joinDate[12];
    struct Employee *next;
} Employee;

typedef struct Attendance {
    char empId[20];
    char empName[50];
    char date[12];
    char status[10];
    struct Attendance *next;
} Attendance;

/* externs from avl.c */
extern Product *root;
extern void inorderVisit(Product *node, void (*visit)(Product *p, void *ctx), void *ctx);

/* externs from linkedlist.c */
extern Sale *saleHead;
extern Purchase *purchaseHead;

/* externs from employee.c */
extern Employee *employeeHead;

/* externs from attendance list (linkedlist.c) */
extern Attendance *attendanceHead;

/* externs from accounting.c */
extern float totalIncome(void);
extern float totalExpense(void);

/* externs from product.c */
extern void lowStockAlert(void);
extern void expiredProducts(void);

/* externs from utils.c */
extern int readInt(const char *prompt);

/* ---- prototypes ---- */
void inventoryReport(void);
void salesReport(void);
void purchaseReport(void);
void accountingReport(void);
void employeeReport(void);
void attendanceReport(void);
void reportsMenu(void);
void reportStatsCB(Product *p, void *ctx);

void reportStatsCB(Product *p, void *ctx) {
    float *totals = (float*)ctx;
    totals[0] += 1;
    totals[1] += p->quantity;
    totals[2] += p->quantity * p->sellPrice;
}

void inventoryReport(void) {
    printf("\n========== INVENTORY REPORT ==========\n");
    float totals[3] = {0, 0, 0};
    inorderVisit(root, reportStatsCB, totals);
    printf("Total Products       : %.0f\n", totals[0]);
    printf("Total Stock Quantity  : %.0f\n", totals[1]);
    printf("Inventory Value       : %.2f BDT\n", totals[2]);
    printf("\n-- Low Stock Products --\n");
    lowStockAlert();
    printf("\n-- Expired Products --\n");
    expiredProducts();
}

void salesReport(void) {
    printf("\n========== SALES REPORT ==========\n");
    Sale *cur = saleHead;
    float totalSales = 0;
    int totalUnits = 0;
    if (cur == NULL) { printf("No sales recorded yet.\n"); return; }
    printf("%-10s %-20s %-6s %-10s %-10s %-12s\n", "ProdID", "Name", "Qty", "Price", "Total", "Date");
    while (cur != NULL) {
        printf("%-10s %-20s %-6d %-10.2f %-10.2f %-12s\n", cur->productId, cur->productName, cur->quantity, cur->pricePerUnit, cur->total, cur->date);
        totalSales += cur->total;
        totalUnits += cur->quantity;
        cur = cur->next;
    }
    printf("---------------------------------------------------------\n");
    printf("Total Sales (Revenue): %.2f BDT\n", totalSales);
    printf("Total Units Sold      : %d\n", totalUnits);
}

void purchaseReport(void) {
    printf("\n========== PURCHASE REPORT ==========\n");
    Purchase *cur = purchaseHead;
    float totalCost = 0;
    int totalUnits = 0;
    if (cur == NULL) { printf("No purchases recorded yet.\n"); return; }
    printf("%-10s %-20s %-6s %-10s %-10s %-12s\n", "ProdID", "Name", "Qty", "Cost/Unit", "Total", "Date");
    while (cur != NULL) {
        printf("%-10s %-20s %-6d %-10.2f %-10.2f %-12s\n", cur->productId, cur->productName, cur->quantity, cur->costPerUnit, cur->total, cur->date);
        totalCost += cur->total;
        totalUnits += cur->quantity;
        cur = cur->next;
    }
    printf("---------------------------------------------------------\n");
    printf("Total Purchases (Units): %d\n", totalUnits);
    printf("Total Purchase Cost     : %.2f BDT\n", totalCost);
}

void accountingReport(void) {
    printf("\n========== ACCOUNTING REPORT ==========\n");
    float income = totalIncome();
    float expense = totalExpense();
    printf("Total Income : %.2f BDT\n", income);
    printf("Total Expense: %.2f BDT\n", expense);
    printf("Profit/Loss  : %.2f BDT\n", income - expense);
}

void employeeReport(void) {
    printf("\n========== EMPLOYEE REPORT ==========\n");
    Employee *cur = employeeHead;
    if (cur == NULL) { printf("No employees found.\n"); return; }
    printf("%-10s %-20s %-15s %-15s %-10s %-12s\n", "ID", "Name", "Phone", "Position", "Salary", "Join Date");
    while (cur != NULL) {
        printf("%-10s %-20s %-15s %-15s %-10.2f %-12s\n", cur->id, cur->name, cur->phone, cur->position, cur->salary, cur->joinDate);
        cur = cur->next;
    }
}

void attendanceReport(void) {
    printf("\n========== ATTENDANCE REPORT ==========\n");
    Attendance *cur = attendanceHead;
    int present = 0, absent = 0, late = 0, leave = 0, total = 0;
    while (cur != NULL) {
        total++;
        if (strcmp(cur->status, "Present") == 0) present++;
        else if (strcmp(cur->status, "Absent") == 0) absent++;
        else if (strcmp(cur->status, "Late") == 0) late++;
        else if (strcmp(cur->status, "Leave") == 0) leave++;
        cur = cur->next;
    }
    if (total == 0) { printf("No attendance records found.\n"); return; }
    printf("Present: %d | Absent: %d | Late: %d | Leave: %d\n", present, absent, late, leave);
    printf("Overall Attendance Percentage: %.2f%%\n", ((present + late) * 100.0) / total);
}

void reportsMenu(void) {
    int choice;
    do {
        printf("\n======= REPORTS MENU =======\n");
        printf("1. Inventory Report\n");
        printf("2. Sales Report\n");
        printf("3. Purchase Report\n");
        printf("4. Accounting Report\n");
        printf("5. Employee Report\n");
        printf("6. Attendance Report\n");
        printf("7. Back to Main Menu\n");
        choice = readInt("Enter choice: ");
        if (feof(stdin)) { printf("\nInput stream closed.\n"); break; }
        switch (choice) {
            case 1: inventoryReport(); break;
            case 2: salesReport(); break;
            case 3: purchaseReport(); break;
            case 4: accountingReport(); break;
            case 5: employeeReport(); break;
            case 6: attendanceReport(); break;
            case 7: printf("Returning to Main Menu...\n"); break;
            default: printf("Invalid choice. Try again.\n");
        }
    } while (choice != 7);
}
