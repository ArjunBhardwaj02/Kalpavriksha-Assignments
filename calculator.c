#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int main() {
    char input[1000];
    fgets(input, sizeof(input), stdin);

    int *values = malloc(sizeof(int) * 1000);
    char *opr = malloc(sizeof(char) * 1000);

    int vcapacity = 1000;
    int ocapacity = 1000;

    int v = 0;
    int o = 0;

    int expectingNumber = 1;

    for (int i = 0; input[i] != '\0' && input[i] != '\n'; i++) {

        if (input[i] == ' ')
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
                vcapacity *= 2;
                values = realloc(values, sizeof(int) * vcapacity);
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
                    ocapacity *= 2;
                    opr = realloc(opr, sizeof(char) * ocapacity);
                }

                opr[o++] = input[i];
            }

            // CASE 2.2: current has HIGH precedence
            else if ((input[i] == '*' || input[i] == '/') &&
                     (opr[o - 1] == '+' || opr[o - 1] == '-')) {

                if (o == ocapacity) {
                    ocapacity *= 2;
                    opr = realloc(opr, sizeof(char) * ocapacity);
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

                    if (op == '*') {
                        result = a * b;
                    }
                    else {
                        if (b == 0) {
                            printf("Error: Division by zero.\n");
                            free(values);
                            free(opr);
                            return 0;
                        }

                        result = a / b;
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

                    if (op == '+')
                        result = a + b;
                    else
                        result = a - b;

                    values[v++] = result;
                }

                if (o == ocapacity) {
                    ocapacity *= 2;
                    opr = realloc(opr, sizeof(char) * ocapacity);
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

                if (op == '+')
                    result = a + b;
                else
                    result = a - b;

                values[v++] = result;

                if (o == ocapacity) {
                    ocapacity *= 2;
                    opr = realloc(opr, sizeof(char) * ocapacity);
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

                if (op == '*') {
                    result = a * b;
                }
                else {
                    if (b == 0) {
                        printf("Error: Division by zero.\n");
                        free(values);
                        free(opr);
                        return 0;
                    }

                    result = a / b;
                }

                values[v++] = result;

                if (o == ocapacity) {
                    ocapacity *= 2;
                    opr = realloc(opr, sizeof(char) * ocapacity);
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

        if (op == '+') {
            result = a + b;
        }
        else if (op == '-') {
            result = a - b;
        }
        else if (op == '*') {
            result = a * b;
        }
        else {
            if (b == 0) {
                printf("Error: Division by zero.\n");
                free(values);
                free(opr);
                return 0;
            }

            result = a / b;
        }

        values[v++] = result;
    }

    printf("%d\n", values[0]);

    free(values);
    free(opr);

    return 0;
}