# Week 3 Assignment – Inventory Management System (C)

This program implements a dynamic inventory management system in C. It demonstrates safe input handling, dynamic memory management (`calloc`, `realloc`), modular design, and basic CRUD operations.

## Features
- Add new products (ID, name, price, quantity)
- View all products
- Update product quantity
- Search by product ID
- Search by product name (partial, case-insensitive)
- Search by price range
- Delete product by ID
- Capacity-doubling strategy to avoid frequent reallocs
- Input validation and safe string handling

## Files
- `inventory.c` — program source (mentor-ready version)
- `testcases.txt` — optional automated test input (example included)
- `.gitignore` — suggested ignores (e.g. `*.exe`, `*.o`)

## Build & Run

### Compile:
```bash
gcc inventory.c -o inventory