/* ============================================================
   accounting.c
   Handles all money in/out of the shop:
     - Manual Add Income / Add Expense
     - Automatic income from Sales, automatic expense from
       Restocking and Employee Salary (called by other modules)
     - View transactions, Daily / Monthly accounting
     - Total Income, Total Expense, Profit/Loss
   Transactions are kept in the linked list defined in linkedlist.c
   ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Transaction {
    int id;
    char type[10];
    char category[30];
    float amount;
    char date[12];
    char description[100];
    struct Transaction *next;
} Transaction;

/* externs from linkedlist.c */
extern Transaction *transactionHead;
extern int addTransaction(Transaction t);
extern int removeTransactionById(int id);

/* externs from stack.c */
extern void pushUndoAccount(const char *opType, int transactionId);

/* externs from utils.c */
extern void readString(const char *prompt, char *buf, int size);
extern float readFloat(const char *prompt);
extern int readInt(const char *prompt);
extern void getCurrentDate(char *buf);

/* ---- prototypes ---- */
int addIncomeAuto(const char *category, float amount, const char *description);
int addExpenseAuto(const char *category, float amount, const char *description);
void addIncomeManual(void);
void addExpenseManual(void);
void viewTransactions(void);
void dailyAccounting(void);
void monthlyAccounting(void);
float totalIncome(void);
float totalExpense(void);
void profitLoss(void);
void accountingMenu(void);

int addIncomeAuto(const char *category, float amount, const char *description) {
    Transaction t;
    strcpy(t.type, "INCOME");
    strcpy(t.category, category);
    t.amount = amount;
    getCurrentDate(t.date);
    strncpy(t.description, description, sizeof(t.description) - 1);
    t.description[sizeof(t.description) - 1] = '\0';
    return addTransaction(t);
}

int addExpenseAuto(const char *category, float amount, const char *description) {
    Transaction t;
    strcpy(t.type, "EXPENSE");
    strcpy(t.category, category);
    t.amount = amount;
    getCurrentDate(t.date);
    strncpy(t.description, description, sizeof(t.description) - 1);
    t.description[sizeof(t.description) - 1] = '\0';
    return addTransaction(t);
}

void addIncomeManual(void) {
    char category[30], desc[100];
    float amount;
    printf("\n--- Add Income ---\n");
    readString("Category (e.g. Other Income): ", category, sizeof(category));
    amount = readFloat("Amount: ");
    if (amount <= 0) { printf("Amount must be positive.\n"); return; }
    readString("Description: ", desc, sizeof(desc));
    int id = addIncomeAuto(category, amount, desc);
    pushUndoAccount("ACCOUNT_INCOME", id);
    printf("Income of %.2f BDT recorded successfully. (Transaction #%d)\n", amount, id);
}

void addExpenseManual(void) {
    char category[30], desc[100];
    float amount;
    printf("\n--- Add Expense ---\n");
    readString("Category (e.g. Utility Bill): ", category, sizeof(category));
    amount = readFloat("Amount: ");
    if (amount <= 0) { printf("Amount must be positive.\n"); return; }
    readString("Description: ", desc, sizeof(desc));
    int id = addExpenseAuto(category, amount, desc);
    pushUndoAccount("ACCOUNT_EXPENSE", id);
    printf("Expense of %.2f BDT recorded successfully. (Transaction #%d)\n", amount, id);
}

void viewTransactions(void) {
    printf("\n%-5s %-8s %-18s %-10s %-12s %-s\n", "ID", "Type", "Category", "Amount", "Date", "Description");
    printf("--------------------------------------------------------------------------------\n");
    Transaction *cur = transactionHead;
    if (cur == NULL) { printf("No transactions recorded yet.\n"); return; }
    while (cur != NULL) {
        printf("%-5d %-8s %-18s %-10.2f %-12s %-s\n", cur->id, cur->type, cur->category, cur->amount, cur->date, cur->description);
        cur = cur->next;
    }
}

void dailyAccounting(void) {
    char date[12];
    readString("Enter date (DD-MM-YYYY) or press Enter for today: ", date, sizeof(date));
    if (strlen(date) == 0) getCurrentDate(date);

    float income = 0, expense = 0;
    Transaction *cur = transactionHead;
    printf("\n--- Accounting for %s ---\n", date);
    while (cur != NULL) {
        if (strcmp(cur->date, date) == 0) {
            printf("[%s] %-18s %.2f BDT - %s\n", cur->type, cur->category, cur->amount, cur->description);
            if (strcmp(cur->type, "INCOME") == 0) income += cur->amount;
            else expense += cur->amount;
        }
        cur = cur->next;
    }
    printf("Total Income : %.2f BDT\n", income);
    printf("Total Expense: %.2f BDT\n", expense);
    printf("Net          : %.2f BDT\n", income - expense);
}

void monthlyAccounting(void) {
    int month, year;
    month = readInt("Enter month (1-12): ");
    year = readInt("Enter year (e.g. 2026): ");

    float income = 0, expense = 0;
    int count = 0;
    Transaction *cur = transactionHead;
    printf("\n--- Accounting for %02d-%04d ---\n", month, year);
    while (cur != NULL) {
        int d, m, y;
        sscanf(cur->date, "%d-%d-%d", &d, &m, &y);
        if (m == month && y == year) {
            count++;
            if (strcmp(cur->type, "INCOME") == 0) income += cur->amount;
            else expense += cur->amount;
        }
        cur = cur->next;
    }
    printf("Transactions found: %d\n", count);
    printf("Total Income : %.2f BDT\n", income);
    printf("Total Expense: %.2f BDT\n", expense);
    printf("Net Profit/Loss: %.2f BDT\n", income - expense);
}

float totalIncome(void) {
    float total = 0;
    Transaction *cur = transactionHead;
    while (cur != NULL) {
        if (strcmp(cur->type, "INCOME") == 0) total += cur->amount;
        cur = cur->next;
    }
    return total;
}

float totalExpense(void) {
    float total = 0;
    Transaction *cur = transactionHead;
    while (cur != NULL) {
        if (strcmp(cur->type, "EXPENSE") == 0) total += cur->amount;
        cur = cur->next;
    }
    return total;
}

void profitLoss(void) {
    float income = totalIncome();
    float expense = totalExpense();
    printf("\n--- Profit / Loss Summary ---\n");
    printf("Total Income : %.2f BDT\n", income);
    printf("Total Expense: %.2f BDT\n", expense);
    if (income - expense >= 0)
        printf("Net Profit   : %.2f BDT\n", income - expense);
    else
        printf("Net Loss     : %.2f BDT\n", expense - income);
}

void accountingMenu(void) {
    int choice;
    do {
        printf("\n======= ACCOUNTING MENU =======\n");
        printf("1. Add Income\n");
        printf("2. Add Expense\n");
        printf("3. View Transactions\n");
        printf("4. Daily Accounting\n");
        printf("5. Monthly Accounting\n");
        printf("6. Total Income\n");
        printf("7. Total Expense\n");
        printf("8. Profit/Loss\n");
        printf("9. Back to Main Menu\n");
        choice = readInt("Enter choice: ");
        if (feof(stdin)) { printf("\nInput stream closed.\n"); break; }
        switch (choice) {
            case 1: addIncomeManual(); break;
            case 2: addExpenseManual(); break;
            case 3: viewTransactions(); break;
            case 4: dailyAccounting(); break;
            case 5: monthlyAccounting(); break;
            case 6: printf("Total Income: %.2f BDT\n", totalIncome()); break;
            case 7: printf("Total Expense: %.2f BDT\n", totalExpense()); break;
            case 8: profitLoss(); break;
            case 9: printf("Returning to Main Menu...\n"); break;
            default: printf("Invalid choice. Try again.\n");
        }
    } while (choice != 9);
}
