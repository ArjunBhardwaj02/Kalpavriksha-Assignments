#include<stdio.h>
#include<stdlib.h>

struct User{
    int id;
    char name[50];
    int age;
};

void createUser(){
    FILE *fp = fopen("users.txt", "a");
    if(fp == NULL){
        printf("Error opening file!\n");
        return;
    }

    struct User user;
    printf("Enter user ID: ");
    scanf("%d", &user.id);
    
    getchar(); // consume newline character left by scanf
    printf("Enter user name: ");

    scanf("%s", user.name);
    getchar(); 

    printf("Enter user age: ");
    scanf("%d", &user.age);
    getchar(); 

    fprintf(fp, "%d %s %d\n", user.id, user.name, user.age);
    fclose(fp);
}

void readUsers(){
    FILE *fp = fopen("users.txt", "r");
    if(fp == NULL){
        printf("Error opening file!\n");
        return;
    }

    struct User user;
    while(fscanf(fp, "%d %49s %d", &user.id, user.name, &user.age) == 3){
        printf("ID: %d, Name: %s, Age: %d\n", user.id, user.name, user.age);
    }
    fclose(fp);
}

void updateUser(){
    FILE *fp = fopen("users.txt", "r+");
    FILE *temp = fopen("temp.txt", "w");
    if(fp == NULL || temp == NULL){
        printf("Error opening file!\n");
        return;
    }

    int id;
    printf("Enter user ID to update: ");
    scanf("%d", &id);
    getchar();

    struct User user;
    int found = 0;
    while(fscanf(fp, "%d %49s %d", &user.id, user.name, &user.age) == 3){
        if(user.id == id){
            printf("Enter new name: ");
            scanf("%s", user.name);
            printf("Enter new age: ");
            scanf("%d", &user.age);
            getchar(); 
            found = 1;
        }
        fprintf(temp, "%d %s %d\n", user.id, user.name, user.age);
    }   
    if(!found){
        printf("User with ID %d not found.\n", id);
    }

    fclose(fp);
    fclose(temp);

    remove("users.txt");
    rename("temp.txt", "users.txt");
}

void deleteUser(){
    FILE *fp = fopen ("users.txt", "r");
    FILE *temp = fopen("temp.txt", "w");
    if(fp == NULL || temp == NULL){
        printf("Error opening file!\n");
        return;
    }

    int id;
    printf("Enter user ID to delete: ");
    scanf("%d", &id);
    getchar(); 

    struct User user;
    int found = 0;
    while(fscanf(fp, "%d %49s %d", &user.id, user.name, &user.age) == 3){
        if(user.id != id){
            fprintf(temp, "%d %s %d\n", user.id, user.name, user.age);
        } else {
            found = 1;
        }
    }
    if(!found){
        printf("User with ID %d not found.\n", id);
    }
    fclose(fp);
    fclose(temp);
    remove("users.txt");
    rename("temp.txt", "users.txt");
}

int main(){
    int choice;
    do{
        printf("1. Create User\n");
        printf("2. Read Users\n");
        printf("3. Update User\n");
        printf("4. Delete User\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); 

        switch(choice){
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
                printf("Invalid choice! Please try again.\n");
        }
    } while(choice != 5);

    return 0;
}