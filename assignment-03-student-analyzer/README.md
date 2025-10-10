# Assignment 03 – Student Performance Analyzer

## 📌 Problem Statement
Write a C program that:
- Reads details of `n` students (roll number, name, and three subject marks).
- Calculates total, average, and grade for each student.
- Displays performance in asterisks (`*`) based on grade.
- Prints all roll numbers using recursion.

## 🧠 Grading Rules
| Average | Grade | Stars |
|----------|--------|--------|
| 85–100   | A | ***** |
| 70–84.99 | B | **** |
| 50–69.99 | C | *** |
| 35–49.99 | D | ** |
| <35      | F | (No stars) |

## ⚙️ How to Compile & Run
```bash
# Compile
gcc student_analyzer.c -o student_analyzer

# Run
./student_analyzer