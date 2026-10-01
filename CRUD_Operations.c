#include <stdio.h>
#include <stdlib.h>
#include<string.h>

struct User {
    int id;
    char name[50];
    int age;
};

int getInt(){
    char input[100];
    int value;
    char extra; //to prevent scenario like 123abc being accepted 

    while(1){
        if (fgets(input, sizeof(input), stdin) == NULL) {
            return 0;
        }

        if(sscanf(input, "%d %c", &value, &extra) == 1){
            return value;
        }
        printf("Invalid Input Format. Please enter a valid integer: ");
    }
}


void createUser() {
    struct User user;
    int found = 0;

    printf("Enter user ID: ");
    user.id = getInt();

    // Check whether ID already exists
    FILE *fp = fopen("users.txt", "r");

    if (fp != NULL) {
        struct User existing;

        while (fscanf(fp, "%d|%49[^|]|%d",
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

    printf("Enter user name: ");
    fgets(user.name, sizeof(user.name), stdin);
    user.name[strcspn(user.name, "\n")] = '\0';

    printf("Enter user age: ");
    user.age = getInt();
    while(user.age <= 0 || user.age > 100){
        printf("Please enter a valid age between 1 and 100: ");
        user.age = getInt();
    }

    // Add new user
    fp = fopen("users.txt", "a");

    if (fp == NULL) {
        printf("Error opening file!\n");
        return;
    }

    fprintf(fp, "%d|%s|%d\n", user.id, user.name, user.age);

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

    while (fscanf(fp, "%d|%49[^|]|%d",
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
    id = getInt();

    struct User user;
    int found = 0;

    while (fscanf(fp, "%d|%49[^|]|%d",
                  &user.id,
                  user.name,
                  &user.age) == 3) {

        if (user.id == id) {
            printf("Enter new name: ");
            fgets(user.name, sizeof(user.name), stdin);
            user.name[strcspn(user.name, "\n")] = '\0';

            printf("Enter new age: ");
            user.age = getInt();
            while(user.age <= 0 || user.age > 100){
        printf("Please enter a valid age between 1 and 100: ");
        user.age = getInt();
    }

            found = 1;
        }

        fprintf(temp, "%d|%s|%d\n",
                user.id,
                user.name,
                user.age);
    }

    fclose(fp);
    fclose(temp);

    if(rename("users.txt", "backup.txt") != 0){
        printf("Error renaming original file!\n");
        return;
    }

    if (rename("temp.txt", "users.txt") != 0) {
        printf("Error replacing temporary file!\n");

        //restore original file
        if(rename("backup.txt", "users.txt") != 0){
            printf("Error restoring original file!\n");
        }
        return;
    }

    if (remove("backup.txt") != 0) {
        printf("Error deleting original file!\n");
        return;
    }


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
    id = getInt();

    struct User user;
    int found = 0;

    while (fscanf(fp, "%d|%49[^|]|%d",
                  &user.id,
                  user.name,
                  &user.age) == 3) {

        if (user.id == id) {
            found = 1;
            continue;
        }

        fprintf(temp, "%d|%s|%d\n",
                user.id,
                user.name,
                user.age);
    }

    fclose(fp);
    fclose(temp);
    
    if(rename("users.txt", "backup.txt") != 0){
        printf("Error renaming original file!\n");
        return;
    }

    if (rename("temp.txt", "users.txt") != 0) {
        printf("Error replacing temporary file!\n");

        //restore original file
        if(rename("backup.txt", "users.txt") != 0){
            printf("Error restoring original file!\n");
        }
        return;
    }

    if (remove("backup.txt") != 0) {
        printf("Error deleting original file!\n");
        return;
    }


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
        choice = getInt();

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
