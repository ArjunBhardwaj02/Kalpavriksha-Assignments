#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>

struct Student{
    char name[50];
    int rollno;
    int marks[3];
};

int totalMarks(struct Student *arr){
    int sum =0;
    for(int i=0;i<3;i++){
        sum += arr->marks[i];
    }
    return sum;
}

float avgMarks(int totalMarks){
    return totalMarks/3.0;
}

char assignGrade(float avgMarks){
    if(avgMarks >= 85)return 'A';
    else if(avgMarks >= 70)return 'B';
    else if(avgMarks >= 50)return 'C';
    else if(avgMarks >= 35)return 'D';
    else return 'F';
}

//check no. of stars according to grade
int countStars(char grade){
    int n=0;
    switch(grade){
        case 'A': n = 5;
        break;
        case 'B': n=4;
        break;
        case 'C': n=3;
        break;
        case 'D': n=2;
        break;
        default: n=0;
    }
    return n;
}

void displayPattern(int n){
    for(int i=0;i<n;i++){
        printf("*");
    }
}

//printing rollno via recursion
void printRollno(struct Student * arr, int i, int n){
    if(i>= n)return;
    printf("%d ", (arr + i)->rollno);
    printRollno(arr, i+1, n);
}

//input number of student
int numStudent(){
    int n;
    char input[100];
    char extra;

    while(1){
        printf("Enter Number of Students: ");
        if(fgets(input, sizeof(input), stdin) == NULL){
            printf("Error Reading the input!");
            return -1;
        }

        if(sscanf(input, "%d %c", &n , &extra) != 1){
            printf("Invalid input! Enter an Integer Only!\n");
            continue;
        }

        //validate range
        if(n < 1 || n > 100){
            printf("Number of Students must lie between 1 - 100\n");
            continue;
        }

        break;
    }
    return n;
}

//check marks
int readMarks(int marks) {
    if (marks < 0 || marks > 100) {
        printf("Marks must be between 0 and 100.\n");
        return 0;
    }
    return 1;
}

//check name validity
int validName(char *name){
    for(int i=0;name[i] != '\0';i++){
        if(!isalpha((unsigned char) name[i]))return 0;
    }
    return 1;
}


//input the students
int inputStudents(struct Student * arr, int n){
    char input[200];
    char extra;
    for(int i=0;i<n;i++){
    while(1){
        printf("Enter RollNo%d, Name%d, Marks1, Marks2, Marks3: ", i+1, i+1);
        if(fgets(input, sizeof(input), stdin) == NULL){
            printf("Error Taking input!");
            return 0;
        }

        
        if(sscanf(input, "%d %49s %d %d %d %c", &arr[i].rollno, arr[i].name, &arr[i].marks[0], &arr[i].marks[1], &arr[i].marks[2], &extra) != 5){
            printf("Enter all 5 fields correctly!\n");
            continue;
        }

        if(!validName(arr[i].name)){
            printf("Invalid Name! Use alphbetic Characters only.\n");
            continue;
        }

        if(arr[i].rollno < 1){
            printf("Roll Number must be atleast 1.\n");
            continue;
        }

        int valid = 1;

        for (int j = 0; j < 3; j++) {
            if (!readMarks(arr[i].marks[j])) {
                valid = 0;
                break;
         }
        }

        if (!valid) {
            printf("Please re-enter all five fields.\n");
            continue;
        }       
        
            break;
    }
}
    return 1;
}

void displayStudentReport(struct Student * arr, int n){
        for(int i=0;i<n;i++){
        //print details
        printf("\nRoll No.: %d\nName: %s", arr[i].rollno, arr[i].name);

        //find total marks
        int totalmarks = totalMarks(&arr[i]);
        printf("\nTotal Marks: %d",totalmarks);
        //find avgMarks
        float avgmarks = avgMarks(totalmarks);
        printf("\nAverage Marks: %f", avgmarks);
        //assign Grades
        char grade = assignGrade(avgmarks);
        printf("\nGrade: %c", grade);
        //display the pattern
        if(avgmarks < 35){
        printf("\nStudent's average below 35, so no pattern print!\n");
        continue;
        }
        int numStars = countStars(grade);
        printf("\nPerformance Pattern: ");
        displayPattern(numStars);
        printf("\n");
    }
}

int main(){
    //no. of students - use array to store it
    int n = numStudent();
    if(n == -1)return 0;

    struct Student * arr = malloc(n*sizeof(*arr));
    if(arr == NULL){
        printf("Memory Allocation failed!");
        return 1;
    }
    //input the students into the array
    if(!inputStudents(arr, n)){
        printf("Error Reading Students Data");
        free(arr);
        return 1;
    }

    //display student report
    displayStudentReport(arr, n);

    //use recursion to print rollnos
    printf("\nList of Roll Numbers: ");
    printRollno(arr,0,n);

    //free the memory
    free(arr);
    return 0;
}
