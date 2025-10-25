#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_SIZE 10

int* getAddress(int *matrix, int size, int row, int col) {
    return (matrix + row * size + col);
}

void swap(int *x, int *y) {
    int temp = *x;
    *x = *y;
    *y = temp;
}

void printMatrix(int *matrix, int size) {
    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size; col++) {
            printf("%4d", *getAddress(matrix, size, row, col));
        }
        printf("\n");
    }
}

void generateRandomMatrix(int *matrix, int size) {
    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size; col++) {
            *getAddress(matrix, size, row, col) = rand() % 256;
        }
    }
}

void transposeMatrix(int *matrix, int size) {
    for (int row = 0; row < size; row++) {
        for (int col = row + 1; col < size; col++) {
            swap(getAddress(matrix, size, row, col),
                 getAddress(matrix, size, col, row));
        }
    }
}

void reverseRows(int *matrix, int size) {
    for (int row = 0; row < size; row++) {
        int left = 0;
        int right = size - 1;

        while (left < right) {
            swap(getAddress(matrix, size, row, left),
                 getAddress(matrix, size, row, right));
            left++;
            right--;
        }
    }
}

void rotate90Clockwise(int *matrix, int size) {
    transposeMatrix(matrix, size);
    reverseRows(matrix, size);
}

void applySmoothingFilter(int *matrix, int size) {
    int previousRow[MAX_SIZE];
    int currentRow[MAX_SIZE];
    int nextRow[MAX_SIZE];
    int outputRow[MAX_SIZE];

    // Initialize the first current row
    for (int col = 0; col < size; col++) {
        currentRow[col] = *(matrix + 0 * size + col);
    }

    if (size > 1) {
        for (int col = 0; col < size; col++) {
            nextRow[col] = *(matrix + 1 * size + col);
        }
    }

    for (int row = 0; row < size; row++) {
        for (int col = 0; col < size; col++) {
            int sum = 0;
            int count = 0;

            for (int dRow = -1; dRow <= 1; dRow++) {
                int neighborRow = row + dRow;
                if (neighborRow < 0 || neighborRow >= size) {
                    continue;
                }

                for (int dCol = -1; dCol <= 1; dCol++) {
                    int neighborCol = col + dCol;
                    if (neighborCol < 0 || neighborCol >= size) {
                        continue;
                    }

                    int value;
                    if (neighborRow == row - 1) {
                        value = previousRow[neighborCol];
                    } else if (neighborRow == row) {
                        value = currentRow[neighborCol];
                    } else {
                        value = nextRow[neighborCol];
                    }

                    sum += value;
                    count++;
                }
            }
            outputRow[col] = sum / count;
        }

        for (int col = 0; col < size; col++) {
            *(matrix + row * size + col) = outputRow[col];
        }

        for (int col = 0; col < size; col++) {
            previousRow[col] = currentRow[col];
        }

        if (row + 2 < size) {
            for (int col = 0; col < size; col++) {
                currentRow[col] = nextRow[col];
                nextRow[col] = *(matrix + (row + 2) * size + col);
            }
        } else {
            for (int col = 0; col < size; col++) {
                currentRow[col] = nextRow[col];
            }
        }
    }
}

int main() {
    int size;
    printf("Enter matrix size (2-10): ");
    if (scanf("%d", &size) != 1 || size < 2 || size > 10) {
        printf("Invalid size. Must be between 2 and 10.\n");
        return 1;
    }

    int matrix[MAX_SIZE][MAX_SIZE];
    srand((unsigned)time(NULL));

    generateRandomMatrix((int*)matrix, size);
    printf("\nOriginal Matrix:\n");
    printMatrix((int*)matrix, size);

    rotate90Clockwise((int*)matrix, size);
    printf("\nMatrix After 90° Rotation:\n");
    printMatrix((int*)matrix, size);

    applySmoothingFilter((int*)matrix, size);
    printf("\nMatrix After Smoothing Filter:\n");
    printMatrix((int*)matrix, size);

    return 0;
}
