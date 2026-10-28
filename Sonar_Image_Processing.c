#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define MAXLEN 100
int getInt(){
    char input[MAXLEN];
    char extra;
    int n;

    while (1)
    {
        if(fgets(input, sizeof(input), stdin) == NULL){
            printf("Error Taking Input!");
            return 0;
        }
        if(strchr(input, '\n') == NULL){
            int ch;
            while((ch = getchar()) != '\n');
            printf("\nInput Buffer Overload!!");
            continue;
        }

        if(sscanf(input, "%d %c", &n , &extra) != 1){
            printf("\nEnter only Integer!");
            continue;
        }

        break;
    }
    return n;
}

void freeMatrix(int **mat, int n){
    if(mat == NULL)return;

    for(int i=0;i < n; i++){
        free(*(mat + i));
    }

    free(mat);
}

void generateMatrix(int **mat, int n){
    int min = 0, max = 255;

    for(int i=0;i<n;i++){
        for(int j =0;j<n;j++){
            *(*(mat + i) + j) = (rand() % (max - min + 1)) + min;
        }
    }
}

void displayMatrix(int **mat, int n){
    for(int i=0;i<n;i++){
        for(int j=0;j < n; j++){
            printf("%d ", *(*(mat + i) + j));
        }
        printf("\n");
    }
}

void swapMatrix(int **mat, int n){
    for(int i=0;i < n;i++){
        for(int j=i+1;j<n;j++){
            int temp = *(*(mat + i) + j);
            *(*(mat + i) + j) = *(*(mat + j) + i);
            *(*(mat + j) + i) = temp;
        }
    }
}

void reverse(int **mat ,int n){
    for(int i=0;i<n;i++){
        int j=0, k = n -1;
        while(j < k){
            int v1 = *(*(mat + i) + j);
            int v2 = *(*(mat + i) + k);

            *(*(mat + i) + j) = v2;
            *(*(mat + i) + k) = v1;

            j++,k--;
        }
    }
}

void rotate(int **mat, int n){
    swapMatrix(mat, n);
    reverse(mat, n);
}

void applyFilter(int **mat , int n){
    if (n == 1) {
        return;
    }

    int *prev = malloc(n * sizeof(int));
    int *curr = malloc(n * sizeof(int));
    int *next = malloc(n * sizeof(int));

    if(prev == NULL || curr == NULL || next == NULL){
        printf("\nMemory Allocation failed!");
        free(prev);
        free(curr);
        free(next);
        return;
    }

    //current
    for (int k = 0; k < n; k++) {
    *(curr + k) = *(*(mat + 0) + k);
    }

    //next
    if (n > 1) {
        for (int k = 0; k < n; k++) {
        *(next + k) = *(*(mat + 1) + k);
        }
    }

    for(int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            int sum =0, count =0;

            int startRow = (i>0) ? i-1 : 0;
            int endRow = (i<n-1) ? i+1 : n-1;

            int startCol = (j > 0) ? j - 1 : 0;
            int endCol = (j < n - 1) ? j + 1 : n - 1;

            for(int p = startRow; p<=endRow; p++){
                for(int q = startCol; q <= endCol; q++){
                   if(p == i-1)sum += *(prev + q);
                   else if(p == i) sum += *(curr + q);
                   else sum += *(next + q);
                   count ++;
                }
            }
            *(*(mat + i) + j) = sum / count;
        }

        int *temp = prev;
        prev = curr;
        curr = next;
        next = temp;

        //set next row
        if (i + 2 < n) {
            for (int k = 0; k < n; k++) {
            *(next + k) = *(*(mat + i + 2) + k);
            }
        }
    }

    free(prev);
    free(curr);
    free(next);
}

int main(){
    int n;
    printf("\nEnter integer n: ");
    n = getInt();
    if(n == 0)return 1;

    while (n < 2 || n > 10) 
    {
        printf("\nValue of n must lie in range 2 - 10");
        printf("\nEnter n: ");
        n = getInt();
    }
    
    int **mat = malloc(n * sizeof(*mat));

    if(mat == NULL){
        printf("\nMemory Allocation Failed");
        return 1;
    }

    for(int row = 0; row < n; row++){
        *(mat+row) = malloc(n * sizeof(**mat));
        if(*(mat + row) == NULL){
            printf("\nMemory Allocation Failed!");

            for (int i = 0; i < row; i++) {
                free(*(mat + i));
            }

            free(mat);
            return 1;
        }
    }

    printf("\nOriginal Matrix: \n");
    generateMatrix(mat, n);
    displayMatrix(mat, n);

    printf("\nMatrix after 90deg Rotation: \n");
    rotate(mat, n);
    displayMatrix(mat, n);

    printf("\nMatrix after applying 3X3 Smoothing filter: \n");
    applyFilter(mat, n);
    displayMatrix(mat ,n);

    freeMatrix(mat, n);

    return 0;
}