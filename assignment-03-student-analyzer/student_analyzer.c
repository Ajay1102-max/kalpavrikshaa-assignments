#include <stdio.h>

// Structure to store student details
struct Student {
    int rollNumber;
    char name[50];
    float marks1, marks2, marks3;
    float total;
    float average;
    char grade;
};

// Function prototypes
float calculateTotal(float m1, float m2, float m3);
float calculateAverage(float total);
char calculateGrade(float average);
void displayPerformancePattern(char grade);
void printRollNumbersRecursively(struct Student students[], int index, int n);

// Function to calculate total marks
float calculateTotal(float m1, float m2, float m3) {
    return m1 + m2 + m3;
}

// Function to calculate average marks
float calculateAverage(float total) {
    return total / 3.0;
}

// Function to assign grade based on average
char calculateGrade(float average) {
    if (average >= 85)
        return 'A';
    else if (average >= 70)
        return 'B';
    else if (average >= 50)
        return 'C';
    else if (average >= 35)
        return 'D';
    else
        return 'F';
}

// Function to display performance pattern (stars)
void displayPerformancePattern(char grade) {
    int stars = 0;

    switch (grade) {
        case 'A': stars = 5; break;
        case 'B': stars = 4; break;
        case 'C': stars = 3; break;
        case 'D': stars = 2; break;
        default: stars = 0;
    }

    if (grade == 'F') {
        return; // Skip printing for failed students
    }

    printf("Performance: ");
    for (int i = 0; i < stars; i++) {
        printf("*");
    }
    printf("\n");
}

// Recursive function to print roll numbers
void printRollNumbersRecursively(struct Student students[], int index, int n) {
    if (index == n)
        return;
    printf("%d ", students[index].rollNumber);
    printRollNumbersRecursively(students, index + 1, n);
}

int main() {
    int n;
    struct Student students[100];

    printf("Enter number of students: ");
    scanf("%d", &n);

    // Input student details
    for (int i = 0; i < n; i++) {
        while (1) {
            printf("\nEnter Roll, Name, and 3 subject marks for student %d: ", i + 1);
            scanf("%d %s %f %f %f", 
                  &students[i].rollNumber, 
                  students[i].name, 
                  &students[i].marks1, 
                  &students[i].marks2, 
                  &students[i].marks3);

            // Check marks range
            if ((students[i].marks1 < 0 || students[i].marks1 > 100) ||
                (students[i].marks2 < 0 || students[i].marks2 > 100) ||
                (students[i].marks3 < 0 || students[i].marks3 > 100)) {
                printf("Invalid marks entered! Each mark must be between 0 and 100.\n");
                printf("Please re-enter details for this student.\n");
                continue;  // ask again for this student
            }
            break;  // valid input, exit loop
        }

        students[i].total = calculateTotal(students[i].marks1, students[i].marks2, students[i].marks3);
        students[i].average = calculateAverage(students[i].total);
        students[i].grade = calculateGrade(students[i].average);
    }

    printf("\n");

    // Display student performance
    for (int i = 0; i < n; i++) {
        printf("Roll: %d\n", students[i].rollNumber);
        printf("Name: %s\n", students[i].name);
        printf("Total: %.0f\n", students[i].total);
        printf("Average: %.2f\n", students[i].average);
        printf("Grade: %c\n", students[i].grade);

        if (students[i].grade != 'F') {
            displayPerformancePattern(students[i].grade);
        }

        printf("\n");
    }

    // Print roll numbers using recursion
    printf("List of Roll Numbers (via recursion): ");
    printRollNumbersRecursively(students, 0, n);
    printf("\n");

    return 0;
}