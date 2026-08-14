<<<<<<< HEAD
# SMART GROCERY SHOP MANAGEMENT SYSTEM
### A Data Structures & Algorithms University Project (Pure C)

---

## 1. Project Folder Structure

```
SmartMart/
│
├── main.c            - Program entry point, main menu loop, load/save orchestration
├── product.c          - Add/Search/Update/Delete/Show Inventory/Low Stock/Expired + Restock
├── avl.c               - AVL Tree (Product inventory index)
├── stack.c            - Undo stack (LIFO) + undo-reversal logic
├── linkedlist.c        - Linked lists: Sales, Purchases, Transactions, Attendance
├── sales.c            - Sell Product
├── accounting.c       - Income / Expense / Transactions / Reports
├── employee.c          - Employee records + Salary payment
├── attendance.c        - Attendance marking, search, monthly report, percentage
├── reports.c           - Reports submenu (pulls from every other module)
├── file.c              - Load/save all 6 .dat files
├── utils.c             - Safe input helpers + date parsing/comparison
├── Makefile
│
├── data/               - Created automatically; holds the live .dat files
│   ├── products.dat
│   ├── employees.dat
│   ├── sales.dat
│   ├── purchases.dat
│   ├── transactions.dat
│   └── attendance.dat
│
└── sample_data/        - Example pre-filled .dat files for a quick demo
    ├── products.dat
    └── employees.dat
```

No custom `.h` files are used anywhere. Every `.c` file that calls a function
defined in another `.c` file re-declares that function (and any struct it
needs) with an `extern` prototype at the top of the file — this is standard,
legal C; the compiler only needs to see a matching prototype before the call,
and the linker resolves the actual symbol at link time.

---

## 2. Compilation

### Option A — GCC directly
```bash
gcc main.c product.c avl.c stack.c linkedlist.c sales.c accounting.c employee.c attendance.c reports.c file.c utils.c -o smartmart
```

### Option B — Makefile (recommended)
```bash
make          # builds ./smartmart and creates the data/ folder
make run      # builds (if needed) and runs it
make clean-data   # wipes saved data to start fresh
make clean    # removes the compiled binary
```

The project compiles cleanly with zero warnings under `-Wall -Wextra -std=c11`.

---

## 3. Running

```bash
./smartmart
```

On first run `data/` will be empty, so the program starts with an empty
inventory — just use option 1 to add products. To try the system pre-loaded
with sample data instead:

```bash
mkdir -p data
cp sample_data/*.dat data/
./smartmart
```

Every menu action that changes data automatically saves all six `.dat` files,
so you can close the program (option 13, or Ctrl+D / Ctrl+C) at any point
without losing anything already confirmed.

---

## 4. Data Structures Used — What and Why

| Feature                         | Data Structure | File          |
|----------------------------------|-----------------|----------------|
| Product inventory                | **AVL Tree** (self-balancing BST), keyed by Product ID | `avl.c` |
| Undo Last Operation              | **Stack** (LIFO, singly linked) | `stack.c` |
| Sales history                    | **Linked List** | `linkedlist.c` |
| Purchase / Restock history       | **Linked List** | `linkedlist.c` |
| Accounting transactions          | **Linked List** | `linkedlist.c` |
| Attendance records                | **Linked List** | `linkedlist.c` |
| Employees                        | **Linked List** | `employee.c` |

### AVL Tree (`avl.c`)
Every `Product` is a tree node (`id, name, category, buyPrice, sellPrice,
quantity, minStock, expiryDate, supplierId, height, *left, *right`). The tree
is kept balanced after every insert and delete using the classic 4 rotation
cases (LL, RR, LR, RL), so height never exceeds `O(log n)` even in the worst
case (e.g. inserting Product IDs in already-sorted order — a plain BST would
degrade to a linked list here, but the AVL tree stays balanced).

Implemented operations: `insertProduct`, `searchProductById`, `deleteProduct`
(with backup-before-delete for Undo), `updateProductData`, `inorderVisit`
(sorted listing, and also used to write `products.dat`), `preorderVisit`,
`postorderVisit`, `heightOf`, `getBalance`, `leftRotate`, `rightRotate`.

### Stack (`stack.c`)
A singly linked list used strictly LIFO: `pushUndo` adds to the head,
`popUndo` removes from the head, `peekUndo` reads the head without removing
it, `isStackEmpty` checks for `NULL`. Six convenience wrapper functions
(`pushUndoAdd`, `pushUndoDelete`, `pushUndoUpdate`, `pushUndoSell`,
`pushUndoRestock`, `pushUndoAccount`) build the correct `UndoOp` snapshot for
each action so the rest of the program never has to know the struct's
internal layout. `performUndo()` pops the top record and — based on its
`opType` — actually reverses inventory, the linked-list history, and the
accounting transaction that resulted from the original action.

### Linked Lists (`linkedlist.c`)
Four independent singly linked lists, each inserted at the head (`O(1)`).
Used for Sales, Purchases, Transactions and Attendance because these are
append-heavy, sequentially-scanned histories — a perfect fit for a linked
list, and there's no need for the ordering/searching power (or rebalancing
cost) of a tree here.

---

## 5. Time Complexity

| Operation                        | Complexity |
|------------------------------------|-------------|
| AVL Insert                        | O(log n) |
| AVL Search (by ID)                | O(log n) |
| AVL Delete                        | O(log n) |
| AVL Inorder / Preorder / Postorder | O(n) |
| Stack Push                        | O(1) |
| Stack Pop / Peek                  | O(1) |
| Linked List Insert (at head)      | O(1) |
| Linked List Search / Delete-by-key | O(n) |
| Product Name search (linear scan) | O(n) — AVL is keyed by ID only, so name search walks the whole tree |

---

## 6. How Everything Is Integrated

```
SELL PRODUCT                         RESTOCK (via Update Product menu)
  Search AVL by ID                     Search AVL by ID
  Check stock                          Increase stock
  Reduce stock                         Record Purchase (linked list)
  Record Sale (linked list)            Auto-create Expense transaction
  Auto-create Income transaction       Push RESTOCK snapshot to Undo Stack
  Push SELL snapshot to Undo Stack     Save all data
  Save all data

PAY SALARY                           UNDO (any of the above)
  Auto-create Expense transaction       Pop the Undo Stack
  Push SALARY marker to Undo Stack      Restore stock / re-insert / re-delete
  Appears in all Accounting reports     Remove the linked transaction (if any)
                                        Remove the linked sale/purchase record
                                        Save all data
```

---

## 7. Sample Test Data

`sample_data/products.dat` and `sample_data/employees.dat` contain 5 sample
products and 3 sample employees (see Section 3 for how to load them). Feel
free to open these `.dat` files in any text editor during a viva — they use a
simple `field|field|field` layout, one record per line, which is easy to
explain and easy to read by hand.

---

## 8. Viva Questions & Answers

**Q1. Why an AVL Tree instead of a plain BST for products?**
A plain BST degrades to O(n) per operation in the worst case (e.g. Product
IDs entered in sorted order create a straight line of nodes). The AVL Tree
guarantees the height stays O(log n) by rebalancing after every insert and
delete, so search/insert/delete stay fast no matter the insertion order.

**Q2. How does the AVL Tree decide when to rebalance?**
After every insert/delete, we walk back up the recursion and recompute each
ancestor's height and *balance factor* (`height(left) - height(right)`). If
the balance factor becomes `> 1` or `< -1`, the subtree is unbalanced and one
of four rotations (Left-Left, Right-Right, Left-Right, Right-Left) is applied
to restore balance in O(1) per node.

**Q3. Why is the Undo feature implemented as a Stack and not a Queue?**
Undo must reverse the *most recent* action first — Last In, First Out — which
is exactly what a Stack provides. A Queue would undo the *oldest* action
first, which is not what users expect from "Undo".

**Q4. How does Undo actually reverse a Sale?**
Before the sale reduces the stock, the product's pre-sale state (its full
data snapshot) is pushed onto the Undo stack along with the quantity sold and
the ID of the automatically-created Income transaction. On Undo, we look the
product back up by ID, add the quantity back to its stock, delete the
matching sale record from the sales linked list, and delete the transaction
by its stored ID — fully reversing every side effect of the sale.

**Q5. Why linked lists for Sales/Purchases/Transactions/Attendance instead of
another AVL Tree?**
These are append-only, chronological histories that are almost always read
either in full (for reports) or filtered by a simple linear condition (date,
employee ID). They don't need to be kept sorted by a search key, so the
O(1) insertion of a linked list is a better fit than the O(log n) insertion
(plus rebalancing overhead) of a tree.

**Q6. How is data made permanent across program runs?**
Every menu action that changes state immediately calls `saveAllData()`,
which writes all six `.dat` files (delimiter-separated plain text, one
record per line). On startup, `loadAllData()` reads each file back — for
products it re-inserts every record into a fresh AVL Tree one at a time
using the same `insertProductRecord()` function used by "Add Product", so
the tree is correctly rebuilt and balanced.

**Q7. Why is `|` used as the field delimiter in the `.dat` files instead of a
comma or fixed-width binary format?**
Product/employee names can contain commas or spaces, so comma-separated
values could be ambiguous. A `|` delimiter is unlikely to appear in real
shop data and keeps the file human-readable in a text editor — useful for
demonstrating file handling during a viva. Binary `fwrite`/`fread` of the
struct was avoided because our structs contain internal pointers
(`left`/`right`/`next`) that are meaningless once the program exits.

**Q8. How does the project avoid needing custom header (`.h`) files?**
Every `.c` file that uses a function or struct from another `.c` file
re-declares that function's prototype (marked `extern`) and re-defines the
struct with an identical field layout at the top of the file. C only
requires a matching declaration to exist *before* a call for the compiler to
generate the correct call, and the linker resolves the actual function
address afterward — so this compiles and links correctly with a single
`gcc *.c -o smartmart` command, without ever needing a shared header.

**Q9. How is "Low Stock" detected?**
Every product stores its own `minStock` threshold. `lowStockAlert()`
performs an AVL inorder traversal and flags any product where
`quantity <= minStock`, which is also reused inside the Inventory Report.

**Q10. How is "Expired" vs "Expiring Soon" decided?**
`daysBetweenAndToday()` converts a product's expiry date and today's date
into `time_t` values (via `mktime`) and returns the difference in days.
A negative result means the product already expired; a result between 0 and
7 is flagged as "Expiring Soon"; anything else is not shown.

**Q11. What happens on AVL deletion when a node has two children?**
We find the in-order successor (the smallest node in the right subtree),
copy its data into the node being "deleted", then recursively delete that
successor node from the right subtree (which has at most one child, so it's
the simple case). The tree is then rebalanced on the way back up the
recursion just like after an insert.

**Q12. How is Attendance Percentage calculated?**
`Attendance % = (Present Days + Late Days) / Total Recorded Days × 100`.
Late is counted as attended-but-late rather than absent, which is a common
real-world attendance policy; Absent and Leave days are excluded from the
numerator but counted in the denominator.

**Q13. Why does Restocking live inside the "Update Product" menu instead of
being its own main-menu option?**
The assignment specifies the main menu must contain exactly these 13 items,
with no separate "Restock" entry. Since restocking is conceptually an update
to a product's quantity (and cost), it's implemented as a submenu under
option 4 — "Update Product" → "Restock (Purchase more stock)" — keeping the
main menu exactly as specified while still fully implementing the feature.

**Q14. What is the time complexity of loading all products on startup?**
`O(n log n)` — the file is read in `O(n)` lines, but each product must be
individually inserted into the AVL tree at `O(log n)`, giving `O(n log n)`
total to rebuild the whole tree.

**Q15. How does the program prevent a duplicate Product ID or Employee ID?**
Before inserting, `findProduct()` / `findEmployee()` searches for an
existing record with that ID (`O(log n)` for products via the AVL tree,
`O(n)` for employees via the linked list) and the Add operation is rejected
with an error message if a match is found.
=======
# smart-grocery-shop-management
A console-based Smart Grocery Shop Management System written in pure C, using AVL Trees, Stacks, and Linked Lists. Built as a Data Structures &amp; Algorithms university team project — handles inventory, sales, purchases, accounting, employees, attendance, and file-based data persistence.
>>>>>>> 02ea32066d8817f800c9ef2034ddc05b2c98cecf
