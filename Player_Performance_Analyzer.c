#include<stdio.h>
#include<stdlib.h>
#include "Players_data.h"
#include<string.h>

int getInt(){
    int n;
    char input[100];
    char extra;

    if(fgets(input, sizeof(input), stdin) == NULL){
        printf("\nError Taking input!");
        return -1;
    }

     if(sscanf(input, "%d %c", &n, &extra) != 1){
        printf("\nEnter only integer");
        return -1;
     }

    return n;
}

int getFloat(float *value){

    char input[100];
    char extra;

    if(value == NULL)return 0;

    if(fgets(input, sizeof(input), stdin) == NULL){
        printf("\nError Taking Input!");
        return 0;
    }

    if(sscanf(input, "%f %c", value, &extra) != 1){
        printf("\nEnter a valid number!");
        return 0;
    }

    return 1;
}

typedef enum{
    BATSMEN,
    BOWLER,
    ALL_ROUNDER
} Role;

typedef enum{
    INDIA = 1,
    AUSTRALIA,
    BANGLADESH,
    ENGLAND,
    NEW_ZEALAND,
    PAKISTAN,
    SOUTH_AFRICA,
    SRI_LANKA,
    AFGHANISTAN,
    WEST_INDIES
} TeamId;

typedef struct Player{
    int id;
    char Name[50];
    char TeamName[50];
    Role Role;
    int TotalRun;
    float BattingAvg;
    float StrikeRate;
    int Wickets;
    float EconomyRate;
    float PerformanceIndex;

    struct Player *nextPlayer; //for connecting next player in LL
    struct Player *nextRole; //for connecting next player according to the role in LL
} PlayerRecord;

typedef struct Team{
    TeamId id;
    char TeamName[50];
    int totalPlayer;
    float AvgBattingStrikeRate;
    
    PlayerRecord * playersHead; //for head of LL denoting Players
    PlayerRecord * roleHeads[3]; //each head for each role in game -> 1 for batsmen, 1 for bowler, 1 for all rounder

    PlayerRecord * playerTail;
} TeamRecord;

//for maxheap
typedef struct {
    PlayerRecord *player;
    int teamIndex;
} HeapNode;

typedef struct{
    HeapNode node[10];
    int size;
} MaxHeap;

char *teamNames[] = {
    "India",
    "Australia",
    "Bangladesh",
    "England",
    "New Zealand",
    "Pakistan",
    "South Africa",
    "Sri Lanka",
    "Afghanistan",
    "West Indies"
    };


void initializeTeam(TeamRecord *team){
    for(int i=0;i<10;i++){
        (team + i)->id = i + 1;
        strcpy((team+i)->TeamName, teamNames[i]);
        (team + i)->AvgBattingStrikeRate = 0;
        (team + i)->totalPlayer = 0;
        (team + i)->playersHead = NULL;
        (team + i)->playerTail = NULL;

        for(int j=0;j<3;j++)
        (team+i)->roleHeads[j] = NULL;
    }
}

PlayerRecord * createPlayer(const Player *player){
    PlayerRecord *p = malloc(sizeof(*p));

    if(p == NULL)return NULL;
    p->id = player->id;
    strcpy(p->Name, player->name);
    strcpy(p->TeamName, player->team);
    if(!strcmp(player->role, "Bowler")){
        p->Role = 1;
        p->PerformanceIndex = (player->wickets * 2) + (100 - player->economyRate);
    }else if(!strcmp(player->role, "Batsman")){
        p->Role = 0;
        p->PerformanceIndex = (player->battingAverage * player->strikeRate)/100;
    }else {
        p->Role = 2;
        p->PerformanceIndex = ((player->battingAverage) * (player->strikeRate)/100) + (player->wickets * 2);
    }

    p->TotalRun = player->totalRuns;
    p->BattingAvg = player->battingAverage;
    p->StrikeRate = player->strikeRate;
    p->Wickets = player->wickets;
    p->EconomyRate = player->economyRate;

    p->nextPlayer = NULL;
    p->nextRole = NULL;

    return p;
}

TeamId getTeamID(const char *teamName) {
    if (strcmp(teamName, "India") == 0)
        return INDIA;
    else if (strcmp(teamName, "Australia") == 0)
        return AUSTRALIA;
    else if (strcmp(teamName, "Bangladesh") == 0)
        return BANGLADESH;
    else if (strcmp(teamName, "England") == 0)
        return ENGLAND;
    else if (strcmp(teamName, "New Zealand") == 0)
        return NEW_ZEALAND;
    else if (strcmp(teamName, "Pakistan") == 0)
        return PAKISTAN;
    else if (strcmp(teamName, "South Africa") == 0)
        return SOUTH_AFRICA;
    else if (strcmp(teamName, "Sri Lanka") == 0)
        return SRI_LANKA;
    else if (strcmp(teamName, "Afghanistan") == 0)
        return AFGHANISTAN;
    else if (strcmp(teamName, "West Indies") == 0)
        return WEST_INDIES;

    return 0; // Invalid team
}

int addPlayer(TeamRecord *teams, const Player *player){
    if(teams == NULL || player == NULL)return 0;

    PlayerRecord * p = createPlayer(player);
    if(p == NULL)return 0;

    int id = getTeamID(p->TeamName);
    int index = id - 1;
    if(id < 1 || id > 10){
        free(p);
        return 0;
    }

    //get address of correct team
    TeamRecord *t = teams + index;

    if(t->playersHead == NULL){
        t->playersHead = p;
        t->playerTail = p;
    }else{
        t->playerTail->nextPlayer = p;
        t->playerTail = p;
    }

    int role = p->Role;
    float PI = p->PerformanceIndex;

    //for maintaining LL of particular role
    PlayerRecord *curr = t->roleHeads[role];
    PlayerRecord *prev = NULL;
    while(curr != NULL && curr->PerformanceIndex >= PI){
        prev = curr;
        curr = curr->nextRole;
    }

    if(prev == NULL){ //insert at head
        prev = p;
        prev->nextRole = curr;
        t->roleHeads[role] = p;
    }else if(curr == NULL){ //insert at tail
        prev->nextRole = p;
        prev = p;
    }
    else{
        prev->nextRole = p;
        p->nextRole = curr;
    }


    t->totalPlayer += 1;

    //for avg batting stike rate
    float count = 0;
    int size = 0;
    PlayerRecord *temp1 = t->roleHeads[0];
    while(temp1){
        count += temp1->StrikeRate;
        size+=1;

        temp1 = temp1->nextRole;
    }

    PlayerRecord *temp2 = t->roleHeads[2];
    while(temp2){
        count += temp2->StrikeRate;
        size+=1;

        temp2 = temp2->nextRole;
    }

    if(size != 0)
    t->AvgBattingStrikeRate = count / size;
    else t->AvgBattingStrikeRate = 0;

    return 1;
}

//display all player
int displayTeam(const TeamRecord *teams, int teamId){
    if(teams == NULL)return 0;
    if(teamId < 1 || teamId > 10){
        printf("\nTeam Id must lie between 1 - 10");
        return 0;
    }

    const TeamRecord *t = teams + teamId - 1;

    const PlayerRecord *p = t->playersHead;

    while(p){
        printf("\nPlayer Id: %d",p->id);
        printf("\nPlayer Name: %s",p->Name);
        printf("\nPlayer Role: %s", p->Role == 0 ? "Batsman" : p->Role == 1 ? "Bowler" : "All Rounder");
        printf("\nTotal Runs: %d",p->TotalRun);
        printf("\nBatting Average: %f",p->BattingAvg);
        printf("\nStrike Rate: %f",p->StrikeRate);
        printf("\nWickets: %d",p->Wickets);
        printf("\nEconomy Rate: %f",p->EconomyRate);
        printf("\nPerformance Index: %f",p->PerformanceIndex);
        printf("\n=============\n");

        p = p->nextPlayer;
    }
    printf("\nTotal Player: %d",t->totalPlayer);
    printf("\nAverage Batting Strike Rate: %f", t->AvgBattingStrikeRate);

    return 1;
}

void displayTeamSorted(const TeamRecord *teams){
    const TeamRecord **t = malloc (10 * sizeof(*t));
    if(t == NULL)return;
    for(int i=0;i<10;i++){
        *(t + i) = (teams + i);
    }

    //sort in descending
    for(int i=0;i<9;i++){
        for(int j=0;j<9-i;j++){
            if((*(t + j))->AvgBattingStrikeRate < (*(t+j+1))->AvgBattingStrikeRate){
                const TeamRecord *temp = *(t+j);
                *(t+j) = *(t + j + 1);
                *(t + j + 1) = temp;
            }
        }
    }

    //display
    printf("\n======= TEAMS SORTED BY BATTING STRIKE RATE =======\n");

    for(int i = 0; i < 10; i++){
        printf("\nTeam ID: %d", (*(t + i))->id);
        printf("\nTeam Name: %s", (*(t + i))->TeamName);
        printf("\nTotal Players: %d", (*(t + i))->totalPlayer);
        printf("\nAverage Batting Strike Rate: %.2f",
               (*(t + i))->AvgBattingStrikeRate);
        printf("\n========================================\n");
    }
    free(t);
}

int displayTopKPlayers(const TeamRecord *teams, int teamId, Role role, int k){
    if(teams == NULL)return 0;
    if(teamId < 1 || teamId > 10){
        printf("\nTeam Id must lie between 1 - 10");
        return 0;
    }
    if(k < 1){
        printf("\nK must be positive");
        return 0;
    }
    if(role < 0 || role > 2){
        printf("\nRole must lie between 0 - 2");
        return 0;
    }

    const TeamRecord *t = teams + teamId - 1;

    const PlayerRecord *p = t->roleHeads[role];
    if(p == NULL){
        printf("\nNo player for this role");
        return 1;
    }
    while(p && k--){
        printf("\n%s - PI: %f",p->Name , p->PerformanceIndex);
        p=p->nextRole;
    }
    return 1;
}

//insert new element in Heap
void heapifyUp(MaxHeap *heap , int index){
    while(index > 0){
        int parent = (index - 1)/2; //formula to find parent node in binary tree
        if(heap->node[parent].player->PerformanceIndex >= heap->node[index].player->PerformanceIndex)break;

        HeapNode temp = heap->node[parent];
        heap->node[parent] = heap->node[index];
        heap->node[index]=temp;

        index = parent;
    }
}

//extract max element from heap
void heapifyDown(MaxHeap *heap, int index){

    while(1){

        int largest = index;
        int left = 2 * index + 1;
        int right = 2 * index + 2;

        if(left < heap->size &&
           heap->node[left].player->PerformanceIndex >
           heap->node[largest].player->PerformanceIndex){

            largest = left;
        }

        if(right < heap->size &&
           heap->node[right].player->PerformanceIndex >
           heap->node[largest].player->PerformanceIndex){

            largest = right;
        }

        if(largest == index)
            break;

        HeapNode temp = heap->node[index];
        heap->node[index] = heap->node[largest];
        heap->node[largest] = temp;

        index = largest;
    }
}

int insertHeap(MaxHeap *heap, PlayerRecord *player, int teamIndex){

    if(heap == NULL || player == NULL)return 0;

    if(heap->size >= 10)return 0;

    int index = heap->size;

    heap->node[index].player = player;
    heap->node[index].teamIndex = teamIndex;

    heap->size++;

    heapifyUp(heap, index);

    return 1;
}

int extractMax(MaxHeap *heap, HeapNode *result){

    if(heap == NULL || result == NULL || heap->size == 0)
        return 0;

    *result = heap->node[0];

    heap->node[0] = heap->node[heap->size - 1];

    heap->size--;

    if(heap->size > 0)
        heapifyDown(heap, 0);

    return 1;
}

int displayAllPlayersByRole(const TeamRecord *teams, Role role){

    if(teams == NULL)return 0;

    if(role < 0 || role > 2){
        printf("\nInvalid role!");
        return 0;
    }

    MaxHeap heap;
    heap.size = 0;

    // Insert first player of each team's selected role
    for(int i = 0; i < 10; i++){

        const PlayerRecord *p = teams[i].roleHeads[role];

        if(p != NULL){
            if(!insertHeap(&heap, (PlayerRecord *)p, i)){
                return 0;
            }
        }
    }

    if(heap.size == 0){
        printf("\nNo players found for this role.");
        return 1;
    }

    printf("\n======= PLAYERS SORTED BY PERFORMANCE INDEX =======\n");

    HeapNode extracted;

    while(extractMax(&heap, &extracted)){

        PlayerRecord *p = extracted.player;

        printf("\nPlayer Name: %s", p->Name);
        printf("\nTeam Name: %s", p->TeamName);
        printf("\nPerformance Index: %.2f", p->PerformanceIndex);
        printf("\n========================================\n");

        // Insert next player from the same team's role list
        if(p->nextRole != NULL){

            if(!insertHeap(&heap, p->nextRole, extracted.teamIndex)){
                return 0;
            }
        }
    }

    return 1;
}

int addPlayerFromInput(TeamRecord *teams){

    Player player;
    char name[50];
    int teamId;
    int role;
    int value;

    printf("\nEnter Player ID: ");
    value = getInt();

    if(value < 1){
        printf("\nInvalid Player ID!");
        return 0;
    }

    player.id = value;

    printf("Enter Player Name: ");

    if(fgets(name, sizeof(name), stdin) == NULL){
        printf("\nError Taking Input!");
        return 0;
    }

    name[strcspn(name, "\n")] = '\0';

    if(strlen(name) == 0){
        printf("\nPlayer name cannot be empty!");
        return 0;
    }

    player.name = name;

    printf("\nSelect Team:\n");

    for(int i = 0; i < 10; i++){
        printf("%d. %s\n", i + 1, teamNames[i]);
    }

    printf("Enter Team ID: ");
    teamId = getInt();

    if(teamId < 1 || teamId > 10){
        printf("\nInvalid Team ID!");
        return 0;
    }

    player.team = teamNames[teamId - 1];

    printf("\nSelect Role:\n");
    printf("1. Batsman\n");
    printf("2. Bowler\n");
    printf("3. All-rounder\n");

    printf("Enter Role: ");
    role = getInt();

    if(role < 1 || role > 3){
        printf("\nInvalid Role!");
        return 0;
    }

    if(role == 1)
        player.role = "Batsman";
    else if(role == 2)
        player.role = "Bowler";
    else
        player.role = "All-rounder";

    printf("Enter Total Runs: ");
    value = getInt();

    if(value < 0){
        printf("\nInvalid Total Runs!");
        return 0;
    }

    player.totalRuns = value;

    printf("Enter Batting Average: ");

    if(!getFloat(&player.battingAverage))
        return 0;

    printf("Enter Strike Rate: ");

    if(!getFloat(&player.strikeRate))
        return 0;

    printf("Enter Wickets: ");
    value = getInt();

    if(value < 0){
        printf("\nInvalid Wickets!");
        return 0;
    }

    player.wickets = value;

    printf("Enter Economy Rate: ");

    if(!getFloat(&player.economyRate))
        return 0;

    if(!addPlayer(teams, &player)){
        printf("\nFailed to add player!");
        return 0;
    }

    printf("\nPlayer added successfully!");

    return 1;
}

void freeAllPlayers(TeamRecord *teams){
    if(teams == NULL)return;

    for(int i=0;i<10;i++){
        PlayerRecord *curr = (teams + i)->playersHead;
        while(curr){
            PlayerRecord *temp = curr;
            curr = curr->nextPlayer;

            free(temp);
        }

        //reset team pointers
        (teams + i)->playersHead = NULL;
        (teams + i)->playerTail = NULL;

        //reset job specific LL
        for(int j=0;j<3;j++){
            (teams + i)->roleHeads[j] = NULL;
        }
        (teams + i)->totalPlayer = 0;
    }
}


int main(){

    TeamRecord Teams[10];

    // Initialize all teams
    initializeTeam(Teams);

    // Load players from the provided dataset
    for(int i = 0; i < playerCount; i++){
        if(!addPlayer(Teams, players + i)){
            printf("\nError adding player with ID: %d\n", (players + i)->id);
            freeAllPlayers(Teams);
            return 1;
        }
    }

    printf("\nAll players loaded successfully!\n");

    int choice;

    while(1){

        printf("\n========== ICC ODI PLAYER ANALYZER ==========\n");
        printf("1. Add Player\n");
        printf("2. Display Team Players\n");
        printf("3. Display Teams Sorted by Average Strike Rate\n");
        printf("4. Display Top K Players of a Team\n");
        printf("5. Display All Players of a Role\n");
        printf("6. Exit\n");
        printf("=============================================\n");

        printf("Enter your choice: ");

        choice = getInt();
        if(choice == -1)return 1;

        switch(choice){

            case 1:
                addPlayerFromInput(Teams);
                break;

            case 2: {
                
                printf("\nEnter Team ID (1-10): ");
                int teamId = getInt();
                if(teamId == -1)return 1;

                displayTeam(Teams, teamId);
                break;
            }

            case 3:
                displayTeamSorted(Teams);
                break;

            case 4: {
                int teamId, role, k;

                printf("\nEnter Team ID (1-10): ");
                teamId = getInt();
                if(teamId == -1)return 1;

                printf("\nSelect Role:\n");
                printf("0. Batsman\n");
                printf("1. Bowler\n");
                printf("2. All-rounder\n");

                printf("Enter role: ");
                role = getInt();
                if(role == -1)return 1;

                printf("\nEnter K: ");
                k = getInt();
                if(k == -1)return 1;

                displayTopKPlayers(Teams, teamId, (Role)role, k);
                break;
            }

            case 5: {
                
                printf("\nSelect Role:\n");
                printf("0. Batsman\n");
                printf("1. Bowler\n");
                printf("2. All-rounder\n");
                printf("Enter role: ");
                
                int role = getInt();
                if(role == -1)return 1;

                displayAllPlayersByRole(Teams, (Role)role);
                break;
            }

            case 6:
                printf("\nExiting program...\n");
                freeAllPlayers(Teams);
                return 0;

            default:
                printf("\nInvalid choice! Try again.\n");
        }
    }

    return 0;
}