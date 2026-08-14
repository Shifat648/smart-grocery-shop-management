/* ============================================================
   main.c
   SMART GROCERY SHOP MANAGEMENT SYSTEM
   Data Structures & Algorithms University Project

   Data Structures demonstrated:
     - AVL Tree   : Product inventory (product.c / avl.c)
     - Stack      : Undo Last Operation (stack.c)
     - Linked List: Sales, Purchases, Transactions,
                    Attendance, Employees
     - Structures, Pointers, Dynamic Memory, File Handling
       throughout every module.

   Program flow:
     Load all data from disk -> Build AVL tree -> Main Menu loop
     -> Save all data to disk -> Free memory -> Exit
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

/* externs from avl.c */
extern Product *root;
extern void freeTree(Product *node);

/* externs from employee.c */
extern void freeEmployeeList(void);

/* externs from linkedlist.c */
extern void freeSaleList(void);
extern void freePurchaseList(void);
extern void freeTransactionList(void);
extern void freeAttendanceList(void);

/* externs from stack.c */
extern void clearUndoStack(void);
extern int performUndo(void);

/* externs from file.c */
extern void loadAllData(void);
extern void saveAllData(void);

/* externs from product.c */
extern void addProduct(void);
extern void searchProduct(void);
extern void updateProduct(void);
extern void deleteProductMenu(void);
extern void showInventory(void);
extern void lowStockAlert(void);
extern void expiredProducts(void);

/* externs from sales.c */
extern void sellProduct(void);

/* externs from reports.c */
extern void reportsMenu(void);

/* externs from accounting.c */
extern void accountingMenu(void);

/* externs from attendance.c */
extern void attendanceMenu(void);

/* externs from utils.c */
extern int readInt(const char *prompt);

/* ---- prototypes ---- */
void printMainMenu(void);
void freeAllMemory(void);
int main(void);

void printMainMenu(void) {
    printf("\n==================================================\n");
    printf("       SMART GROCERY SHOP MANAGEMENT SYSTEM\n");
    printf("==================================================\n");
    printf(" 1. Add Product\n");
    printf(" 2. Search Product\n");
    printf(" 3. Sell Product\n");
    printf(" 4. Update Product\n");
    printf(" 5. Delete Product\n");
    printf(" 6. Show Inventory\n");
    printf(" 7. Low Stock Alert\n");
    printf(" 8. Expired Products\n");
    printf(" 9. Undo Last Operation\n");
    printf("10. Reports\n");
    printf("11. Accounting\n");
    printf("12. Attendance Management\n");
    printf("13. Exit\n");
    printf("==================================================\n");
}

void freeAllMemory(void) {
    freeTree(root);
    freeEmployeeList();
    freeSaleList();
    freePurchaseList();
    freeTransactionList();
    freeAttendanceList();
    clearUndoStack();
}

int main(void) {
    printf("Loading saved data...\n");
    loadAllData();
    printf("Data loaded successfully. Welcome to Smart Grocery Shop!\n");

    int choice;
    do {
        printMainMenu();
        choice = readInt("Enter your choice (1-13): ");

        if (feof(stdin)) {
            printf("\nInput stream closed. Saving data and exiting...\n");
            saveAllData();
            freeAllMemory();
            break;
        }

        switch (choice) {
            case 1: addProduct(); saveAllData(); break;
            case 2: searchProduct(); break;
            case 3: sellProduct(); saveAllData(); break;
            case 4: updateProduct(); saveAllData(); break;
            case 5: deleteProductMenu(); saveAllData(); break;
            case 6: showInventory(); break;
            case 7: lowStockAlert(); break;
            case 8: expiredProducts(); break;
            case 9: performUndo(); saveAllData(); break;
            case 10: reportsMenu(); break;
            case 11: accountingMenu(); saveAllData(); break;
            case 12: attendanceMenu(); saveAllData(); break;
            case 13:
                printf("Saving data before exit...\n");
                saveAllData();
                freeAllMemory();
                printf("Thank you for using Smart Grocery Shop Management System!\n");
                break;
            default:
                printf("Invalid choice! Please enter a number between 1 and 13.\n");
        }
    } while (choice != 13);

    return 0;
}
