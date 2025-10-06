#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME "users.txt"

struct User {
    int id;
    char name[50];
    int age;
};

// Check if ID already exists in file
int idExists(int id) {
    struct User user;
    FILE *file = fopen(FILENAME, "r");
    if (!file) return 0; // file not found = no IDs yet

    while (fscanf(file, "%d %s %d", &user.id, user.name, &user.age) == 3) {
        if (user.id == id) {
            fclose(file);
            return 1; // found
        }
    }
    fclose(file);
    return 0; // not found
}

// Function to create (add) a new user
void addUser() {
    struct User user;
    FILE *file = fopen(FILENAME, "a");
    if (!file) {
        printf("Error opening file!\n");
        return;
    }

    printf("Enter ID: ");
    scanf("%d", &user.id);

    // Check unique ID
    if (idExists(user.id)) {
        printf("Error: User with ID %d already exists.\n", user.id);
        fclose(file);
        return;
    }

    printf("Enter Name: ");
    scanf("%s", user.name);

    printf("Enter Age: ");
    scanf("%d", &user.age);

    // Check non-negative age
    if (user.age < 0) {
        printf("Error: Age cannot be negative.\n");
        fclose(file);
        return;
    }

    fprintf(file, "%d %s %d\n", user.id, user.name, user.age);
    fclose(file);

    printf("User added successfully.\n");
}

// Function to read (display) all users
void displayUsers() {
    struct User user;
    FILE *file = fopen(FILENAME, "r");
    if (!file) {
        printf("No users found. File does not exist yet.\n");
        return;
    }

    printf("\n--- User Records ---\n");
    while (fscanf(file, "%d %s %d", &user.id, user.name, &user.age) == 3) {
        printf("ID: %d, Name: %s, Age: %d\n", user.id, user.name, user.age);
    }
    fclose(file);
}

// Function to update a user by ID
void modifyUserById() {
    struct User user;
    int id, found = 0;

    printf("Enter ID of user to update: ");
    scanf("%d", &id);

    FILE *file = fopen(FILENAME, "r");
    FILE *temp = fopen("temp.txt", "w");
    if (!file || !temp) {
        printf("Error opening file!\n");
        return;
    }

    while (fscanf(file, "%d %s %d", &user.id, user.name, &user.age) == 3) {
        if (user.id == id) {
            printf("Enter new Name: ");
            scanf("%s", user.name);
            printf("Enter new Age: ");
            scanf("%d", &user.age);

            if (user.age < 0) {
                printf("Error: Age cannot be negative.\n");
                fclose(file);
                fclose(temp);
                remove("temp.txt");
                return;
            }

            found = 1;
        }
        fprintf(temp, "%d %s %d\n", user.id, user.name, user.age);
    }

    fclose(file);
    fclose(temp);

    remove(FILENAME);
    rename("temp.txt", FILENAME);

    if (found) {
        printf("User updated successfully.\n");
    } else {
        printf("User with ID %d not found.\n", id);
    }
}

// Function to delete a user by ID
void deleteUserById() {
    struct User user;
    int id, found = 0;

    printf("Enter ID of user to delete: ");
    scanf("%d", &id);

    FILE *file = fopen(FILENAME, "r");
    FILE *temp = fopen("temp.txt", "w");
    if (!file || !temp) {
        printf("Error opening file!\n");
        return;
    }

    while (fscanf(file, "%d %s %d", &user.id, user.name, &user.age) == 3) {
        if (user.id == id) {
            found = 1; // Skip writing this user
        } else {
            fprintf(temp, "%d %s %d\n", user.id, user.name, user.age);
        }
    }

    fclose(file);
    fclose(temp);

    remove(FILENAME);
    rename("temp.txt", FILENAME);

    if (found) {
        printf("User deleted successfully.\n");
    } else {
        printf("User with ID %d not found.\n", id);
    }
}

int main() {
    int choice;

    while (1) {
        printf("\n--- CRUD Operations Menu ---\n");
        printf("1. Add User\n");
        printf("2. Display Users\n");
        printf("3. Modify User By ID\n");
        printf("4. Delete User By ID\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: addUser(); break;
            case 2: displayUsers(); break;
            case 3: modifyUserById(); break;
            case 4: deleteUserById(); break;
            case 5: exit(0);
            default: printf("Invalid choice! Try again.\n");
        }
    }

    return 0;
}
