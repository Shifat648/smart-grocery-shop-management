/*
    attendance.c

    This file manages employee attendance.

    Features:
    1. Mark Attendance
    2. Present
    3. Absent
    4. Late
    5. Leave
    6. View Today's Attendance
    7. Search Employee Attendance
    8. Monthly Attendance Report
    9. Attendance Percentage
    10. Manage Employees

    Attendance records are stored in the linked list
    from linkedlist.c.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/* =========================================================
   EMPLOYEE STRUCTURE
   ========================================================= */

typedef struct Employee
{
    char id[20];
    char name[50];
    char phone[15];
    char position[30];

    float salary;

    char joinDate[12];

    struct Employee *next;

} Employee;


/* =========================================================
   ATTENDANCE STRUCTURE
   ========================================================= */

typedef struct Attendance
{
    char empId[20];
    char empName[50];

    char date[12];
    char status[10];

    struct Attendance *next;

} Attendance;


/* =========================================================
   FUNCTIONS FROM OTHER FILES
   ========================================================= */

/* From linkedlist.c */

extern Attendance *attendanceHead;

extern void addAttendance(Attendance attendance);


/* From employee.c */

extern Employee *findEmployee(const char *id);

extern void employeeMenu(void);


/* From utils.c */

extern void readString(
    const char *message,
    char *text,
    int size
);

extern int readInt(const char *message);

extern void getCurrentDate(char *date);


/* =========================================================
   FUNCTION PROTOTYPES
   ========================================================= */

void markAttendanceStatus(const char *status);

void markAttendanceMenuOption(void);

void viewTodayAttendance(void);

void searchEmployeeAttendance(void);

void monthlyAttendanceReport(void);

void attendancePercentage(void);

void attendanceMenu(void);


/* =========================================================
   MARK ATTENDANCE
   =========================================================

   This function marks attendance for one employee.

   status = NULL
       → user can enter Present/Absent/Late/Leave

   status = "Present"
       → automatically marks Present

   status = "Absent"
       → automatically marks Absent

   status = "Late"
       → automatically marks Late

   status = "Leave"
       → automatically marks Leave
   ========================================================= */

void markAttendanceStatus(const char *status)
{
    char employeeID[20];

    char attendanceStatus[10];

    Employee *employee;

    Attendance attendance;


    /* Get employee ID */

    readString(
        "Enter Employee ID: ",
        employeeID,
        sizeof(employeeID)
    );


    /* Find employee */

    employee = findEmployee(employeeID);


    /* Employee not found */

    if (employee == NULL)
    {
        printf(
            "Employee with ID '%s' not found.\n",
            employeeID
        );

        return;
    }


    /* If status is already given */

    if (status != NULL)
    {
        strcpy(attendanceStatus, status);
    }

    /* Otherwise ask the user */

    else
    {
        printf(
            "\nStatus Options:\n"
        );

        printf("Present\n");
        printf("Absent\n");
        printf("Late\n");
        printf("Leave\n");


        readString(
            "Enter Status: ",
            attendanceStatus,
            sizeof(attendanceStatus)
        );


        /* Check status */

        if (
            strcmp(attendanceStatus, "Present") != 0 &&
            strcmp(attendanceStatus, "Absent") != 0 &&
            strcmp(attendanceStatus, "Late") != 0 &&
            strcmp(attendanceStatus, "Leave") != 0
        )
        {
            printf(
                "Invalid status.\n"
                "Use Present, Absent, Late or Leave.\n"
            );

            return;
        }
    }


    /* Store employee information */

    strcpy(
        attendance.empId,
        employee->id
    );

    strcpy(
        attendance.empName,
        employee->name
    );


    /* Get today's date */

    getCurrentDate(attendance.date);


    /* Store attendance status */

    strcpy(
        attendance.status,
        attendanceStatus
    );


    /* Add attendance to linked list */

    addAttendance(attendance);


    printf(
        "\nAttendance marked successfully.\n"
    );

    printf(
        "Employee : %s\n",
        employee->name
    );

    printf(
        "Status   : %s\n",
        attendanceStatus
    );

    printf(
        "Date     : %s\n",
        attendance.date
    );
}


/* =========================================================
   MARK ATTENDANCE MENU OPTION
   ========================================================= */

void markAttendanceMenuOption(void)
{
    printf(
        "\n========== MARK ATTENDANCE ==========\n"
    );

    /*
        NULL means the user will select
        the attendance status manually.
    */

    markAttendanceStatus(NULL);
}


/* =========================================================
   VIEW TODAY'S ATTENDANCE
   ========================================================= */

void viewTodayAttendance(void)
{
    char today[12];

    Attendance *current;

    int count = 0;


    /* Get today's date */

    getCurrentDate(today);


    printf(
        "\n========== TODAY'S ATTENDANCE ==========\n"
    );

    printf(
        "Date: %s\n\n",
        today
    );


    printf(
        "%-15s %-25s %-10s\n",
        "Employee ID",
        "Employee Name",
        "Status"
    );

    printf(
        "--------------------------------------------------\n"
    );


    /* Start from first attendance record */

    current = attendanceHead;


    /* Check all attendance records */

    while (current != NULL)
    {
        if (
            strcmp(current->date, today) == 0
        )
        {
            printf(
                "%-15s %-25s %-10s\n",
                current->empId,
                current->empName,
                current->status
            );

            count++;
        }


        current = current->next;
    }


    /* No records */

    if (count == 0)
    {
        printf(
            "No attendance marked for today.\n"
        );
    }
}


/* =========================================================
   SEARCH EMPLOYEE ATTENDANCE
   ========================================================= */

void searchEmployeeAttendance(void)
{
    char employeeID[20];

    Employee *employee;

    Attendance *current;

    int count = 0;


    printf(
        "\n========== SEARCH EMPLOYEE ATTENDANCE ==========\n"
    );


    /* Get employee ID */

    readString(
        "Enter Employee ID: ",
        employeeID,
        sizeof(employeeID)
    );


    /* Find employee */

    employee = findEmployee(employeeID);


    if (employee == NULL)
    {
        printf(
            "Employee with ID '%s' not found.\n",
            employeeID
        );

        return;
    }


    printf(
        "\nAttendance History\n"
    );

    printf(
        "Employee: %s\n",
        employee->name
    );

    printf(
        "ID: %s\n\n",
        employee->id
    );


    printf(
        "%-15s %-10s\n",
        "Date",
        "Status"
    );

    printf(
        "-------------------------\n"
    );


    /* Start from first record */

    current = attendanceHead;


    /* Search employee's records */

    while (current != NULL)
    {
        if (
            strcmp(current->empId, employeeID) == 0
        )
        {
            printf(
                "%-15s %-10s\n",
                current->date,
                current->status
            );

            count++;
        }


        current = current->next;
    }


    if (count == 0)
    {
        printf(
            "No attendance records found.\n"
        );
    }
}


/* =========================================================
   MONTHLY ATTENDANCE REPORT
   ========================================================= */

void monthlyAttendanceReport(void)
{
    char employeeID[20];

    int month;
    int year;

    int present = 0;
    int absent = 0;
    int late = 0;
    int leave = 0;

    int totalDays;

    Employee *employee;

    Attendance *current;


    printf(
        "\n========== MONTHLY ATTENDANCE REPORT ==========\n"
    );


    /* Get employee ID */

    readString(
        "Enter Employee ID: ",
        employeeID,
        sizeof(employeeID)
    );


    /* Find employee */

    employee = findEmployee(employeeID);


    if (employee == NULL)
    {
        printf(
            "Employee with ID '%s' not found.\n",
            employeeID
        );

        return;
    }


    /* Get month */

    month = readInt(
        "Enter Month (1-12): "
    );


    /* Get year */

    year = readInt(
        "Enter Year: "
    );


    /* Start from first attendance */

    current = attendanceHead;


    /* Check all records */

    while (current != NULL)
    {
        int day;
        int recordMonth;
        int recordYear;


        /* Check employee */

        if (
            strcmp(current->empId, employeeID) == 0
        )
        {
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
                &recordMonth,
                &recordYear
            );


            /* Check month and year */

            if (
                recordMonth == month &&
                recordYear == year
            )
            {
                if (
                    strcmp(
                        current->status,
                        "Present"
                    ) == 0
                )
                {
                    present++;
                }

                else if (
                    strcmp(
                        current->status,
                        "Absent"
                    ) == 0
                )
                {
                    absent++;
                }

                else if (
                    strcmp(
                        current->status,
                        "Late"
                    ) == 0
                )
                {
                    late++;
                }

                else if (
                    strcmp(
                        current->status,
                        "Leave"
                    ) == 0
                )
                {
                    leave++;
                }
            }
        }


        current = current->next;
    }


    /* Calculate total recorded days */

    totalDays =
        present +
        absent +
        late +
        leave;


    /* Show report */

    printf(
        "\n========== MONTHLY REPORT ==========\n"
    );

    printf(
        "Employee : %s\n",
        employee->name
    );

    printf(
        "ID       : %s\n",
        employee->id
    );

    printf(
        "Month    : %02d-%04d\n\n",
        month,
        year
    );


    printf(
        "Present Days : %d\n",
        present
    );

    printf(
        "Absent Days  : %d\n",
        absent
    );

    printf(
        "Late Days    : %d\n",
        late
    );

    printf(
        "Leave Days   : %d\n",
        leave
    );


    /* Calculate percentage */

    if (totalDays > 0)
    {
        float percentage;

        /*
            Present and Late are counted
            as attended days.
        */

        percentage =
            (present + late) * 100.0 / totalDays;


        printf(
            "Attendance Percentage: %.2f%%\n",
            percentage
        );
    }

    else
    {
        printf(
            "No attendance records found "
            "for this month.\n"
        );
    }
}


/* =========================================================
   ATTENDANCE PERCENTAGE
   ========================================================= */

void attendancePercentage(void)
{
    char employeeID[20];

    int presentDays = 0;
    int totalDays = 0;

    float percentage;

    Employee *employee;

    Attendance *current;


    printf(
        "\n========== ATTENDANCE PERCENTAGE ==========\n"
    );


    /* Get employee ID */

    readString(
        "Enter Employee ID: ",
        employeeID,
        sizeof(employeeID)
    );


    /* Find employee */

    employee = findEmployee(employeeID);


    if (employee == NULL)
    {
        printf(
            "Employee with ID '%s' not found.\n",
            employeeID
        );

        return;
    }


    /* Start from first attendance */

    current = attendanceHead;


    /* Check all records */

    while (current != NULL)
    {
        if (
            strcmp(
                current->empId,
                employeeID
            ) == 0
        )
        {
            totalDays++;


            /*
                Present and Late both count
                as attended days.
            */

            if (
                strcmp(
                    current->status,
                    "Present"
                ) == 0 ||
                strcmp(
                    current->status,
                    "Late"
                ) == 0
            )
            {
                presentDays++;
            }
        }


        current = current->next;
    }


    /* No records */

    if (totalDays == 0)
    {
        printf(
            "No attendance records found "
            "for %s.\n",
            employee->name
        );

        return;
    }


    /* Calculate percentage */

    percentage =
        (presentDays * 100.0) / totalDays;


    printf(
        "\nEmployee: %s\n",
        employee->name
    );

    printf(
        "Total Working Days Recorded: %d\n",
        totalDays
    );

    printf(
        "Attended Days: %d\n",
        presentDays
    );

    printf(
        "Attendance Percentage: %.2f%%\n",
        percentage
    );


    printf(
        "\nFormula:\n"
    );

    printf(
        "Attendance Percentage = "
        "(Attended Days / Total Days) x 100\n"
    );

    printf(
        "= (%d / %d) x 100\n",
        presentDays,
        totalDays
    );
}


/* =========================================================
   ATTENDANCE MANAGEMENT MENU
   ========================================================= */

void attendanceMenu(void)
{
    int choice;


    do
    {
        printf(
            "\n============================================\n"
        );

        printf(
            "       ATTENDANCE MANAGEMENT MENU\n"
        );

        printf(
            "============================================\n"
        );


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

      choice = readInt(
    "Enter choice: "
    );

        /* Stop if input stream closed (e.g. EOF from piped input) */
    if (feof(stdin))
 {
    printf("\nInput stream closed.\n");
    break;
       }

       /* Perform selected operation */

         switch (choice)
        {
            case 1:

                markAttendanceMenuOption();

                break;


            case 2:

                printf(
                    "\n========== MARK PRESENT ==========\n"
                );

                markAttendanceStatus("Present");

                break;


            case 3:

                printf(
                    "\n========== MARK ABSENT ==========\n"
                );

                markAttendanceStatus("Absent");

                break;


            case 4:

                printf(
                    "\n========== MARK LATE ==========\n"
                );

                markAttendanceStatus("Late");

                break;


            case 5:

                printf(
                    "\n========== MARK LEAVE ==========\n"
                );

                markAttendanceStatus("Leave");

                break;


            case 6:

                viewTodayAttendance();

                break;


            case 7:

                searchEmployeeAttendance();

                break;


            case 8:

                monthlyAttendanceReport();

                break;


            case 9:

                attendancePercentage();

                break;


            case 10:

                employeeMenu();

                break;


            case 11:

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
    while (choice != 11);
}
