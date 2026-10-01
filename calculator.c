#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include<stdbool.h>

# define INITIAL_CAPACITY 1000

bool allocate_memory_values(int *vcap, int **ptr){
    int *temp = realloc(*ptr, sizeof(int) * (*vcap) * 2);
    if (temp == NULL) {
        return false;
    }
    *vcap *= 2;
    *ptr = temp;
    return true;
}

bool allocate_memory_opr(int *vcap, char **ptr){
    char *temp = realloc(*ptr, sizeof(char) * (*vcap) * 2);
    if (temp == NULL) {
        return false;
    }
    *vcap *= 2;
    *ptr = temp;
    return true;
}

bool apply_operator(int a, int b, char op, int *result){
    if(op == '+')*result = a + b;
    else if(op == '-')*result = a-b;
    else if(op == '*')*result = a*b;
    else{
        if(b==0){
            return false;
        }else *result = a / b;
    }
    return true;
}

int main() {
    char input[INITIAL_CAPACITY];
    printf("Enter the Expression: ");
    if(fgets(input, sizeof(input), stdin) == NULL){
        printf("Error: Failed to read the input");
        return 0;
    }

    int *values = malloc(sizeof(int) * INITIAL_CAPACITY);
    char *opr = malloc(sizeof(char) * INITIAL_CAPACITY);

    if(values == NULL || opr == NULL){
        printf("Error: Memory allocation Failed");
        free(values);
        free(opr);
        return 0;
    }

    int vcapacity = INITIAL_CAPACITY;
    int ocapacity = INITIAL_CAPACITY;

    int v = 0;
    int o = 0;

    int expectingNumber = 1;

    for (int i = 0; input[i] != '\0' && input[i] != '\n'; i++) {

        if (isspace((unsigned char)input[i]))
            continue;

        // CASE 1: NUMBER
        else if (isdigit(input[i])) {

            // Two numbers without an operator
            if (!expectingNumber) {
                printf("Error: Invalid expression.\n");
                free(values);
                free(opr);
                return 0;
            }

            int num = 0;

            while (isdigit(input[i])) {
                num = num * 10 + (input[i] - '0');
                i++;
            }

            i--;

            if (v == vcapacity) {
                if(!allocate_memory_values(&vcapacity, &values)){
                    printf("Error: Memory allocation Failed");
                    free(values);
                    free(opr);
                    return 0;
                }
            }

            values[v++] = num;

            expectingNumber = 0;
        }

        // CASE 2: OPERATOR
        else if (input[i] == '+' || input[i] == '-' ||
                 input[i] == '*' || input[i] == '/') {

            // Operator cannot come when expecting a number
            if (expectingNumber) {
                printf("Error: Invalid expression.\n");
                free(values);
                free(opr);
                return 0;
            }

            // CASE 2.1: operator stack empty
            if (o == 0) {

                if (o == ocapacity) {
                    if(!allocate_memory_opr(&ocapacity, &opr)){
                        printf("Error: Memory allocation Failed");
                        free(values);
                        free(opr);
                        return 0;
                    }
                }

                opr[o++] = input[i];
            }

            // CASE 2.2: current has HIGH precedence
            else if ((input[i] == '*' || input[i] == '/') &&
                     (opr[o - 1] == '+' || opr[o - 1] == '-')) {

                if (o == ocapacity) {
                    if(!allocate_memory_opr(&ocapacity, &opr)){
                        printf("Error: Memory allocation Failed");
                        free(values);
                        free(opr);
                        return 0;
                    }
                }

                opr[o++] = input[i];
            }

            // CASE 2.3: current has LOW precedence
            else if ((input[i] == '+' || input[i] == '-') &&
                     (opr[o - 1] == '*' || opr[o - 1] == '/')) {

                // First calculate all * and /
                while (o > 0 &&
                       (opr[o - 1] == '*' || opr[o - 1] == '/')) {

                    int b = values[--v];
                    int a = values[--v];

                    char op = opr[--o];

                    int result;
                    if(!apply_operator(a,b,op, &result)){
                        printf("Error: Division by zero");
                        free(values);
                        free(opr);
                        return 0;
                    }

                    values[v++] = result;
                }

                // Then calculate pending + or -
                if (o > 0 &&
                    (opr[o - 1] == '+' || opr[o - 1] == '-')) {

                    int b = values[--v];
                    int a = values[--v];

                    char op = opr[--o];

                    int result;

                    if(!apply_operator(a,b,op, &result)){
                        printf("Error: Division by zero");
                        free(values);
                        free(opr);
                        return 0;
                    }

                    values[v++] = result;
                }

                if (o == ocapacity) {
                    if(!allocate_memory_opr(&ocapacity, &opr)){
                        printf("Error: Memory allocation Failed");
                        free(values);
                        free(opr);
                        return 0;
                    }
                }

                opr[o++] = input[i];
            }

            // CASE 2.4: same precedence + and -
            else if ((input[i] == '+' || input[i] == '-') &&
                     (opr[o - 1] == '+' || opr[o - 1] == '-')) {

                int b = values[--v];
                int a = values[--v];

                char op = opr[--o];

                int result;

                if(!apply_operator(a,b,op, &result)){
                        printf("Error: Division by zero");
                        free(values);
                        free(opr);
                        return 0;
                    }

                values[v++] = result;

                if (o == ocapacity) {
                    if(!allocate_memory_opr(&ocapacity, &opr)){
                        printf("Error: Memory allocation Failed");
                        free(values);
                        free(opr);
                        return 0;
                    }
                }

                opr[o++] = input[i];
            }

            // CASE 2.5: same precedence * and /
            else if ((input[i] == '*' || input[i] == '/') &&
                     (opr[o - 1] == '*' || opr[o - 1] == '/')) {

                int b = values[--v];
                int a = values[--v];

                char op = opr[--o];

                int result;

                if(!apply_operator(a,b,op, &result)){
                        printf("Error: Division by zero");
                        free(values);
                        free(opr);  
                        return 0;
                    }

                values[v++] = result;

                if (o == ocapacity) {
                    if(!allocate_memory_opr(&ocapacity, &opr)){
                        printf("Error: Memory allocation Failed");
                        free(values);
                        free(opr);
                        return 0;
                    }
                }

                opr[o++] = input[i];
            }

            else {
                printf("Error: Invalid expression.\n");
                free(values);
                free(opr);
                return 0;
            }

            // After an operator, we must get a number
            expectingNumber = 1;
        }

        // CASE 3: INVALID CHARACTER
        else {
            printf("Error: Invalid expression.\n");
            free(values);
            free(opr);
            return 0;
        }
    }

    // Empty expression or expression ending with operator
    if (expectingNumber) {
        printf("Error: Invalid expression.\n");
        free(values);
        free(opr);
        return 0;
    }

    // Process remaining operators
    while (o > 0) {

        int b = values[--v];
        int a = values[--v];

        char op = opr[--o];

        int result;

        if(!apply_operator(a,b,op, &result)){
                        printf("Error: Division by zero");
                        free(values);
                        free(opr);
                        return 0;
                    }

        values[v++] = result;
    }

    printf("%d\n", values[0]);

    free(values);
    free(opr);

    return 0;
}
