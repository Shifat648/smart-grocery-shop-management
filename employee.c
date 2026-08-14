/* ============================================================
   employee.c
   Employee records (linked list) and salary payment.
   Paying a salary automatically creates an Expense transaction
   (feature required by the Accounting module).
   ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Employee {
    char id[20];
    char name[50];
    char phone[15];
    char position[30];
    float salary;
    char joinDate[12];
    struct Employee *next;
} Employee;

Employee *employeeHead = NULL;

/* externs from accounting.c */
extern int addExpenseAuto(const char *category, float amount, const char *description);
extern void pushUndoAccount(const char *opType, int transactionId);

/* externs from utils.c */
extern void readString(const char *prompt, char *buf, int size);
extern float readFloat(const char *prompt);
extern void getCurrentDate(char *buf);
extern int isValidDateFormat(const char *date);

/* ---- prototypes ---- */
void addEmployee(void);
Employee* findEmployee(const char *id);
void listEmployees(void);
void paySalary(void);
void employeeMenu(void);
void freeEmployeeList(void);

Employee* findEmployee(const char *id) {
    Employee *cur = employeeHead;
    while (cur != NULL) {
        if (strcmp(cur->id, id) == 0) return cur;
        cur = cur->next;
    }
    return NULL;
}

void addEmployee(void) {
    Employee e;
    memset(&e, 0, sizeof(e));

    printf("\n--- Add Employee ---\n");
    readString("Employee ID: ", e.id, sizeof(e.id));
    if (findEmployee(e.id) != NULL) {
        printf("Error: Employee ID '%s' already exists!\n", e.id);
        return;
    }
    readString("Employee Name: ", e.name, sizeof(e.name));
    readString("Phone Number: ", e.phone, sizeof(e.phone));
    readString("Position: ", e.position, sizeof(e.position));
    e.salary = readFloat("Salary: ");
    do {
        readString("Joining Date (DD-MM-YYYY): ", e.joinDate, sizeof(e.joinDate));
        if (feof(stdin)) { printf("Input ended. Add Employee cancelled.\n"); return; }
        if (!isValidDateFormat(e.joinDate)) printf("Invalid date format. Use DD-MM-YYYY.\n");
    } while (!isValidDateFormat(e.joinDate));

    Employee *node = (Employee*)malloc(sizeof(Employee));
    if (node == NULL) { printf("Memory allocation failed!\n"); return; }
    *node = e;
    node->next = employeeHead;
    employeeHead = node;
    printf("Employee '%s' added successfully!\n", e.name);
}

void listEmployees(void) {
    printf("\n%-10s %-20s %-15s %-15s %-10s %-12s\n", "ID", "Name", "Phone", "Position", "Salary", "Join Date");
    printf("--------------------------------------------------------------------------------\n");
    Employee *cur = employeeHead;
    if (cur == NULL) { printf("No employees found.\n"); return; }
    while (cur != NULL) {
        printf("%-10s %-20s %-15s %-15s %-10.2f %-12s\n", cur->id, cur->name, cur->phone, cur->position, cur->salary, cur->joinDate);
        cur = cur->next;
    }
}

void paySalary(void) {
    char id[20];
    printf("\n--- Pay Employee Salary ---\n");
    readString("Enter Employee ID: ", id, sizeof(id));
    Employee *e = findEmployee(id);
    if (e == NULL) { printf("Employee with ID '%s' not found.\n", id); return; }

    char desc[100];
    sprintf(desc, "Salary payment to %s (%s)", e->name, e->id);
    int txId = addExpenseAuto("Employee Salary", e->salary, desc);
    pushUndoAccount("SALARY", txId);

    printf("Salary of %.2f BDT paid to %s. Recorded as Expense.\n", e->salary, e->name);
}

void employeeMenu(void) {
    int choice;
    do {
        printf("\n======= EMPLOYEE MENU =======\n");
        printf("1. Add Employee\n");
        printf("2. List Employees\n");
        printf("3. Pay Salary\n");
        printf("4. Back to Main Menu\n");
        printf("Enter choice: ");
        char buf[16];
        if (fgets(buf, sizeof(buf), stdin) == NULL) { printf("\nInput stream closed.\n"); break; }
        choice = atoi(buf);
        switch (choice) {
            case 1: addEmployee(); break;
            case 2: listEmployees(); break;
            case 3: paySalary(); break;
            case 4: printf("Returning to Main Menu...\n"); break;
            default: printf("Invalid choice. Try again.\n");
        }
    } while (choice != 4);
}

void freeEmployeeList(void) {
    Employee *cur = employeeHead;
    while (cur != NULL) { Employee *tmp = cur; cur = cur->next; free(tmp); }
    employeeHead = NULL;
}
