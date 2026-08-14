/* ============================================================
   attendance.c
   Implements main menu option 12: Attendance Management
   Uses a Linked List (attendanceHead, defined in linkedlist.c)
   to keep the full attendance history of every employee.
   Employee management (Add/List/Pay Salary) is reached from
   this same submenu since employees must exist before their
   attendance can be marked.
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

typedef struct Attendance {
    char empId[20];
    char empName[50];
    char date[12];
    char status[10];
    struct Attendance *next;
} Attendance;

/* externs from linkedlist.c */
extern Attendance *attendanceHead;
extern void addAttendance(Attendance a);

/* externs from employee.c */
extern Employee* findEmployee(const char *id);
extern void employeeMenu(void);

/* externs from utils.c */
extern void readString(const char *prompt, char *buf, int size);
extern int readInt(const char *prompt);
extern void getCurrentDate(char *buf);

/* ---- prototypes ---- */
void markAttendanceStatus(const char *presetStatus);
void markAttendanceMenuOption(void);
void viewTodayAttendance(void);
void searchEmployeeAttendance(void);
void monthlyAttendanceReport(void);
void attendancePercentage(void);
void attendanceMenu(void);

/* Core function: marks one attendance record for a given employee & status */
void markAttendanceStatus(const char *presetStatus) {
    char id[20];
    readString("Enter Employee ID: ", id, sizeof(id));
    Employee *e = findEmployee(id);
    if (e == NULL) { printf("Error: Employee with ID '%s' not found.\n", id); return; }

    char status[10];
    if (presetStatus != NULL) {
        strcpy(status, presetStatus);
    } else {
        printf("Status options: Present / Absent / Late / Leave\n");
        readString("Enter Status: ", status, sizeof(status));
        if (strcmp(status, "Present") != 0 && strcmp(status, "Absent") != 0 &&
            strcmp(status, "Late") != 0 && strcmp(status, "Leave") != 0) {
            printf("Invalid status. Must be Present, Absent, Late or Leave.\n");
            return;
        }
    }

    Attendance a;
    strcpy(a.empId, e->id);
    strcpy(a.empName, e->name);
    getCurrentDate(a.date);
    strcpy(a.status, status);
    addAttendance(a);

    printf("Attendance marked: %s - %s on %s\n", e->name, status, a.date);
}

void markAttendanceMenuOption(void) {
    printf("\n--- Mark Attendance ---\n");
    markAttendanceStatus(NULL);
}

void viewTodayAttendance(void) {
    char today[12];
    getCurrentDate(today);
    printf("\n--- Today's Attendance (%s) ---\n", today);
    Attendance *cur = attendanceHead;
    int count = 0;
    while (cur != NULL) {
        if (strcmp(cur->date, today) == 0) {
            printf("%-10s %-20s %-10s\n", cur->empId, cur->empName, cur->status);
            count++;
        }
        cur = cur->next;
    }
    if (count == 0) printf("No attendance marked for today yet.\n");
}

void searchEmployeeAttendance(void) {
    char id[20];
    printf("\n--- Search Employee Attendance ---\n");
    readString("Enter Employee ID: ", id, sizeof(id));
    Employee *e = findEmployee(id);
    if (e == NULL) { printf("Employee with ID '%s' not found.\n", id); return; }

    printf("Attendance history for %s (%s):\n", e->name, e->id);
    Attendance *cur = attendanceHead;
    int count = 0;
    while (cur != NULL) {
        if (strcmp(cur->empId, id) == 0) {
            printf("%-12s : %s\n", cur->date, cur->status);
            count++;
        }
        cur = cur->next;
    }
    if (count == 0) printf("No attendance records found for this employee.\n");
}

void monthlyAttendanceReport(void) {
    char id[20];
    int month, year;
    printf("\n--- Monthly Attendance Report ---\n");
    readString("Enter Employee ID: ", id, sizeof(id));
    Employee *e = findEmployee(id);
    if (e == NULL) { printf("Employee with ID '%s' not found.\n", id); return; }
    month = readInt("Enter Month (1-12): ");
    year = readInt("Enter Year (e.g. 2026): ");

    int present = 0, absent = 0, late = 0, leave = 0;
    Attendance *cur = attendanceHead;
    while (cur != NULL) {
        if (strcmp(cur->empId, id) == 0) {
            int d, m, y;
            sscanf(cur->date, "%d-%d-%d", &d, &m, &y);
            if (m == month && y == year) {
                if (strcmp(cur->status, "Present") == 0) present++;
                else if (strcmp(cur->status, "Absent") == 0) absent++;
                else if (strcmp(cur->status, "Late") == 0) late++;
                else if (strcmp(cur->status, "Leave") == 0) leave++;
            }
        }
        cur = cur->next;
    }

    int totalDays = present + absent + late + leave;
    printf("\nEmployee: %s (%s) - %02d-%04d\n", e->name, e->id, month, year);
    printf("Present Days : %d\n", present);
    printf("Absent Days  : %d\n", absent);
    printf("Late Days    : %d\n", late);
    printf("Leave Days   : %d\n", leave);
    if (totalDays > 0)
        printf("Attendance Percentage: %.2f%%\n", ((present + late) * 100.0) / totalDays);
    else
        printf("No attendance records found for this month.\n");
}

void attendancePercentage(void) {
    char id[20];
    printf("\n--- Attendance Percentage ---\n");
    readString("Enter Employee ID: ", id, sizeof(id));
    Employee *e = findEmployee(id);
    if (e == NULL) { printf("Employee with ID '%s' not found.\n", id); return; }

    int present = 0, total = 0;
    Attendance *cur = attendanceHead;
    while (cur != NULL) {
        if (strcmp(cur->empId, id) == 0) {
            total++;
            if (strcmp(cur->status, "Present") == 0 || strcmp(cur->status, "Late") == 0) present++;
        }
        cur = cur->next;
    }

    if (total == 0) {
        printf("No attendance records found for %s.\n", e->name);
        return;
    }
    printf("Employee: %s | Total Working Days Recorded: %d\n", e->name, total);
    printf("Attendance Percentage = (%d / %d) x 100 = %.2f%%\n", present, total, (present * 100.0) / total);
}

void attendanceMenu(void) {
    int choice;
    do {
        printf("\n======= ATTENDANCE MANAGEMENT MENU =======\n");
        printf("1. Mark Attendance\n");
        printf("2. Present\n");
        printf("3. Absent\n");
        printf("4. Late\n");
        printf("5. Leave\n");
        printf("6. View Today's Attendance\n");
        printf("7. Search Employee Attendance\n");
        printf("8. Monthly Attendance Report\n");
        printf("9. Attendance Percentage\n");
        printf("10. Manage Employees (Add/List/Pay Salary)\n");
        printf("11. Back to Main Menu\n");
        choice = readInt("Enter choice: ");
        if (feof(stdin)) { printf("\nInput stream closed.\n"); break; }
        switch (choice) {
            case 1: markAttendanceMenuOption(); break;
            case 2: printf("\n--- Quick Mark: Present ---\n"); markAttendanceStatus("Present"); break;
            case 3: printf("\n--- Quick Mark: Absent ---\n"); markAttendanceStatus("Absent"); break;
            case 4: printf("\n--- Quick Mark: Late ---\n"); markAttendanceStatus("Late"); break;
            case 5: printf("\n--- Quick Mark: Leave ---\n"); markAttendanceStatus("Leave"); break;
            case 6: viewTodayAttendance(); break;
            case 7: searchEmployeeAttendance(); break;
            case 8: monthlyAttendanceReport(); break;
            case 9: attendancePercentage(); break;
            case 10: employeeMenu(); break;
            case 11: printf("Returning to Main Menu...\n"); break;
            default: printf("Invalid choice. Try again.\n");
        }
    } while (choice != 11);
}
