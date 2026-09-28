#include <stdio.h>
#include <stdlib.h>

struct User {
    int id;
    char name[50];
    int age;
};

void createUser() {
    struct User user;
    int found = 0;

    printf("Enter user ID: ");
    scanf("%d", &user.id);

    printf("Enter user name: ");
    scanf("%49s", user.name);

    printf("Enter user age: ");
    scanf("%d", &user.age);

    // Check whether ID already exists
    FILE *fp = fopen("users.txt", "r");

    if (fp != NULL) {
        struct User existing;

        while (fscanf(fp, "%d %49s %d",
                      &existing.id,
                      existing.name,
                      &existing.age) == 3) {

            if (existing.id == user.id) {
                found = 1;
                break;
            }
        }

        fclose(fp);
    }

    if (found) {
        printf("User with ID %d already exists.\n", user.id);
        return;
    }

    // Add new user
    fp = fopen("users.txt", "a");

    if (fp == NULL) {
        printf("Error opening file!\n");
        return;
    }

    fprintf(fp, "%d %s %d\n", user.id, user.name, user.age);

    fclose(fp);

    printf("User added successfully.\n");
}

void readUsers() {
    FILE *fp = fopen("users.txt", "r");

    if (fp == NULL) {
        printf("No users found.\n");
        return;
    }

    struct User user;

    while (fscanf(fp, "%d %49s %d",
                  &user.id,
                  user.name,
                  &user.age) == 3) {

        printf("ID: %d, Name: %s, Age: %d\n",
               user.id,
               user.name,
               user.age);
    }

    fclose(fp);
}

void updateUser() {
    FILE *fp = fopen("users.txt", "r");
    FILE *temp = fopen("temp.txt", "w");

    if (fp == NULL || temp == NULL) {
        printf("Error opening file!\n");

        if (fp != NULL)
            fclose(fp);

        if (temp != NULL)
            fclose(temp);

        return;
    }

    int id;
    printf("Enter user ID to update: ");
    scanf("%d", &id);

    struct User user;
    int found = 0;

    while (fscanf(fp, "%d %49s %d",
                  &user.id,
                  user.name,
                  &user.age) == 3) {

        if (user.id == id) {
            printf("Enter new name: ");
            scanf("%49s", user.name);

            printf("Enter new age: ");
            scanf("%d", &user.age);

            found = 1;
        }

        fprintf(temp, "%d %s %d\n",
                user.id,
                user.name,
                user.age);
    }

    fclose(fp);
    fclose(temp);

    remove("users.txt");
    rename("temp.txt", "users.txt");

    if (found)
        printf("User updated successfully.\n");
    else
        printf("User with ID %d not found.\n", id);
}

void deleteUser() {
    FILE *fp = fopen("users.txt", "r");
    FILE *temp = fopen("temp.txt", "w");

    if (fp == NULL || temp == NULL) {
        printf("Error opening file!\n");

        if (fp != NULL)
            fclose(fp);

        if (temp != NULL)
            fclose(temp);

        return;
    }

    int id;
    printf("Enter user ID to delete: ");
    scanf("%d", &id);

    struct User user;
    int found = 0;

    while (fscanf(fp, "%d %49s %d",
                  &user.id,
                  user.name,
                  &user.age) == 3) {

        if (user.id == id) {
            found = 1;
            continue;
        }

        fprintf(temp, "%d %s %d\n",
                user.id,
                user.name,
                user.age);
    }

    fclose(fp);
    fclose(temp);

    remove("users.txt");
    rename("temp.txt", "users.txt");

    if (found)
        printf("User deleted successfully.\n");
    else
        printf("User with ID %d not found.\n", id);
}

int main() {
    int choice;

    do {
        printf("\n1. Create User\n");
        printf("2. Read Users\n");
        printf("3. Update User\n");
        printf("4. Delete User\n");
        printf("5. Exit\n");

        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                createUser();
                break;

            case 2:
                readUsers();
                break;

            case 3:
                updateUser();
                break;

            case 4:
                deleteUser();
                break;

            case 5:
                printf("Exiting...\n");
                break;

            default:
                printf("Invalid choice!\n");
        }

    } while (choice != 5);

    return 0;
}