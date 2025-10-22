#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void generateMatrix(int **matrix, int n) {
    for (int i = 0; i < n; i++)
        for (int j = 0; j < n; j++)
            *(*(matrix + i) + j) = rand() % 256;
}

void displayMatrix(int **matrix, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            printf("%4d", *(*(matrix + i) + j));
        printf("\n");
    }
}

void rotate90Clockwise(int **matrix, int n) {
    for (int i = 0; i < n; i++)
        for (int j = i + 1; j < n; j++) {
            int temp = *(*(matrix + i) + j);
            *(*(matrix + i) + j) = *(*(matrix + j) + i);
            *(*(matrix + j) + i) = temp;
        }

    for (int i = 0; i < n; i++) {
        int *left = *(matrix + i);
        int *right = *(matrix + i) + n - 1;
        while (left < right) {
            int temp = *left;
            *left = *right;
            *right = temp;
            left++;
            right--;
        }
    }
}

int averageOfNeighbors(int **matrix, int n, int x, int y) {
    int sum = 0, count = 0;
    for (int i = -1; i <= 1; i++)
        for (int j = -1; j <= 1; j++) {
            int ni = x + i, nj = y + j;
            if (ni >= 0 && ni < n && nj >= 0 && nj < n) {
                sum += *(*(matrix + ni) + nj);
                count++;
            }
        }
    return sum / count;
}

void applySmoothingFilter(int **matrix, int n) {
    int *temp = (int *)malloc(n * sizeof(int));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++)
            *(temp + j) = averageOfNeighbors(matrix, n, i, j);
        for (int j = 0; j < n; j++)
            *(*(matrix + i) + j) = *(temp + j);
    }
    free(temp);
}

int main() {
    int n;
    printf("Enter matrix size (2-10): ");
    scanf("%d", &n);
    if (n < 2 || n > 10) {
        printf("Invalid size.\n");
        return 1;
    }

    int **matrix = (int **)malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++)
        *(matrix + i) = (int *)malloc(n * sizeof(int));

    srand(time(0));
    generateMatrix(matrix, n);

    printf("\nOriginal Randomly Generated Matrix:\n");
    displayMatrix(matrix, n);

    rotate90Clockwise(matrix, n);
    printf("\nMatrix after 90° Clockwise Rotation:\n");
    displayMatrix(matrix, n);

    applySmoothingFilter(matrix, n);
    printf("\nMatrix after Applying 3×3 Smoothing Filter:\n");
    displayMatrix(matrix, n);

    for (int i = 0; i < n; i++)
        free(*(matrix + i));
    free(matrix);
    return 0;
}