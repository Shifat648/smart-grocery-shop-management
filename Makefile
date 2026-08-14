# ============================================================
# Makefile - Smart Grocery Shop Management System
# ============================================================

CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = smartmart
SOURCES = main.c product.c avl.c stack.c linkedlist.c sales.c \
          accounting.c employee.c attendance.c reports.c file.c utils.c

all: $(TARGET)

$(TARGET): $(SOURCES)
	$(CC) $(CFLAGS) $(SOURCES) -o $(TARGET)
	@mkdir -p data

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

clean-data:
	rm -f data/*.dat

.PHONY: all run clean clean-data
