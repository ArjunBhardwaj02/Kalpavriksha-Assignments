#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<ctype.h>
#include<limits.h>

#define NAME_SIZE 100
#define DEG_SIZE 50

struct Employee{
    int id;
    char name[NAME_SIZE];
    char designation[DEG_SIZE];

    struct Employee *manager;
    struct Employee *firstReport; //to check which employee reports first to manager or upper hierarcy
    struct Employee *nextReport;
};
int getInt(){
    char input[NAME_SIZE];
    char extra;
    int n;

    while(1){
    if(fgets(input, sizeof(input), stdin) ==NULL){
        printf("Error taking input");
        return -1;
    }

    if(strchr(input, '\n') == NULL){
        int ch = getchar();
        if(ch != '\n' && ch != EOF){
            while((ch= getchar()) != '\n' && ch != EOF);
            printf("\nInput Too long!");
            continue;
        }
    }

    if(sscanf(input, "%d %c", &n , &extra) != 1){
        printf("\nEnter only integer!");
        printf("\nEnter Employee ID: ");
        continue;
    }

        break;
    }

    return n;
}

void freeEmployees(struct Employee *root){
    if(root == NULL)return;

    freeEmployees(root->firstReport);
    freeEmployees(root->nextReport);

    free(root);
}

int getName(struct Employee *e){
    char input[NAME_SIZE];

    while(1){
    if(fgets(input, sizeof(input), stdin) == NULL){
        printf("Error taking Input");
        return -1;
    }

    if(strchr(input, '\n') == NULL){
        int ch = getchar();
        if(ch != '\n' && ch != EOF){
            while((ch= getchar()) != '\n' && ch != EOF);
            printf("\nInput Too long!");
            continue;
        }
    }
    
    input[strcspn(input, "\n")] = '\0';

    int found =0;
    int valid = 1;

    for(int i=0; input[i] != '\0'; i++){
        char c = (unsigned char)input[i];

        if(!isalpha(c) && !isspace(c)){
            valid = 0;
            break;
        } 
        if(!isspace(c))found = 1;
    }

    if(!valid){
    printf("\nEnter only Letters");
    printf("\nEnter: ");
    continue;
}

    if(!found){
        printf("\nString can't be empty!");
        continue;
    }
    break;
}

    strcpy(e->name, input);

    return 1;
}

int getDesignation(struct Employee *e){
    char input[DEG_SIZE];

    while(1){
    if(fgets(input, sizeof(input), stdin) == NULL){
        printf("Error taking Input");
        return -1;
    }

    if(strchr(input, '\n') == NULL){
        int ch = getchar();
        if(ch != '\n' && ch != EOF){
            while((ch= getchar()) != '\n' && ch != EOF);
            printf("\nInput Too long!");
            continue;
        }
    }

    input[strcspn(input, "\n")] = '\0';

    int found =0;
    int valid = 1;

    for(int i=0; input[i] != '\0'; i++){
        char c = (unsigned char)input[i];

        if(!isalpha(c) && !isspace(c)){
            valid = 0;
            break;
        } 
        if(!isspace(c))found = 1;
    }

    if(!valid){
    printf("\nEnter only Letters");
    printf("\nEnter: ");
    continue;
}

    if(!found){
        printf("\nString can't be empty!");
        continue;
    }
    break;
}

    strcpy(e->designation, input);

    return 1;
}

//find employee
struct Employee * findEmployee(struct Employee *e, int id){
    if(e == NULL)return NULL;

    if(e->id == id)return e;

    struct Employee* a= findEmployee(e->firstReport, id);
    struct Employee * b = findEmployee(e->nextReport, id);

    return a != NULL ? a : b;
}

//add employee
int addEmployee(struct Employee **root)
{
    int id, managerId;
    struct Employee *newEmployee;
    struct Employee *manager;

    printf("\nEnter employee ID: ");
    id = getInt();

    if (id == -1)
        return -1;

    if(id <= 0){
        printf("\nID must be greater than 0");
        return 0;
    }

    if (findEmployee(*root, id) != NULL) {
        printf("Employee ID already exists!\n");
        return 0;
    }

    newEmployee = malloc(sizeof(struct Employee));

    if (newEmployee == NULL) {
        printf("Memory allocation failed!\n");
        return -1;
    }

    newEmployee->id = id;
    newEmployee->manager = NULL;
    newEmployee->firstReport = NULL;
    newEmployee->nextReport = NULL;

    printf("Enter employee name: ");
    if (getName(newEmployee) == -1) {
        free(newEmployee);
        return -1;
    }

    printf("Enter designation: ");
    if (getDesignation(newEmployee) == -1) {
        free(newEmployee);
        return -1;
    }

    /* Add the CEO as the root employee */
    if (*root == NULL) {
        newEmployee->manager = NULL;
        *root = newEmployee;

        printf("CEO added successfully!\n");
        return 1;
    }

    printf("Enter reporting manager ID: ");
    managerId = getInt();

    if (managerId == -1) {
        free(newEmployee);
        return -1;
    }

    manager = findEmployee(*root, managerId);

    if (manager == NULL) {
        printf("Manager not found!\n");
        free(newEmployee);
        return 0;
    }

    newEmployee->manager = manager;
    newEmployee->nextReport = manager->firstReport;
    manager->firstReport = newEmployee;

    printf("Employee added successfully!\n");
    return 1;
}

//remove employee
int countDirectReport(struct Employee *e){
    if(e == NULL)return 0;

    int a = countDirectReport(e->nextReport);
    return 1 + a;
}

struct Employee *findBestSibling(struct Employee *curr, struct Employee * target, struct Employee *best, int *minReports){
    if(curr == NULL)return best;

    if(curr != target){
        int count = countDirectReport(curr->firstReport);
        if(count < *minReports){
            *minReports = count;
            best = curr;
        }
    }

    return findBestSibling(curr->nextReport, target, best, minReports);
}

//find last employee in sibling list
struct Employee* findLastReport(struct Employee * e){
    if(e == NULL || e->nextReport == NULL)return e;

    return findLastReport(e->nextReport);
}

// Update the manager of every transferred employee
void updateManager(struct Employee *e, struct Employee *manager)
{
    if (e == NULL)
        return;

    e->manager = manager;
    updateManager(e->nextReport, manager);
}

// Remove an employee from a manager's report list
void unlinkEmployee(struct Employee **head, struct Employee *target)
{
    if (*head == NULL)
        return;

    if (*head == target) {
        *head = target->nextReport;
        return;
    }

    unlinkEmployee(&((*head)->nextReport), target);
}


/*
 * Employee Removal Policy:
 *
 * 1. The CEO cannot be removed.
 * 2. When an employee is removed, their direct reportees
 *    are transferred to the sibling having the fewest
 *    direct reportees.
 * 3. If no sibling exists, the reportees are transferred
 *    to the removed employee's manager.
 * 4. The manager pointers of all transferred employees
 *    are updated accordingly.
 */


int removeEmployee(struct Employee **root, int id){
    //check employee id
    struct Employee * target = findEmployee(*root , id);
    if(target == NULL){
        printf("\nEmployee Don't Exits");
        return 0;
    }

    //check for ceo
    if((*root) == target){
        printf("\nCEO Can't be removed");
        return 0;
    }

    struct Employee *manager= target->manager;
    struct Employee *reportees = target->firstReport;

    int minReports = INT_MAX;

    struct Employee *best = findBestSibling(manager->firstReport, target, NULL, &minReports);

    //detach target reportees
    target->firstReport = NULL;

    // Unlink target from the manager's report list
    unlinkEmployee(&manager->firstReport, target);

    // Transfer target's reportees
    if (reportees != NULL) {
        // If no sibling exists, transfer to the manager
        struct Employee *destination =
            (best != NULL) ? best : manager;

        // Update every transferred employee's manager
        updateManager(reportees, destination);

        // Attach the reportees to the destination
        struct Employee *last = findLastReport(reportees);

        last->nextReport = destination->firstReport;
        destination->firstReport = reportees;
    }

    free(target);

    printf("\nEmployee removed successfully.");
    return 1;

}


//count employee
int countEmployee(struct Employee * root){
    if(root == NULL)return 0;

    int a = countEmployee(root->firstReport);
    int b = countEmployee(root->nextReport);

    return a+b + 1;
}

//display hierarchy
void displayHierarchy(struct Employee * root, int showSibling){
    if(root == NULL)return;

    printf("%d - %s - %s\n", root->id, root->name, root->designation);
    displayHierarchy(root->firstReport, 1);

    if(showSibling)displayHierarchy(root->nextReport, 1);
}

//display complete Hierarchy
void displayCompleteHierarchy(struct Employee *root){
    if(root == NULL)return;
    
    displayHierarchy(root,0);
    
    printf("Total Employees: %d\n", countEmployee(root));
}


int main(){
    struct Employee *root = NULL;

    int n;
    while (1)
    {
        printf("1. Add Employee\n");
        printf("2. Remove Employee\n");
        printf("3. Display Employee Hierarchy\n");
        printf("4. Display Complete Hierarchy\n");
        printf("5. Exit\n");

        n = getInt();

        if(n == -1){
            printf("\nInput Ended!");
            freeEmployees(root);
            return 0;
        }

        switch(n){
            case 1:{ int a = addEmployee(&root);
            if(a == -1){
                freeEmployees(root);
                return 1;
            }
            else if(a == 0)break;
            break;
            }
            
            case 2: { printf("\nEnter Employee ID to remove: ");
                int id= getInt();
                if(id == -1){
            freeEmployees(root);
            return 0;
            }   
            int a= removeEmployee(&root, id);
            if(a==0)break;
            break;
            }
            case 3:{
                int id;
                printf("\nEnter Employee ID: ");
                id = getInt();
                if(id == -1){
                freeEmployees(root);
                return 0;
                }

                struct Employee * e= findEmployee(root, id);
                if(e == NULL)printf("Employee Not found!");
                else{ displayHierarchy(e,0);
                    int count = countEmployee(e->firstReport);
                    printf("Total employees reporting under %d: %d\n", e->id, count);
                }
            break;
            }
            
            case 4: 
            displayCompleteHierarchy(root);
            break;

            case 5: 
            freeEmployees(root);
            return 0;

            default: printf("Enter options from 1 -5 \n");
    }
}
    

    return 0;
}
