# Week 2 Assignment – Sonar Image Processing

This C program simulates sonar image processing using dynamic memory allocation and pointer arithmetic.  
It demonstrates how 2D data (like sonar images) can be manipulated using only pointers, without direct array indexing or extra matrices.

---

## 🧭 Overview

Each sonar image is represented as an N×N matrix (2 ≤ N ≤ 10).  
Every element of the matrix represents a pixel intensity between **0 and 255**.

The program performs three main operations:

1. **Generate Matrix**  
   - Creates a random NxN matrix using `rand() % 256`.  
   - Values represent sonar pixel intensities.

2. **Rotate Matrix (90° Clockwise)**  
   - Rotates the image in-place using **pointer arithmetic** only.  
   - No extra N×N matrix is used.

3. **Apply 3×3 Smoothing Filter**  
   - Each element is replaced with the average of itself and its valid neighbors.  
   - Averages are calculated using pointer traversal.

---

## ⚙️ Features

- Uses **dynamic memory allocation** (`malloc()` / `free()`).
- Implements **pointer arithmetic** instead of array indexing.
- **No extra 2D arrays** used (only minimal temporary memory for a row).
- Clean, modular functions for readability and reusability.
- Handles **edge/corner elements** correctly during smoothing.

---

## 🧩 Functions Summary

| Function | Purpose |
|-----------|----------|
| `generateMatrix()` | Generates random sonar image matrix |
| `displayMatrix()` | Prints the matrix in formatted grid |
| `rotate90Clockwise()` | Rotates the matrix 90° clockwise in-place |
| `averageOfNeighbors()` | Calculates average of a cell and its valid neighbors |
| `applySmoothingFilter()` | Applies 3×3 smoothing filter row-by-row |
| `main()` | Controls the program flow |

---

## 💻 Input / Output Format

### **Input:**
