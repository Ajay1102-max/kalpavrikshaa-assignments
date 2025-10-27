#include <stdio.h>

#define MAX_STUDENTS 100

enum Grade { A, B, C, D, F };

struct Student {
    int rollNumber;
    char name[50];
    float marks1, marks2, marks3;
    float total;
    float average;
    enum Grade grade;
};

int getStarCountForGrade(enum Grade grade) {
    switch (grade) {
        case A: return 5;
        case B: return 4;
        case C: return 3;
        case D: return 2;
        default: return 0;
    }
}

float calculateTotal(float m1, float m2, float m3) {
    return m1 + m2 + m3;
}

float calculateAverage(float total) {
    return total / 3.0f;
}

enum Grade calculateGrade(float average) {
    if (average >= 85)
        return A;
    else if (average >= 70)
        return B;
    else if (average >= 50)
        return C;
    else if (average >= 35)
        return D;
    else
        return F;
}

void displayPerformancePattern(enum Grade grade) {
    if (grade == F)
        return;

    printf("Performance: ");
    for (int i = 0; i < getStarCountForGrade(grade); i++) {
        printf("*");
    }
    printf("\n");
}

int isDuplicateRoll(struct Student students[], int count, int roll) {
    for (int i = 0; i < count; i++) {
        if (students[i].rollNumber == roll)
            return 1;
    }
    return 0;
}

void inputStudents(struct Student students[], int *n) {
    printf("Enter number of students: ");
    scanf("%d", n);

    for (int i = 0; i < *n; i++) {
        while (1) {
            printf("Enter Roll, Name, and 3 subject marks for student %d: ", i + 1);
            scanf("%d %s %f %f %f",
                  &students[i].rollNumber,
                  students[i].name,
                  &students[i].marks1,
                  &students[i].marks2,
                  &students[i].marks3);

            if (isDuplicateRoll(students, i, students[i].rollNumber)) {
                printf("Duplicate roll number! Please re-enter.\n");
                continue;
            }

            if ((students[i].marks1 < 0 || students[i].marks1 > 100) ||
                (students[i].marks2 < 0 || students[i].marks2 > 100) ||
                (students[i].marks3 < 0 || students[i].marks3 > 100)) {
                printf("Invalid marks! Marks must be between 0 and 100.\n");
                continue;
            }

            students[i].total = calculateTotal(students[i].marks1, students[i].marks2, students[i].marks3);
            students[i].average = calculateAverage(students[i].total);
            students[i].grade = calculateGrade(students[i].average);
            break;
        }
    }
}

void displayResults(struct Student students[], int n) {
    for (int i = 0; i < n; i++) {
        printf("Roll: %d\n", students[i].rollNumber);
        printf("Name: %s\n", students[i].name);
        printf("Total: %.0f\n", students[i].total);
        printf("Average: %.2f\n", students[i].average);

        char gradeChar;
        switch (students[i].grade) {
            case A: gradeChar = 'A'; break;
            case B: gradeChar = 'B'; break;
            case C: gradeChar = 'C'; break;
            case D: gradeChar = 'D'; break;
            default: gradeChar = 'F';
        }

        printf("Grade: %c\n", gradeChar);

        if (students[i].grade != F)
            displayPerformancePattern(students[i].grade);

        printf("\n");
    }
}

void printRollNumbersRecursively(struct Student students[], int index, int n) {
    if (index == n)
        return;
    printf("%d ", students[index].rollNumber);
    printRollNumbersRecursively(students, index + 1, n);
}

int main() {
    struct Student students[MAX_STUDENTS];
    int n;

    inputStudents(students, &n);
    printf("\n");
    displayResults(students, n);
    printf("List of Roll Numbers (via recursion): ");
    printRollNumbersRecursively(students, 0, n);
    printf("\n");

    return 0;
}