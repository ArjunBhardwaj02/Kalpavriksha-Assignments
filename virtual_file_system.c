#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define NUM_BLOCKS 10
#define BLOCK_SIZE 512
#define MAX_NAME 50

typedef enum {
    FILE_TYPE, 
    DIRECTORY_TYPE
} NodeType;

typedef struct FreeBlock{
    int index;
    struct FreeBlock *next; //for next free block
    struct FreeBlock *prev; //for previous free block
} FreeBlock;

typedef struct FileNode{  
    char name[MAX_NAME + 1];
    NodeType type;

    struct FileNode *parent;
    struct FileNode *next;
    struct FileNode *prev;

    struct FileNode *children;  //point to first children in directory

    size_t fileSize;
    int blockCount;
    int *blockPointers;
} FileNode;

typedef struct FileSystem{
    unsigned char (*virtualDisk)[BLOCK_SIZE];   //store actual content

    int totalBlocks;
    int usedBlocks;

    FreeBlock *freeHead;
    FreeBlock *freeTail;

    FileNode *root; //point to root directory
    FileNode *cwd; // point to current directory
} FileSystem;

FileNode *createNode(const char *name , NodeType type, FileNode *parent){
    FileNode *f = malloc(sizeof(*f));
    if(f == NULL)return NULL;
    strcpy(f->name, name);
    f->type = type;
    f->parent = parent;

    f->next = NULL;
    f->prev = NULL;
    f->children = NULL;
    f->fileSize = 0;
    f->blockCount = 0;
    f->blockPointers = NULL;

    return f;
}

FreeBlock * createFreeBlock(int index){
    FreeBlock *f = malloc(sizeof(*f));
    if(f == NULL)return NULL;

    f->index = index;
    f->next = NULL;
    f->prev = NULL;

    return f;
}

int initializeFreeList(FileSystem *fs){
    int totalBlocks = fs->totalBlocks;
    fs->freeHead = NULL;
    fs->freeTail = NULL;

    for(int i=0;i<totalBlocks;i++){
        FreeBlock *node = createFreeBlock(i);
        if(node == NULL)return 0;
        if(fs->freeHead == NULL){
            fs->freeHead = node;
            fs->freeTail = node;
        }else{
            fs->freeTail->next = node;
            node->prev = fs->freeTail;
            fs->freeTail = node;
        }
    }
    return 1;
}

int initializeFileSystem(FileSystem * fs){
    fs->virtualDisk = NULL;
    fs->totalBlocks = NUM_BLOCKS;
    fs->usedBlocks = 0;
    fs->freeHead = NULL;
    fs->freeTail = NULL;
    fs->root = NULL;
    fs->cwd = NULL;

    fs->virtualDisk = calloc(fs->totalBlocks, sizeof(*fs->virtualDisk));
    if(fs->virtualDisk == NULL)return 0;

    //create root directory
    fs->root = createNode("/",DIRECTORY_TYPE,NULL);
    if(fs->root == NULL){
        free(fs->virtualDisk);
        fs->virtualDisk = NULL;
        return 0;
    }

    fs->root->next = fs->root;
    fs->root->prev = fs->root;

    fs->cwd = fs->root;

    //initialize free block list
    if(!initializeFreeList(fs)){
        free(fs->virtualDisk);
        free(fs->root);
        fs->virtualDisk = NULL;
        fs->root = NULL;
        fs->cwd = NULL;
        return 0;
    }

    return 1;
}

//remove first node from list and return idx
int allocateFreeBlock(FileSystem *fs){
    if(fs->freeHead == NULL)return -1;
    
    FreeBlock *temp = fs->freeHead;
    int idx = temp->index;

    fs->freeHead = fs->freeHead->next;

    if(fs->freeHead == NULL){   //if list is empty
        fs->freeTail = NULL;
    }else fs->freeHead->prev = NULL;

    free(temp);
    fs->usedBlocks++;       //freeBlocks denote the blocks available in the memory

    return idx;
}

//free the block and add to the tail
int releaseFreeBlock(FileSystem *fs, int index){
    FreeBlock * f = createFreeBlock(index);
    if(f == NULL)return 0;

    if(fs->freeHead == NULL){
        fs->freeHead = f;
        fs->freeTail = f;
    }else{
        f->prev = fs->freeTail;
        fs->freeTail->next = f;
        fs->freeTail = f;
    }
    fs->usedBlocks--;
    return 1;
}

//directory operation - add directory
int addChild(FileNode *parent, FileNode *child){
    if(parent == NULL || child == NULL)return 0;

    if(parent->children == NULL){
        //first child
        parent->children = child;

        //for making it circular
        child->next = child;
        child->prev = child;
    }else{
        //insert at end
        FileNode *last = parent->children->prev;

        child->next = parent->children;
        child->prev = last;

        last->next = child;
        parent->children->prev = child;
    }
    child->parent = parent;

    return 1;
}

//search directory or file for a file or subdirectory by name
FileNode *findChild(FileNode *parent, const char *name){
    if(parent == NULL)return NULL;
    if(parent->children == NULL)return NULL;

    FileNode * temp = parent->children;
    do{
        if(!strcmp(temp->name, name))return temp;
        temp = temp->next;
    }while(temp != parent->children);
    
    return NULL;
}

//creating new directory
int mkdir(FileSystem *fs, const char *name){
    if(fs == NULL || name == NULL)return 0;
    
    //search if the directory already exist
    if(findChild(fs->cwd, name) != NULL){
        printf("\nDirectory already exists!");
        return 0;
    }

    FileNode *temp = createNode(name,DIRECTORY_TYPE,fs->cwd);
    if(temp == NULL)return 0;

    if(!addChild(fs->cwd, temp)){
        free(temp);
        return 0;
    }

    return 1;
}

//change directory
int cd(FileSystem *fs, const char *name){
    if(fs == NULL || name == NULL || fs->cwd == NULL)return 0;

    //move to root directory
    if(!strcmp(name, "/")){
        fs->cwd = fs->root;
        return 1;
    }
    
    if(!strcmp("..",name)){
        //move to parent
        if(fs->cwd->parent != NULL)
            fs->cwd = fs->cwd->parent;

        return 1;
    }

    FileNode *temp = findChild(fs->cwd,name);

    if(!temp){
        printf("\nDirectory not found!");
        return 0;
    }

    if(temp->type != DIRECTORY_TYPE){
        printf("\nCannot Enter a file");
        return 0;
    }

    fs->cwd = temp;

    return 1;
}

//print working directory

//using recursion to print path
void printPath(FileNode *node){
    if(node == NULL)return;

    printPath(node->parent);
    if(node->parent == NULL){
        printf("/");
    }
    else printf("%s/",node->name);
}

void pwd(FileSystem *fs){
    if(fs == NULL || fs->cwd == NULL)return;

    printPath(fs->cwd);
    printf("\n");
}

//list directories
void ls(FileSystem *fs){
    if(fs == NULL || fs->cwd == NULL)return;

    FileNode *firstChild = fs->cwd->children;

    if(firstChild == NULL){
        printf("\nDirectory is empty");
        return;
    }

    do{
        if(firstChild->type == DIRECTORY_TYPE){
            printf("%s/",firstChild->name);
            printf("\n");
        }else printf("%s\n", firstChild->name);
        
        firstChild = firstChild->next;
    }while(firstChild != fs->cwd->children);
}

//Disk Free - display disk usage
void df(FileSystem *fs){
    if(fs == NULL || fs->cwd == NULL)return;

    printf("\nTotal Blocks: %d",fs->totalBlocks);
    printf("\nUsed Blocks: %d",fs->usedBlocks);
    printf("\nFree Blocks: %d", fs->totalBlocks - fs->usedBlocks);
    printf("\nBlock Size: %d bytes",BLOCK_SIZE);
    printf("\nTotal Capacity: %d bytes",fs->totalBlocks * BLOCK_SIZE);
    printf("\nFree Capacity: %d bytes", BLOCK_SIZE*(fs->totalBlocks - fs->usedBlocks));
}

//Remove Directory
int rmdir(FileSystem *fs, const char *name){
    if(fs == NULL || fs->cwd == NULL || name == NULL)return 0;
    if(!strcmp(name, "/")){
        printf("\nRoot can't be removed!");
        return 0;
    }

    FileNode *target = findChild(fs->cwd , name);
    if(target == NULL){
        printf("\nDirectory not found!");
        return 0;
    }

    if(target->type != DIRECTORY_TYPE){
        printf("\nOnly Directories can be removed");
        return 0;
    }

    if(target->children != NULL){
        printf("\nDirectory is not empty");
        return 0;
    }

    if(target->next == target){
        //only 1 child
        fs->cwd->children = NULL;
    }else{
        //multiple children
        target->prev->next = target->next;
        target->next->prev = target->prev;

        if(fs->cwd->children == target){
            fs->cwd->children = target->next;
        }
    }

    free(target);
    return 1;
}

//creating a file
int create(FileSystem *fs, const char *name){
    if(fs == NULL || name == NULL)return 0;
    if(findChild(fs->cwd, name)){
        printf("\nFile Already exists!");
        return 0;
    }

    FileNode *temp = createNode(name, FILE_TYPE, fs->cwd);
    if(temp == NULL)return 0;

    if(!addChild(fs->cwd, temp)){
        free(temp);
        return 0;
    }

    return 1;
}

int delete(FileSystem *fs, const char *name){
    if(fs == NULL || fs->cwd == NULL || name == NULL)return 0;

    FileNode *target = findChild(fs->cwd, name);
    if(target == NULL)return 0;

    if(target->type != FILE_TYPE){
        printf("\nOnly Files can be removed");
        return 0;
    }

    //release all allocated blocks
    for(int i=0;i<target->blockCount;i++){
        if(!releaseFreeBlock(fs, target->blockPointers[i])){
            printf("\nError releasing block!");
            return 0;
        }
    }

    free(target->blockPointers);

    if(target->next == target){
        fs->cwd->children = NULL;
    }else{
        target->prev->next = target->next;
        target->next->prev = target->prev;
        if(fs->cwd->children == target) fs->cwd->children = target->next;
    }

    free(target);
    return 1;
}

//write in file
int write(FileSystem *fs, const char *name, const char *data){
    if(fs == NULL || name == NULL || data == NULL)return 0;

    FileNode *target = findChild(fs->cwd, name);
    if(target == NULL)return 0;

    if(target->type != FILE_TYPE){
        printf("\nOnly filetype allowed");
        return 0;
    }

    //calculate required blocks
    size_t filesize = strlen(data);
    int requiredBlocks = (filesize + BLOCK_SIZE - 1)/ BLOCK_SIZE;

    //check available free blocks
    if(requiredBlocks > fs->totalBlocks - fs->usedBlocks){
        printf("\nNot enough Blocks!");
        return 0;
    }

    int *blocks = NULL;

    if(requiredBlocks > 0){
        blocks = malloc(requiredBlocks * sizeof(*blocks));
        if(blocks == NULL)return 0;
    }

    for(int i=0;i<requiredBlocks;i++){
        int idx = allocateFreeBlock(fs);

        if(idx == -1){
            //release previously allocated
            for(int j =0;j<i;j++){
                releaseFreeBlock(fs, blocks[j]);
            }
            free(blocks);
            return 0;
        }
        blocks[i] = idx;
    }

    //copy data to virtual disk
    size_t offset = 0;

    for (int i = 0; i < requiredBlocks; i++) {
        size_t bytesToCopy = filesize - offset;

        if (bytesToCopy > BLOCK_SIZE)
            bytesToCopy = BLOCK_SIZE;

        memcpy(fs->virtualDisk[blocks[i]],
               data + offset,
               bytesToCopy);

        offset += bytesToCopy;
    }

    // Release old blocks after successful new allocation
    for (int i = 0; i < target->blockCount; i++) {
        releaseFreeBlock(fs, target->blockPointers[i]);
    }

    free(target->blockPointers);

    // Update file metadata
    target->blockPointers = blocks;
    target->blockCount = requiredBlocks;
    target->fileSize = filesize;

    return 1;
}

//read file
// Read file contents
int read(FileSystem *fs, const char *name) {
    if (fs == NULL || fs->cwd == NULL || name == NULL)
        return 0;

    FileNode *target = findChild(fs->cwd, name);

    if (target == NULL) {
        printf("\nFile not found!");
        return 0;
    }

    if (target->type != FILE_TYPE) {
        printf("\nCannot read a directory!");
        return 0;
    }

    if (target->fileSize == 0) {
        printf("\nFile is empty!");
        return 1;
    }

    size_t remaining = target->fileSize;

    printf("\nFile Contents:\n");

    // Read data from allocated blocks
    for (int i = 0; i < target->blockCount; i++) {
        size_t remainBytes = remaining;

        if (remainBytes > BLOCK_SIZE)
            remainBytes = BLOCK_SIZE;

        fwrite(fs->virtualDisk[target->blockPointers[i]],
               1,
               remainBytes,
               stdout);

        remaining -= remainBytes;
    }

    printf("\n");

    return 1;
}

void freeDirectory(FileNode *node) {
    if (node == NULL)
        return;

    // Free all children
    if (node->children != NULL) {
        FileNode *first = node->children;
        FileNode *curr = first;

        do {
            FileNode *next = curr->next;

            // Break circular links
            curr->next = NULL;
            curr->prev = NULL;

            freeDirectory(curr);

            curr = next;

        } while (curr != first);
    }

    // Free file block pointers
    free(node->blockPointers);

    // Free current node
    free(node);
}

void freeFreeList(FileSystem *fs) {
    if (fs == NULL)
        return;

    FreeBlock *curr = fs->freeHead;

    while (curr != NULL) {
        FreeBlock *next = curr->next;

        free(curr);
        curr = next;
    }

    fs->freeHead = NULL;
    fs->freeTail = NULL;
}

//clean memory when leaving
void cleanupFileSystem(FileSystem *fs) {
    if (fs == NULL)
        return;

    // Free all files and directories
    freeDirectory(fs->root);

    // Free free-block linked list
    freeFreeList(fs);

    // Free virtual disk
    free(fs->virtualDisk);

    // Reset filesystem
    fs->virtualDisk = NULL;
    fs->root = NULL;
    fs->cwd = NULL;

    fs->freeHead = NULL;
    fs->freeTail = NULL;

    fs->totalBlocks = 0;
    fs->usedBlocks = 0;
}

int main() {
    FileSystem fs;

    // Initialize filesystem
    if (!initializeFileSystem(&fs)) {
        printf("\nFailed to initialize filesystem!");
        return 1;
    }

    int choice;
    char name[MAX_NAME + 1];
    char data[10000];

    while (1) {
        printf("\n\n========== VIRTUAL FILE SYSTEM ==========");
        printf("\n1.  Create Directory");
        printf("\n2.  Create File");
        printf("\n3.  Write File");
        printf("\n4.  Read File");
        printf("\n5.  Delete File");
        printf("\n6.  Remove Directory");
        printf("\n7.  List Directory");
        printf("\n8.  Change Directory");
        printf("\n9.  Print Working Directory");
        printf("\n10. Disk Free");
        printf("\n0.  Exit");
        printf("\n=========================================");
        printf("\nEnter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("\nInvalid input!");

            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF);

            continue;
        }

        // Clear remaining newline
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF);

        switch (choice) {

            case 1:
                printf("\nEnter directory name: ");

                if (fgets(name, sizeof(name), stdin) == NULL)
                    break;

                name[strcspn(name, "\n")] = '\0';

                if (mkdir(&fs, name))
                    printf("\nDirectory created successfully!");
                else
                    printf("\nDirectory creation failed!");

                break;

            case 2:
                printf("\nEnter file name: ");

                if (fgets(name, sizeof(name), stdin) == NULL)
                    break;

                name[strcspn(name, "\n")] = '\0';

                if (create(&fs, name))
                    printf("\nFile created successfully!");
                else
                    printf("\nFile creation failed!");

                break;

            case 3:
                printf("\nEnter file name: ");

                if (fgets(name, sizeof(name), stdin) == NULL)
                    break;

                name[strcspn(name, "\n")] = '\0';

                printf("\nEnter file content: ");

                if (fgets(data, sizeof(data), stdin) == NULL)
                    break;

                data[strcspn(data, "\n")] = '\0';

                if (write(&fs, name, data))
                    printf("\nFile written successfully!");
                else
                    printf("\nFile writing failed!");

                break;

            case 4:
                printf("\nEnter file name: ");

                if (fgets(name, sizeof(name), stdin) == NULL)
                    break;

                name[strcspn(name, "\n")] = '\0';

                read(&fs, name);
                break;

            case 5:
                printf("\nEnter file name: ");

                if (fgets(name, sizeof(name), stdin) == NULL)
                    break;

                name[strcspn(name, "\n")] = '\0';

                if (delete(&fs, name))
                    printf("\nFile deleted successfully!");
                else
                    printf("\nFile deletion failed!");

                break;

            case 6:
                printf("\nEnter directory name: ");

                if (fgets(name, sizeof(name), stdin) == NULL)
                    break;

                name[strcspn(name, "\n")] = '\0';

                if (rmdir(&fs, name))
                    printf("\nDirectory removed successfully!");
                else
                    printf("\nDirectory removal failed!");

                break;

            case 7:
                ls(&fs);
                break;

            case 8:
                printf("\nEnter directory name: ");

                if (fgets(name, sizeof(name), stdin) == NULL)
                    break;

                name[strcspn(name, "\n")] = '\0';

                if (cd(&fs, name))
                    printf("\nDirectory changed successfully!");

                break;

            case 9:
                pwd(&fs);
                break;

            case 10:
                df(&fs);
                break;

            case 0:
                cleanupFileSystem(&fs);
                printf("\nFilesystem closed successfully!");
                return 0;

            default:
                printf("\nInvalid choice!");
        }
    }

    cleanupFileSystem(&fs);
    return 0;
}