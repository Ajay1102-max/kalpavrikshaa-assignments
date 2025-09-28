#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

// Forward declarations
int parseExpression(const char **str);
int parseTerm(const char **str);
int parseFactor(const char **str);

// Skip whitespaces
void skipSpaces(const char **str) {
    while (**str == ' ') (*str)++;
}

// Parse integer numbers
int parseNumber(const char **str) {
    skipSpaces(str);
    int num = 0;
    int sign = 1;

    if (**str == '-') {
        sign = -1;
        (*str)++;
    }

    if (!isdigit(**str)) {
        printf("Error: Invalid expression.\n");
        exit(1);
    }

    while (isdigit(**str)) {
        num = num * 10 + (**str - '0');
        (*str)++;
    }

    return sign * num;
}

// Parse factors (numbers only here)
int parseFactor(const char **str) {
    return parseNumber(str);
}

// Parse terms (multiplication/division)
int parseTerm(const char **str) {
    int value = parseFactor(str);

    while (1) {
        skipSpaces(str);
        if (**str == '*') {
            (*str)++;
            value *= parseFactor(str);
        } else if (**str == '/') {
            (*str)++;
            int divisor = parseFactor(str);
            if (divisor == 0) {
                printf("Error: Division by zero.\n");
                exit(1);
            }
            value /= divisor;
        } else {
            break;
        }
    }
    return value;
}

// Parse expressions (addition/subtraction)
int parseExpression(const char **str) {
    int value = parseTerm(str);

    while (1) {
        skipSpaces(str);
        if (**str == '+') {
            (*str)++;
            value += parseTerm(str);
        } else if (**str == '-') {
            (*str)++;
            value -= parseTerm(str);
        } else {
            break;
        }
    }
    return value;
}

int main() {
    char input[256];

    // Read input expression
    if (!fgets(input, sizeof(input), stdin)) {
        printf("Error: Invalid input.\n");
        return 1;
    }

    // Remove newline if present
    input[strcspn(input, "\n")] = 0;

    const char *expr = input;
    int result = parseExpression(&expr);

    skipSpaces(&expr);
    if (*expr != '\0') { // if anything left unparsed
        printf("Error: Invalid expression.\n");
        return 1;
    }

    printf("%d\n", result);
    return 0;
}
