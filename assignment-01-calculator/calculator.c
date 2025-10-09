#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <limits.h>

#define MAX_INPUT_SIZE 235  // Global constant for input size

// Function prototypes
int evaluateExpression(const char *expr, int *error);
int parseTerm(const char **expr, int *error);
int parseFactor(const char **expr, int *error);
int safeMultiply(int a, int b, int *error);
int safeAdd(int a, int b, int *error);

// Function to skip spaces
void skipSpaces(const char **expr) {
    while (**expr == ' ') (*expr)++;
}

// Handle safe addition with overflow check
int safeAdd(int a, int b, int *error) {
    if ((b > 0 && a > INT_MAX - b) || (b < 0 && a < INT_MIN - b)) {
        printf("Error: Integer overflow.\n");
        *error = 1;
        return 0;
    }
    return a + b;
}

// Handle safe multiplication with overflow check
int safeMultiply(int a, int b, int *error) {
    if (a != 0 && (b > INT_MAX / a || b < INT_MIN / a)) {
        printf("Error: Integer overflow.\n");
        *error = 1;
        return 0;
    }
    return a * b;
}

// Parse a factor (integer number)
int parseFactor(const char **expr, int *error) {
    skipSpaces(expr);
    int sign = 1;

    if (**expr == '+') (*expr)++;
    else if (**expr == '-') {
        sign = -1;
        (*expr)++;
    }

    skipSpaces(expr);

    if (!isdigit(**expr)) {
        printf("Error: Invalid expression.\n");
        *error = 1;
        return 0;
    }

    int num = 0;
    while (isdigit(**expr)) {
        int digit = **expr - '0';
        if (num > (INT_MAX - digit) / 10) {
            printf("Error: Integer overflow.\n");
            *error = 1;
            return 0;
        }
        num = num * 10 + digit;
        (*expr)++;
    }

    return sign * num;
}

// Parse multiplication/division
int parseTerm(const char **expr, int *error) {
    int value = parseFactor(expr, error);
    if (*error) return 0;

    while (1) {
        skipSpaces(expr);
        char op = **expr;
        if (op != '*' && op != '/') break;
        (*expr)++;

        int next = parseFactor(expr, error);
        if (*error) return 0;

        if (op == '*') value = safeMultiply(value, next, error);
        else {
            if (next == 0) {
                printf("Error: Division by zero.\n");
                *error = 1;
                return 0;
            }
            value /= next;
        }
    }
    return value;
}

// Parse addition/subtraction
int evaluateExpression(const char *expr, int *error) {
    const char *p = expr;
    int value = parseTerm(&p, error);
    if (*error) return 0;

    int hasOperator = 0; // Track if expression has any +, -, *, /

    while (1) {
        skipSpaces(&p);
        char op = *p;
        if (op != '+' && op != '-') break;

        hasOperator = 1; // Found operator

        p++;

        skipSpaces(&p);

        // Reject double operators like ++ or --
        if (*p == '+' || *p == '-') {
            printf("Error: Invalid consecutive operators.\n");
            *error = 1;
            return 0;
        }

        int next = parseTerm(&p, error);
        if (*error) return 0;

        if (op == '+') value = safeAdd(value, next, error);
        else value = safeAdd(value, -next, error);
    }

    skipSpaces(&p);
    if (*p != '\0') {
        printf("Error: Invalid trailing characters.\n");
        *error = 1;
        return 0;
    }

    // Expression should contain at least one operator
    if (!hasOperator) {
        printf("Error: Invalid expression (no operator found).\n");
        *error = 1;
        return 0;
    }

    return value;
}

int main() {
    char expr[MAX_INPUT_SIZE];

    printf("Enter expression: ");
    fgets(expr, sizeof(expr), stdin);

    // Remove newline character
    expr[strcspn(expr, "\n")] = '\0';

    int error = 0;
    int result = evaluateExpression(expr, &error);

    if (!error) {
        printf("Result: %d\n", result);
    }

    return 0;
}