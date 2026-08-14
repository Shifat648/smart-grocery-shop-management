# 🛒 Smart Grocery Shop Management System

### A Data Structures & Algorithms University Team Project (Pure C)

A complete console-based grocery shop management system written in **pure C**, developed as a **Data Structures & Algorithms university team project**.

The system manages products, sales, purchases, accounting, employees, attendance, reports, and data persistence while demonstrating the practical use of **AVL Trees, Stacks, and Linked Lists**.

---

## 👥 Team Members

This project was developed collaboratively by a team of **3 members**.

| Role            | Name                     | ID              | Contribution                                                                                                      |
| --------------- | ------------------------ | --------------- | ------------------------------------------------------------------------------------------------------------------ |
| **Team Lead**   | **Md Shifat Uz Zaman**   | —                | Project planning, system architecture, core development, integration, testing, debugging, and final documentation |
| **Team Member** | **Nusrat Jahan Laboni**  | 253-15-408       | Module development, testing, and documentation                                                                    |
| **Team Member** | **Mymona Ethina**        | 253-15-618      | Module development, testing, and documentation                                                                    |


## ✨ Features

| Feature                       | Description                                                              |
| :---------------------------- | :----------------------------------------------------------------------- |
| 📦 **Product Management**     | Add, search, update, delete, and display products.                       |
| 🌳 **AVL Tree Inventory**     | Products are indexed using a self-balancing AVL Tree.                    |
| 🔍 **Product Search**         | Search products efficiently by Product ID.                               |
| 💰 **Sell Product**           | Sell products with automatic stock reduction.                            |
| 📥 **Restocking**             | Purchase additional stock through the Update Product menu.               |
| ⚠️ **Low Stock Alert**        | Detect products that reach their minimum stock level.                    |
| 📅 **Expired Products**       | Detect expired and soon-to-expire products.                              |
| ↩️ **Undo Last Operation**    | Reverse recent operations using a Stack.                                 |
| 💵 **Accounting**             | Manage income, expenses, transactions, and profit/loss.                  |
| 👨‍💼 **Employee Management** | Store employee information and salary records.                           |
| 🕒 **Attendance Management**  | Mark and track employee attendance.                                      |
| 📊 **Reports**                | Generate inventory, sales, purchase, accounting, and attendance reports. |
| 💾 **Data Persistence**       | Automatically save and load data using `.dat` files.                     |
| 🗂️ **Sample Data**           | Includes sample products and employees for demonstration.                |

---

## 🧠 Data Structures Used

| System Component      | Data Structure | Purpose                                                         |
| :-------------------- | :------------- | :-------------------------------------------------------------- |
| **Product Inventory** | AVL Tree       | Fast product insertion, search, deletion, and sorted traversal. |
| **Undo System**       | Stack (LIFO)   | Reverses the most recent operation.                             |
| **Sales History**     | Linked List    | Stores sales records.                                           |
| **Purchase History**  | Linked List    | Stores purchase/restock records.                                |
| **Accounting**        | Linked List    | Stores income and expense transactions.                         |
| **Attendance**        | Linked List    | Stores employee attendance records.                             |
| **Employees**         | Linked List    | Stores employee information.                                    |

---

## 🚀 Getting Started

### Prerequisites

* GCC compiler
* Linux, Windows, or macOS
* Basic terminal/command-line environment
* C compiler supporting C11

---

## 📁 Project Structure

```text
📦 SmartMart
│
├── main.c
├── product.c
├── avl.c
├── stack.c
├── linkedlist.c
├── sales.c
├── accounting.c
├── employee.c
├── attendance.c
├── reports.c
├── file.c
├── utils.c
├── Makefile
│
├── data/
│   ├── products.dat
│   ├── employees.dat
│   ├── sales.dat
│   ├── purchases.dat
│   ├── transactions.dat
│   └── attendance.dat
│
└── sample_data/
    ├── products.dat
    └── employees.dat
```

> **Note:** The project intentionally uses no custom `.h` files. Functions and required structures are declared in the `.c` files where needed.

---

## ⚙️ Compilation

### Option 1 — GCC

```bash
gcc main.c product.c avl.c stack.c linkedlist.c sales.c accounting.c employee.c attendance.c reports.c file.c utils.c -o smartmart
```

### Option 2 — Makefile

```bash
make
```

Additional commands:

```bash
make run
make clean-data
make clean
```

---

## ▶️ Running

```bash
./smartmart
```

To load sample data:

```bash
mkdir -p data
cp sample_data/*.dat data/
./smartmart
```

---

## 🖥️ Main Menu

```text
========================================
       SMART GROCERY SHOP MANAGEMENT
========================================

[1]  Add Product
[2]  Search Product
[3]  Sell Product
[4]  Update Product
[5]  Delete Product
[6]  Show Inventory
[7]  Low Stock Alert
[8]  Expired Products
[9]  Undo Last Operation
[10] Reports
[11] Accounting
[12] Attendance Management
[13] Exit

========================================
```

---

## 📊 Time Complexity

| Operation           |   Complexity |
| ------------------- | -----------: |
| AVL Insert          | **O(log n)** |
| AVL Search          | **O(log n)** |
| AVL Delete          | **O(log n)** |
| AVL Traversal       |     **O(n)** |
| Stack Push          |     **O(1)** |
| Stack Pop           |     **O(1)** |
| Stack Peek          |     **O(1)** |
| Linked List Insert  |     **O(1)** |
| Linked List Search  |     **O(n)** |
| Linked List Delete  |     **O(n)** |
| Product Name Search |     **O(n)** |

---

## 🤝 Team Collaboration

The project was developed collaboratively by dividing the system into independent modules.

* Each team member worked on assigned modules.
* The **Team Lead** coordinated the overall architecture and integration.
* All members participated in testing and debugging.
* The final source code and documentation were reviewed together before submission.

This workflow allowed the team to develop and integrate multiple modules efficiently while maintaining a consistent project structure.

---

## 🎓 Academic Purpose

This project was developed as part of a **Data Structures & Algorithms** course to demonstrate the practical application of:

* AVL Trees
* Stacks
* Linked Lists
* File Handling
* Modular Programming
* Algorithmic Time Complexity
* Team-based Software Development

---

## 👨‍💻 About the Developer

**Md Shifat Uz Zaman**

💻 C Programmer
🎓 Computer Science & Engineering Student
🌱 Interested in Software Development
🧠 Interested in Data Structures & Algorithms
🚀 Passionate about building practical software projects
📧 Email: shifatuszamun@gmail.com

---

## 📄 License

This project is developed for **educational and academic purposes** as a university team project.

---

**Built with C • Data Structures • Algorithms • Teamwork • Problem Solving**

> *"Good software is built through good teamwork."*
