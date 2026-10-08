#include<stdio.h>
#include<stdlib.h>

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

void displayPattern(char grade){
    int n=0;
    if(grade == 'A')n=5;
    else if(grade == 'B')n=4;
    else if(grade == 'C')n =3;
    else if(grade == 'D')n = 2;
    
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


int main(){
    //no. of students - use array to store it
    int n;
    printf("Enter the No. of Students: ");
    scanf("%d", &n);

    //check if n is not positive
    while(n < 1){
        printf("No. of Students must be greater than or equals to 1\n");
        printf("Enter the No. of Students: ");
        scanf("%d", &n);
    }

    //check if n is greater than 100
    while(n>100){
        printf("No. of Students must be less than or equals to 100\n");
        printf("Enter the No. of Students: ");
        scanf("%d", &n);
    }

    struct Student * arr = malloc(n*sizeof(struct Student));
    if(arr == NULL){
        printf("Memory Allocation failed!");
        free(arr);
        return 0;
    }
    //input the students into the array
    for(int i=0;i<n;i++){
        printf("Enter Rollno%d, Name%d, Marks1, Marks2, Marks3: ", i+1,i+1);
        scanf("%d %s %d %d %d", &arr[i].rollno, arr[i].name, &arr[i].marks[0], &arr[i].marks[1], &arr[i].marks[2]);

        //check if rollno entered is less than or equal to 0
        while(arr[i].rollno < 1){
            printf("Roll Number should be greater than or equal to 1\n");
            printf("Enter Roll No.: ");
            scanf("%d", &arr[i].rollno);
        }

        //check if marks entered is b/w 0-100
         while ((arr[i].marks[0] < 0 || arr[i].marks[0] > 100) ||
               (arr[i].marks[1] < 0 || arr[i].marks[1] > 100) ||
               (arr[i].marks[2] < 0 || arr[i].marks[2] > 100)) {
            printf("Marks must be between 0 and 100!\n");
            printf("Enter Marks again: ");
            scanf("%d %d %d", &arr[i].marks[0], &arr[i].marks[1], &arr[i].marks[2]);
        }
    }
    //using an iterative loop to traverse along the array
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
        printf("Student's average below 35, so no pattern print!\n");
        continue;
        }
        printf("\nPerformance Pattern: ");
        displayPattern(grade);
        printf("\n");
    }

    //use recursion to print rollnos
    printf("\nList of Roll Numbers: ");
    printRollno(arr,0,n);

    //free the memory
    free(arr);
    return 0;
}