# Assignment 01 – Calculator

## 📌 Problem Statement
Design a console-based calculator program in **C** that accepts mathematical expressions as input in the form of strings and evaluates them.  
The calculator should support the following:
- Operators: `+`, `-`, `*`, `/`
- Integer-only operations
- Operator precedence (DMAS rule)
- Whitespace ignored
- Error handling:
  - Division by zero → `Error: Division by zero.`
  - Invalid expressions → `Error: Invalid expression.`

---

## 🛠️ How to Compile & Run
Open terminal inside this folder (`assignment-01-calculator`) and run:

```bash
# Compile
gcc calculator.c -o calculator

# Run interactively
./calculator
3 + 5 * 2
# Output: 13

# Run with echo
echo "3 + 5 * 2" | ./calculator
# Output: 13
