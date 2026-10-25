#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>
#include<errno.h>
#include<limits.h>

struct Customer{
    char name[100];
    int num_of_tickets;  //no. of tickets
    int ticket_category;
};

const int STANDARD = 150;
const int PREMIUM = 250;
const int RECLINER = 400;

int getInt(){
    int n;
    char input[100];

    if(fgets(input, sizeof(input), stdin) == NULL){
        printf("\nError Taking input!");
        return -1;
    }

     // Detect overlong input and discard remaining characters
    if (strchr(input, '\n') == NULL && !feof(stdin)) {
        int ch;

        while ((ch = getchar()) != '\n' && ch != EOF);

        printf("\nInput is too long!");
        return 0;
    }

    errno =0;
    char *endptr;

    long value = strtol(input, &endptr, 10);

    if(errno == ERANGE){
        printf("\nInput outof range!");
        return 0;
    }

    if(endptr == input){
        printf("\nEnter only Numeric value!");
        return 0;
    }

    //skip trailing whitespace
    while (isspace((unsigned char)*endptr)) {
            endptr++;
        }
    
    //skip trailing characters
    if(*endptr != '\0'){
        printf("\nEnter only Numeric Value");
        return 0;
    }

    // Check int range before conversion
        if (value < INT_MIN || value > INT_MAX) {
            printf("\nInput out of bound!");
            printf("\nEnter Ticket Category: ");
            return 0;
        }
    
        n = (int)value;

        return n;
}

//read customer name
int getCustomerName(struct Customer *s) {
    char input[100];

    while (1) {
        if (fgets(input, sizeof(input), stdin) == NULL) {
            printf("Error fetching name!");
            return 0;
        }

        // Detect overlong input and discard remaining characters
        if (strchr(input, '\n') == NULL ) {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);

            printf("\nName is too long!");
            printf("\nEnter Name: ");
            continue;
        }

        // Remove the newline
        input[strcspn(input, "\n")] = '\0';

        // Check whether the name contains at least one non-space character
        int flag = 0;

        for (int i = 0; input[i] != '\0'; i++) {
            if (!isalpha((unsigned char)input[i]) &&
                !isspace((unsigned char)input[i])) {
                printf("\nName must contain only letters!");
                printf("\nEnter Name: ");
                flag = -1;
                break;
            }

            if (!isspace((unsigned char)input[i])) {
                flag = 1;
            }
        }

        if (flag == -1) {
            continue;
        }

        if (flag == 0) {
            printf("\nName cannot be empty!");
            printf("\nEnter Name: ");
            continue;
        }

        strcpy(s->name, input);
        return 1;
    }
}

// read no. of tickets - must be b/w 1 - 20
int getTicketCount(struct Customer *s){
    int n;
    char extra;
    while (1)
    {
        printf("\nEnter Number of Tickets: ");
    n = getInt();
    if(n == 0)continue;
    else if(n == -1)return 0;
    
    if(n < 1 || n > 20){
        printf("\nNo. of Tickets must lie in range 1 - 20");
        printf("\nEnter Number of Tickets: ");
        continue;
    }

    s->num_of_tickets = n;
    return 1;
}
}

//get ticket category - category is 1 -3
int getTicketCategory(struct Customer *s){
    int n;

    while (1)
    {
    printf("\nEnter Ticket Category: ");
    n = getInt();
    if(n == -1)return 0;
    else if(n == 0)continue;

    if(n < 1 || n > 3){
        printf("\nTicket Category Must lie between 1 - 3");
        printf("\nEnter Ticket Category: ");
        continue;
    }

    s->ticket_category = n;
    return 1;
}
}

//get ticket price
int getTicketPrice(struct Customer *s){
    int category = s->ticket_category;

    switch(category){
        case 1: return STANDARD;
        case 2: return PREMIUM;
        case 3: return  RECLINER;
        default: return -1;
    }
}

//calculate bill
int subtotal(struct Customer *s, double * result){
    int num_tickets = s->num_of_tickets;
    int price = getTicketPrice(s);
    if(price == -1){
        return 0;
    }

    *result =  price * num_tickets;
    return 1;
}

//get discount
int getDiscount(int ticketCount){
    if(ticketCount >=5 )return 10;
    else return 0;
}

char *getCategoryName(int category){
    switch (category)
    {
    case 1:return "Standard";
    case 2: return "Premium";
    case 3: return "Recliner";
    default:
        return NULL;
    }
}

//print summary
void displayBill(struct Customer *c, double Subtotal, int discount){
    const char *categoryName = getCategoryName(c->ticket_category);
    if(categoryName == NULL){
        printf("Error!! Invalid ticket category.\n");
        return;
    }

    printf("\nCustomer: %s", c->name);
    printf("\nTicket Category: %s", categoryName);

    printf("\nTickets: %d",c->num_of_tickets);
    printf("\nSubtotal: %0.2lf", Subtotal);
    printf("\nDiscount: %0.2lf", (Subtotal * discount)/100);
    printf("\nFinal Bill: %0.2lf", Subtotal - (Subtotal * discount)/100);
}

int main(){
    struct Customer *c = malloc(sizeof(*c));
    if(c==NULL){
        printf("Memory Allocation Failed!");
        return 1;
    }
    printf("\nEnter the customer Name: ");
    int a = getCustomerName(c);
    if(!a){
        free(c);
        return 1;
    }

    int d = getTicketCount(c);
    if(d == 0){
        free(c);
        return 1;
    }
    
    int b =getTicketCategory(c);
    if(b == 0){
        free(c);
        return 1;
    }


    double Subtotal;

    if(!subtotal(c, &Subtotal)){
        printf("Error while fetching ticket price!");
        free(c);
        return 1;
    }

    int discount = getDiscount(c->num_of_tickets);
    
    displayBill(c,Subtotal, discount);

    free(c);

    return 0;
}
