/*
    accounting.c

    This file manages all money transactions of the shop.

    Features:
    1. Add Income
    2. Add Expense
    3. Automatic Income from Sales
    4. Automatic Expense from Restocking
    5. Automatic Expense from Employee Salary
    6. View Transactions
    7. Daily Accounting
    8. Monthly Accounting
    9. Total Income
    10. Total Expense
    11. Profit / Loss
    12. Undo Account Transaction

    Transactions are stored in the linked list from linkedlist.c.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* ================= TRANSACTION STRUCTURE ================= */

typedef struct Transaction {

    int id;

    char type[10];          // INCOME or EXPENSE
    char category[30];      // Sales, Restocking, Salary, etc.

    float amount;

    char date[12];

    char description[100];

    struct Transaction *next;

} Transaction;


/* ================= EXTERNAL FUNCTIONS ================= */

/*
    These variables/functions are created in other files.
*/

/* From linkedlist.c */
extern Transaction *transactionHead;
extern int addTransaction(Transaction t);
extern int removeTransactionById(int id);


/* From stack.c */
extern void pushUndoAccount(const char *operation, int transactionID);


/* From utils.c */
extern void readString(const char *message, char *text, int size);
extern float readFloat(const char *message);
extern int readInt(const char *message);
extern void getCurrentDate(char *date);


/* ================= FUNCTION PROTOTYPES ================= */

/* Automatic transactions */
int addIncomeAuto(const char *category, float amount,
                  const char *description);

int addExpenseAuto(const char *category, float amount,
                   const char *description);


/* Manual transactions */
void addIncomeManual(void);
void addExpenseManual(void);


/* Reports */
void viewTransactions(void);
void dailyAccounting(void);
void monthlyAccounting(void);

float totalIncome(void);
float totalExpense(void);

void profitLoss(void);


/* Menu */
void accountingMenu(void);


/* =========================================================
   ADD AUTOMATIC INCOME
   Used by Sales module.
   Example:
   Customer buys products for 500 BDT
   -> 500 BDT is automatically added as income.
   ========================================================= */

int addIncomeAuto(const char *category,
                  float amount,
                  const char *description)
{
    Transaction transaction;

    /* Set transaction type */
    strcpy(transaction.type, "INCOME");

    /* Set category */
    strcpy(transaction.category, category);

    /* Set amount */
    transaction.amount = amount;

    /* Get today's date */
    getCurrentDate(transaction.date);

    /* Add description */
    strncpy(transaction.description,
            description,
            sizeof(transaction.description) - 1);

    transaction.description[
        sizeof(transaction.description) - 1
    ] = '\0';


    /* Add transaction to linked list */
    return addTransaction(transaction);
}


/* =========================================================
   ADD AUTOMATIC EXPENSE
   Used by Restocking and Employee modules.

   Example:
   Restocking costs 1000 BDT
   -> 1000 BDT is automatically added as expense.
   ========================================================= */

int addExpenseAuto(const char *category,
                   float amount,
                   const char *description)
{
    Transaction transaction;

    /* Set transaction type */
    strcpy(transaction.type, "EXPENSE");

    /* Set category */
    strcpy(transaction.category, category);

    /* Set amount */
    transaction.amount = amount;

    /* Get today's date */
    getCurrentDate(transaction.date);

    /* Add description */
    strncpy(transaction.description,
            description,
            sizeof(transaction.description) - 1);

    transaction.description[
        sizeof(transaction.description) - 1
    ] = '\0';


    /* Add transaction to linked list */
    return addTransaction(transaction);
}


/* =========================================================
   MANUAL ADD INCOME
   ========================================================= */

void addIncomeManual(void)
{
    char category[30];
    char description[100];

    float amount;

    int transactionID;


    printf("\n========== ADD INCOME ==========\n");


    /* Get category */
    readString(
        "Enter category: ",
        category,
        sizeof(category)
    );


    /* Get amount */
    amount = readFloat("Enter amount: ");


    /* Check amount */
    if (amount <= 0)
    {
        printf("Amount must be greater than 0.\n");
        return;
    }


    /* Get description */
    readString(
        "Enter description: ",
        description,
        sizeof(description)
    );


    /* Add income */
    transactionID = addIncomeAuto(
        category,
        amount,
        description
    );


    /* Save operation for Undo */
    pushUndoAccount(
        "ACCOUNT_INCOME",
        transactionID
    );


    printf(
        "Income added successfully.\n"
        "Amount: %.2f BDT\n"
        "Transaction ID: %d\n",
        amount,
        transactionID
    );
}


/* =========================================================
   MANUAL ADD EXPENSE
   ========================================================= */

void addExpenseManual(void)
{
    char category[30];
    char description[100];

    float amount;

    int transactionID;


    printf("\n========== ADD EXPENSE ==========\n");


    /* Get category */
    readString(
        "Enter category: ",
        category,
        sizeof(category)
    );


    /* Get amount */
    amount = readFloat("Enter amount: ");


    /* Check amount */
    if (amount <= 0)
    {
        printf("Amount must be greater than 0.\n");
        return;
    }


    /* Get description */
    readString(
        "Enter description: ",
        description,
        sizeof(description)
    );


    /* Add expense */
    transactionID = addExpenseAuto(
        category,
        amount,
        description
    );


    /* Save operation for Undo */
    pushUndoAccount(
        "ACCOUNT_EXPENSE",
        transactionID
    );


    printf(
        "Expense added successfully.\n"
        "Amount: %.2f BDT\n"
        "Transaction ID: %d\n",
        amount,
        transactionID
    );
}


/* =========================================================
   VIEW ALL TRANSACTIONS
   ========================================================= */

void viewTransactions(void)
{
    Transaction *current;

    current = transactionHead;


    printf("\n================ ALL TRANSACTIONS ================\n");

    printf(
        "%-5s %-8s %-18s %-10s %-12s %s\n",
        "ID",
        "Type",
        "Category",
        "Amount",
        "Date",
        "Description"
    );

    printf(
        "--------------------------------------------------------------------------\n"
    );


    /* No transaction */
    if (current == NULL)
    {
        printf("No transactions found.\n");
        return;
    }


    /* Show every transaction */
    while (current != NULL)
    {
        printf(
            "%-5d %-8s %-18s %-10.2f %-12s %s\n",
            current->id,
            current->type,
            current->category,
            current->amount,
            current->date,
            current->description
        );

        current = current->next;
    }
}


/* =========================================================
   DAILY ACCOUNTING
   ========================================================= */

void dailyAccounting(void)
{
    char date[12];

    float income = 0;
    float expense = 0;

    Transaction *current;


    printf("\n========== DAILY ACCOUNTING ==========\n");


    /* Ask for date */
    readString(
        "Enter date (DD-MM-YYYY) or press Enter for today: ",
        date,
        sizeof(date)
    );


    /* If user enters nothing, use today's date */
    if (strlen(date) == 0)
    {
        getCurrentDate(date);
    }


    printf("\nAccounting for: %s\n", date);


    current = transactionHead;


    /* Check all transactions */
    while (current != NULL)
    {
        /* Check whether date matches */
        if (strcmp(current->date, date) == 0)
        {
            printf(
                "[%s] %s - %.2f BDT - %s\n",
                current->type,
                current->category,
                current->amount,
                current->description
            );


            /* Add income */
            if (strcmp(current->type, "INCOME") == 0)
            {
                income = income + current->amount;
            }

            /* Add expense */
            else
            {
                expense = expense + current->amount;
            }
        }


        current = current->next;
    }


    /* Show result */
    printf("\nTotal Income : %.2f BDT\n", income);
    printf("Total Expense: %.2f BDT\n", expense);
    printf("Net Result   : %.2f BDT\n", income - expense);
}


/* =========================================================
   MONTHLY ACCOUNTING
   ========================================================= */

void monthlyAccounting(void)
{
    int month;
    int year;

    float income = 0;
    float expense = 0;

    int transactionCount = 0;

    Transaction *current;


    printf("\n========== MONTHLY ACCOUNTING ==========\n");


    /* Get month and year */
    month = readInt("Enter month (1-12): ");
    year = readInt("Enter year: ");


    printf(
        "\nAccounting for %02d-%04d\n",
        month,
        year
    );


    current = transactionHead;


    /* Check all transactions */
    while (current != NULL)
    {
        int day;
        int transactionMonth;
        int transactionYear;


        /*
            Convert date:

            DD-MM-YYYY

            Example:
            14-08-2026
        */

        sscanf(
            current->date,
            "%d-%d-%d",
            &day,
            &transactionMonth,
            &transactionYear
        );


        /* Check month and year */
        if (
            transactionMonth == month &&
            transactionYear == year
        )
        {
            transactionCount++;


            /* Add income */
            if (
                strcmp(current->type, "INCOME") == 0
            )
            {
                income = income + current->amount;
            }


            /* Add expense */
            else
            {
                expense = expense + current->amount;
            }
        }


        current = current->next;
    }


    /* Show result */
    printf(
        "Transactions found: %d\n",
        transactionCount
    );

    printf(
        "Total Income : %.2f BDT\n",
        income
    );

    printf(
        "Total Expense: %.2f BDT\n",
        expense
    );

    printf(
        "Net Profit/Loss: %.2f BDT\n",
        income - expense
    );
}


/* =========================================================
   CALCULATE TOTAL INCOME
   ========================================================= */

float totalIncome(void)
{
    float total = 0;

    Transaction *current;

    current = transactionHead;


    while (current != NULL)
    {
        /* Only count income */
        if (
            strcmp(current->type, "INCOME") == 0
        )
        {
            total = total + current->amount;
        }

        current = current->next;
    }


    return total;
}


/* =========================================================
   CALCULATE TOTAL EXPENSE
   ========================================================= */

float totalExpense(void)
{
    float total = 0;

    Transaction *current;

    current = transactionHead;


    while (current != NULL)
    {
        /* Only count expense */
        if (
            strcmp(current->type, "EXPENSE") == 0
        )
        {
            total = total + current->amount;
        }

        current = current->next;
    }


    return total;
}


/* =========================================================
   PROFIT / LOSS
   ========================================================= */

void profitLoss(void)
{
    float income;
    float expense;
    float result;


    /* Get total income */
    income = totalIncome();


    /* Get total expense */
    expense = totalExpense();


    /* Calculate profit/loss */
    result = income - expense;


    printf("\n========== PROFIT / LOSS ==========\n");

    printf(
        "Total Income : %.2f BDT\n",
        income
    );

    printf(
        "Total Expense: %.2f BDT\n",
        expense
    );


    /* Profit */
    if (result >= 0)
    {
        printf(
            "Net Profit   : %.2f BDT\n",
            result
        );
    }

    /* Loss */
    else
    {
        printf(
            "Net Loss     : %.2f BDT\n",
            -result
        );
    }
}


/* =========================================================
   ACCOUNTING MENU
   ========================================================= */

void accountingMenu(void)
{
    int choice;


    do
    {
        printf("\n====================================\n");
        printf("          ACCOUNTING MENU\n");
        printf("====================================\n");

        printf("1. Add Income\n");
        printf("2. Add Expense\n");
        printf("3. View Transactions\n");
        printf("4. Daily Accounting\n");
        printf("5. Monthly Accounting\n");
        printf("6. Total Income\n");
        printf("7. Total Expense\n");
        printf("8. Profit/Loss\n");
        printf("9. Back to Main Menu\n");

/* Get user choice */
        choice = readInt("Enter choice: ");

        /* Stop if input stream closed (e.g. EOF from piped input) */
        if (feof(stdin))
        {
            printf("\nInput stream closed.\n");
            break;
        }

        /* Perform selected operation */
        switch (choice)
        
        {  case 1:
                addIncomeManual();
                break;


            case 2:
                addExpenseManual();
                break;


            case 3:
                viewTransactions();
                break;


            case 4:
                dailyAccounting();
                break;


            case 5:
                monthlyAccounting();
                break;


            case 6:
                printf(
                    "\nTotal Income: %.2f BDT\n",
                    totalIncome()
                );
                break;


            case 7:
                printf(
                    "\nTotal Expense: %.2f BDT\n",
                    totalExpense()
                );
                break;


            case 8:
                profitLoss();
                break;


            case 9:
                printf(
                    "Returning to Main Menu...\n"
                );
                break;


            default:
                printf(
                    "Invalid choice. Please try again.\n"
                );
        }

    }
    while (choice != 9);
}
