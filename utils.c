/* ============================================================
   utils.c
   Small helper functions shared across the project:
     - Safe console input (no gets(), uses fgets())
     - Date parsing / comparison  (format: DD-MM-YYYY)
   ============================================================ */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* ---- prototypes ---- */
void readString(const char *prompt, char *buf, int size);
int readInt(const char *prompt);
float readFloat(const char *prompt);
void trimNewline(char *s);
void getCurrentDate(char *buf);
int isValidDateFormat(const char *date);
int compareDates(const char *d1, const char *d2);
int isDateBeforeToday(const char *date);
int daysBetweenAndToday(const char *date);
void flushInputLine(void);

void trimNewline(char *s) {
    size_t len = strlen(s);
    if (len > 0 && (s[len - 1] == '\n' || s[len - 1] == '\r'))
        s[len - 1] = '\0';
    if (len > 1 && s[len - 2] == '\r')
        s[len - 2] = '\0';
}

/* Reads a full line safely (fgets) so buffer overflow / gets() is avoided */
void readString(const char *prompt, char *buf, int size) {
    if (prompt != NULL) printf("%s", prompt);
    if (fgets(buf, size, stdin) == NULL) {
        buf[0] = '\0';
        return;
    }
    /* if the line was longer than the buffer, drain the rest of stdin.
       This check MUST happen before the newline is stripped, otherwise
       every normal-length line would be mistaken for an overflow. */
    if (strchr(buf, '\n') == NULL) {
        int c;
        while ((c = getchar()) != '\n' && c != EOF) { }
    }
    trimNewline(buf);
}

void flushInputLine(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}

int readInt(const char *prompt) {
    char buf[64];
    int value;
    char extra;
    while (1) {
        printf("%s", prompt);
        if (fgets(buf, sizeof(buf), stdin) == NULL) return 0;
        if (sscanf(buf, "%d %c", &value, &extra) == 1) {
            return value;
        }
        printf("Invalid number. Please enter a valid integer.\n");
    }
}

float readFloat(const char *prompt) {
    char buf[64];
    float value;
    char extra;
    while (1) {
        printf("%s", prompt);
        if (fgets(buf, sizeof(buf), stdin) == NULL) return 0.0f;
        if (sscanf(buf, "%f %c", &value, &extra) == 1) {
            return value;
        }
        printf("Invalid number. Please enter a valid decimal value.\n");
    }
}

void getCurrentDate(char *buf) {
    time_t t = time(NULL);
    struct tm *tm_info = localtime(&t);
    sprintf(buf, "%02d-%02d-%04d", tm_info->tm_mday, tm_info->tm_mon + 1, tm_info->tm_year + 1900);
}

int isValidDateFormat(const char *date) {
    int d, m, y;
    if (strlen(date) != 10) return 0;
    if (date[2] != '-' || date[5] != '-') return 0;
    if (sscanf(date, "%d-%d-%d", &d, &m, &y) != 3) return 0;
    if (m < 1 || m > 12 || d < 1 || d > 31 || y < 2000 || y > 2100) return 0;
    return 1;
}

/* Returns <0 if d1 is earlier than d2, 0 if equal, >0 if later */
int compareDates(const char *d1, const char *d2) {
    int d1d, d1m, d1y, d2d, d2m, d2y;
    sscanf(d1, "%d-%d-%d", &d1d, &d1m, &d1y);
    sscanf(d2, "%d-%d-%d", &d2d, &d2m, &d2y);
    if (d1y != d2y) return d1y - d2y;
    if (d1m != d2m) return d1m - d2m;
    return d1d - d2d;
}

int isDateBeforeToday(const char *date) {
    char today[12];
    getCurrentDate(today);
    return compareDates(date, today) < 0;
}

/* Positive => date is in the future (days remaining), negative => already past */
int daysBetweenAndToday(const char *date) {
    int d, m, y;
    sscanf(date, "%d-%d-%d", &d, &m, &y);
    struct tm target = {0};
    target.tm_mday = d;
    target.tm_mon = m - 1;
    target.tm_year = y - 1900;
    target.tm_hour = 12;

    time_t targetTime = mktime(&target);
    time_t now = time(NULL);
    struct tm *nowTmPtr = localtime(&now);
    struct tm nowTm = *nowTmPtr;
    nowTm.tm_hour = 12; nowTm.tm_min = 0; nowTm.tm_sec = 0;
    time_t nowNoon = mktime(&nowTm);

    double diffSeconds = difftime(targetTime, nowNoon);
    return (int)(diffSeconds / (60 * 60 * 24));
}
