#include<stdio.h>
#include<stdlib.h>
#include<ctype.h>
#include<string.h>

struct Customer{
    char name[100];
    int N_tickets;  //no. of tickets
    int ticket_category;
};

const int STANDARD = 150;
const int PREMIUM = 250;
const int RECLINER = 400;

//read customer name
int getCustomerName(struct Customer * s){
    char input[100];

    if(fgets(input, sizeof(input), stdin) == NULL){
        printf("Error fetching name!");
        return 0;
    }
    //remove the new line injected using fgets
    input[strcspn(input,"\n")] = '\0';

    //check empty name
    int flag = 0; // if atleast 1 character is found then name is not empty

    //check if user entered correct name structure
    for(int i=0;input[i] !='\0';i++){
        if(!isalpha((unsigned char)input[i]) && !isspace((unsigned char)input[i])){
            printf("\nName must contain only letters!");
            printf("\nEnter Name: ");
           return getCustomerName(s);
        }
        if(!isspace((unsigned char)input[i])){
            flag =1;
        }
    }

    if(!flag){
        printf("\nName Cannot be empty!");
        printf("\nEnter Name: ");
        return getCustomerName(s);
    }

    strcpy(s->name, input);
    
    return 1;
}

// read no. of tickets - must be b/w 1 - 20
int getTicketCount(struct Customer *s){
    int n;
    char input[100];
    char extra;
    //check input validty
    if(fgets(input, sizeof(input), stdin) == NULL){
        printf("Error Taking input");
        return 0;
    }
    if(sscanf(input, "%d %c", &n , &extra) != 1){
        printf("\nInput must be Number only!");
        printf("\nEnter Number of Tickets: ");
        return getTicketCount(s);
    }
    
    if(n < 1 || n > 20){
        printf("\nNo. of Tickets must lie in range 1 - 20");
        printf("\nEnter Number of Tickets: ");
        return getTicketCount(s);
    }

    s->N_tickets = n;
    return 1;
}

//get ticket category - category is 1 -3
int getTicketCategory(struct Customer *s){
    int n;
    char input[100];
    char extra; 
    //check input validity
    if(fgets(input, sizeof(input), stdin) == NULL){
        printf("Error Taking input!");
        return 0;
    }

    if(sscanf(input, "%d %c", &n, &extra) != 1){
        printf("\nEnter only Number!");
        printf("\nEnter Ticket Category: ");
        return getTicketCategory(s);
    }
    
    if(n < 1 || n > 3){
        printf("\nTicket Category Must lie between 1 - 3");
        printf("\nEnter Ticket Category: ");
        return getTicketCategory(s);
    }

    s->ticket_category = n;
    return 1;
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
double subtotal(struct Customer *s){
    int num_tickets = s->N_tickets;
    int price = getTicketPrice(s);
    if(price == -1){
        printf("Error while fetching ticket price!");
        return 0;
    }

    return price * num_tickets;
}

//get discount
int getDiscount(int ticketCount){
    if(ticketCount >=5 )return 10;
    else return 0;
}

//print summary
void displayBill(struct Customer *c, double Subtotal, int discount){
    printf("\nCustomer: %s", c->name);
    switch(c->ticket_category){
        case 1: printf("\nTicket Category: Standard"); break;
        case 2: printf("\nTicket Category: Premium"); break;
        case 3: printf("\nTicket Category: Recliner"); break;
        default: printf("Error!!"); return;
    }

    printf("\nTickets: %d",c->N_tickets);
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

    printf("\nEnter Number of Tickets: ");
    int d = getTicketCount(c);
    if(!d){
        free(c);
        return 1;
    }
    
    printf("\nEnter Ticket Category: ");
    int b =getTicketCategory(c);
    if(!b){
        free(c);
        return 1;
    }


    double Subtotal = subtotal(c);
    if(!Subtotal){
        free(c);
        return 1;
    }

    int discount = getDiscount(c->N_tickets);
    
    displayBill(c,Subtotal, discount);

    free(c);

    return 0;
}