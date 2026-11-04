#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include<string.h>

struct Block{
    size_t start; //starting offset
    size_t size;
    int allocationId;
    int isAllocated; //1 = allocated , 0 = free
};

struct MemoryManager{
    unsigned char *memoryPool; //actual memory buffer
    struct Block * blocks;
    size_t poolSize;
    size_t blockCount;
    size_t capacity;
    int nextAllocationId;
    int allocationStrategy;
};

int getInt(){
    char input[1000];
    char extra;
    int n;

    while(1){
    if(fgets(input, sizeof(input), stdin) == NULL){
        printf("\nError Taking input!");
        continue;
    }

    if(sscanf(input, "%d %c", &n, &extra) != 1){
        printf("\nEnter only Integer");
        continue;
    }

    break;
}

    return n;
}

int initializePool(struct MemoryManager *m){
    printf("\nEnter Pool Size: ");
     m->poolSize = getInt();

    while(m->poolSize < 1 || m->poolSize > 1000000){
        printf("\nPoolsize must be of range 1 - 1000000");
        printf("\nEnter Pool Size: ");
        m->poolSize = getInt();
    }

    m->memoryPool = malloc(m->poolSize * sizeof(unsigned char));
    if(m->memoryPool == NULL){
        printf("\nMemory Allocation failed!");
        return 0;
    }

    m->blocks = malloc(sizeof(struct Block));
    if(m->blocks == NULL){
        printf("\nMemory Allocation failed!");
        free(m->memoryPool);
        return 0;
    }

    m->blocks->start = 0;
    m->blocks->size = m->poolSize;
    m->blocks->allocationId = 0;
    m->blocks->isAllocated = 0;

    m->blockCount = 1;
    m->capacity = 1;
    m->nextAllocationId = 1;
    m->allocationStrategy = 2; //best fit

    return 1;
}

//increase memory whenever needed
int ensureCapacity(struct MemoryManager *m, size_t requiredCapacity){
     if (requiredCapacity <= m->capacity) {
        return 1;
    }

      size_t newCapacity = m->capacity * 2;

    while (newCapacity < requiredCapacity) {
        newCapacity *= 2;
    }

        struct Block * t = realloc(m->blocks, newCapacity * sizeof(*t));
        if(t == NULL){
            printf("\nError allocation memory!");
            return 0;
        }
        m->capacity =newCapacity;
        m->blocks = t;

    return 1;
}

int firstFit(struct MemoryManager *m , size_t size){
    for(int i=0;i<m->blockCount;i++){
        if(!(m->blocks+i)->isAllocated){
            if((m->blocks+i)->size >= size){
                return i;
            }
        }
    }
    return -1;
}

int bestFit(struct MemoryManager *m, size_t size){
    int minIdx = -1;
    size_t allocatedSize = SIZE_MAX;
    for(int i=0;i<m->blockCount;i++){
        if(!(m->blocks+i)->isAllocated){
            if((m->blocks+i)->size >= size){
                if(allocatedSize > (m->blocks + i)->size){
                    allocatedSize = (m->blocks + i)->size;
                    minIdx = i;
                }
            }
        }
    }

    return minIdx;
}

int worstFit(struct MemoryManager *m, size_t size){
    int maxIdx = -1;
    size_t allocatedSize = 0;

    for(int i=0;i<m->blockCount;i++){
        if(!(m->blocks+i)->isAllocated){
            if((m->blocks + i)->size >= size){
                if((m->blocks + i)->size > allocatedSize){
                    allocatedSize = (m->blocks + i)->size;
                    maxIdx = i;
                }
            }
        }
    }

    return maxIdx;
}

int findSuitableBlock(struct MemoryManager *m, size_t size, int allocationStrategy){
    int idx;
    if(allocationStrategy == 1){
        idx = firstFit(m,size);
    }else if(allocationStrategy == 2){
        idx = bestFit(m,size);
    }else idx = worstFit(m,size);

    return idx;
}

int allocateMemory(struct MemoryManager *m, size_t size, int allocationStrategy){
    //validate requested size
    if(size == 0 || size > m->poolSize){
        printf("\nRequested Size is larger than available size!");
        return 0;
    }

    int idx = findSuitableBlock(m,size,allocationStrategy);
    if(idx == -1){
        printf("\nNo suitable block found!");
        return 0;
    }

    if((m->blocks + idx)->size == size){
        (m->blocks + idx)->isAllocated = 1;
        (m->blocks + idx)->allocationId = m->nextAllocationId;

        m->nextAllocationId++;
    }else if((m->blocks + idx)->size > size){
        //split the memory

        //ensure capacity for 1 additional metadata block
        if(!ensureCapacity(m,m->blockCount + 1)){
            return 0;
        }

        size_t oldsize = (m->blocks + idx)->size;
        size_t oldstart = (m->blocks + idx)->start;

        //shift blocks to the right
        for(size_t i= m->blockCount; i> (size_t)idx+1; i--){
            *(m->blocks + i) = *(m->blocks + i- 1);
        }

        //update the selected block
        (m->blocks + idx)->size = size;
        (m->blocks + idx)->isAllocated = 1;
        (m->blocks + idx)->allocationId = m->nextAllocationId;

        //create the remaining free block
        (m->blocks + idx + 1)->start = oldstart + size;
        (m->blocks + idx + 1)->size = oldsize - size;
        (m->blocks + idx + 1)->allocationId = 0;
        (m->blocks + idx + 1)->isAllocated = 0;

        m->blockCount++;
        m->nextAllocationId++;

    }

    return 1;
}

void mergeFreeBlock(struct MemoryManager *m){
    size_t i = 1;

    while(i< m->blockCount){
        if(!(m->blocks + i - 1)->isAllocated && !(m->blocks + i)->isAllocated){
            (m->blocks + i - 1)->size += (m->blocks + i)->size;

            //shift left
            for(size_t j = i+1;j<m->blockCount;j++){
                *(m->blocks + j - 1) = *(m->blocks + j);
            }
            m->blockCount--;
        }else i++;
    }
}

int releaseMemory(struct MemoryManager *m, size_t allocationId){
    //reject ids less than or equal to 0
    if(allocationId <=0){
        printf("\nId must be greater than 0");
        return 0;
    }

    //find allocationId
    int found = 0;
    for(int i=0;i<m->blockCount;i++){
        if((m->blocks + i)->allocationId == allocationId && (m->blocks + i)->isAllocated){
            (m->blocks + i)->isAllocated = 0;
            (m->blocks + i)->allocationId = 0;
            found = 1;
            break;
        }
    }

    if(!found){
        printf("\nMemory with allocation id %zu not found!", allocationId);
        return 0;
    }

    //merge adjacent free blocks
    mergeFreeBlock(m);

    return 1;
}

int resizeMemory(struct MemoryManager *m, int allocationId, size_t newSize){
    if(newSize == 0 || newSize > m->poolSize){
        printf("\nSize is out of range");
        return 0;
    }

    //search for allocationId
    int found = 0, i = 0;

    for(i;i<m->blockCount;i++){
        if((m->blocks + i)->allocationId == allocationId && (m->blocks + i)->isAllocated){
            found = 1;
            break;
        }
    }

    if(!found){
        printf("\nAllocation Id not Found!");
        return 0;
    }

    if((m->blocks + i)->size > newSize){
        //required is less than => shrink

        size_t originalSize = (m->blocks + i)->size;
        size_t originalStart = (m->blocks + i)->start;

        if(!ensureCapacity(m,m->blockCount + 1)){
            return 0;
        }

        //shift the subsequent blocks to right
        for(int j=m->blockCount; j>i;j--){
            *(m->blocks+j) = *(m->blocks+j-1);
        }

        //update the block
        (m->blocks + i)->size = newSize;

        //create new free block at i+1
        (m->blocks + i+1)->start = originalStart + newSize;
        (m->blocks + i+1)->size = originalSize - newSize;
        (m->blocks + i+1)->allocationId = 0;
        (m->blocks + i+1)->isAllocated = 0;

        m->blockCount++;

        mergeFreeBlock(m);

    }else if((m->blocks + i)->size < newSize){
        //current allocated block is smaller than the requested size => expansion

        size_t requiredSize = newSize - (m->blocks+i)->size;

        //check if next block is free and size is exactly as required
        if(i+1 < m->blockCount && !(m->blocks + i+ 1)->isAllocated && (m->blocks + i + 1)->size == requiredSize){
            //expand the current allocation
            (m->blocks+i)->size = newSize;

            //shift left to remove 
            for(size_t j = i+2;j<m->blockCount;j++){
                *(m->blocks+j-1) = *(m->blocks+ j);
            }

            m->blockCount--;
        }else if(i+1 < m->blockCount && !(m->blocks + i + 1)->isAllocated && (m->blocks + i + 1)->size > requiredSize){
            //check if next block has size greater than requiredSize
            (m->blocks + i)->size = newSize;
            (m->blocks + i+1)->start += requiredSize;
            (m->blocks + i + 1)->size -= requiredSize;
        }else{
            //not enough memory here => relocation needed
            
            //store original allocation detail
            size_t oldstart = (m->blocks + i)->start;
            size_t oldsize = (m->blocks + i)->size;
            int oldId = (m->blocks + i)->allocationId;

            //find another suitable block
            int idx = findSuitableBlock(m,newSize,m->allocationStrategy);

            if(idx == -1){
                printf("\nNo Suitable block exits for relocation");
                return 0;
            }

            //store destination block details
            size_t newstart = (m->blocks + idx)->start;
            size_t availableSize = (m->blocks + idx)->size;

            //ensure metadata capacity if splitting required
            if(availableSize > newSize){
                if(!ensureCapacity(m,m->blockCount + 1))
                    return 0;
            }

            //allocate destination block
            if(availableSize == newSize){
                (m->blocks + idx)->isAllocated = 1;
                (m->blocks + idx)->allocationId = oldId;
            }else{
                //shift metadata for new entry
                for(size_t j=m->blockCount; j>(size_t)idx + 1; j--){
                    *(m->blocks + j) = *(m->blocks + j -1);
                }

                (m->blocks + idx)->size = newSize;
                (m->blocks + idx)->isAllocated = 1;
                (m->blocks + idx)->allocationId = oldId;

                //create remaining free block
                (m->blocks + idx + 1)->start = newstart + newSize;
                (m->blocks + idx + 1)->size = availableSize - newSize;
                (m->blocks + idx + 1)->isAllocated = 0;
                (m->blocks + idx + 1)->allocationId = 0;

                m->blockCount++;
            }

            //copy old allocation data to new location
            memcpy(m->memoryPool + newstart, m->memoryPool + oldstart, oldsize);

            //release old allocation
            for(size_t j=0;j<m->blockCount;j++){
                if((m->blocks + j)-> start == oldstart && (m->blocks + j)->allocationId == oldId){
                    (m->blocks + j)->isAllocated = 0;
                    (m->blocks + j)->allocationId = 0;

                    break;
                }
            }

            mergeFreeBlock(m);

        }
    }else{
        //required memory = current allocated memory

        printf("\nsize is already equal to requested size");
        return 1;
    }

    return 1;
}

void displayMemoryMap(struct MemoryManager *m){
    for(size_t i=0;i<m->blockCount;i++){
        printf("\nBlock Number: %d \nStarting Address: %zu \nEnding Address: %d \nBlock Size: %d \nStatus: %s \nAllocation ID: %d", i+1, (m->blocks + i)->start, (m->blocks + i)->start + (m->blocks + i)->size - 1, (m->blocks + i)->size, (m->blocks + i)->isAllocated == 1 ? "Allocated" : "Free", (m->blocks + i)->allocationId);
        printf("\n------------------");
    }
}

void displayMemoryStatistics(struct MemoryManager *m){
    size_t totalMemory = 0, allocatedMemory = 0, freeMemory = 0;
    int n_allocated = 0, n_free = 0;

    for(size_t i =0;i<m->blockCount;i++){
        totalMemory += (m->blocks + i)->size;
        if((m->blocks + i)->isAllocated){
            allocatedMemory += (m->blocks + i)->size;
            n_allocated++;
        }else{
            freeMemory += (m->blocks + i)->size;
            n_free++;
        }
    }

    printf("\nTotal Pool Size: %d", totalMemory);
    printf("\nAllocated Memory: %d", allocatedMemory);
    printf("\nFree Memory: %d", freeMemory);
    printf("\nNumber of Allocated Blocks: %d",n_allocated);
    printf("\nNumber of Free Blocks: %d", n_free);
    printf("\nMemory Utilization: %0.2lf", ((double)allocatedMemory / totalMemory) * 100.0);
    printf("\nFree Memory Percentage: %0.2lf", ((double)freeMemory / totalMemory) * 100.0);
}

void detectExternalFragmentation(struct MemoryManager *m, size_t requestedSize){

    size_t totalFreeMemory = 0;
    size_t largestFreeBlock = 0;

    // Calculate total free memory and largest free block
    for(size_t i = 0; i < m->blockCount; i++){

        if(!(m->blocks + i)->isAllocated){

            totalFreeMemory += (m->blocks + i)->size;

            if((m->blocks + i)->size > largestFreeBlock){
                largestFreeBlock = (m->blocks + i)->size;
            }
        }
    }

    printf("\nTotal Free Memory: %zu bytes", totalFreeMemory);
    printf("\nLargest Free Block: %zu bytes", largestFreeBlock);

    if(requestedSize == 0 || requestedSize > m->poolSize){
        printf("\nInvalid requested size!");
    }
    else if(totalFreeMemory >= requestedSize && largestFreeBlock < requestedSize){
        printf("\nExternal Fragmentation Detected!");
        printf("\nTotal free memory is sufficient, but no contiguous block is large enough.");
    }
    else if(largestFreeBlock >= requestedSize){
        printf("\nNo external fragmentation preventing this allocation.");
    }
    else{
        printf("\nInsufficient total free memory!");
    }
}

void changeAllocationStrategy(struct MemoryManager *m){
    printf("\n1 -> First Fit");
    printf("\n2 -> Best Fit");
    printf("\n3 -> Worst Fit");

    while(1){
    printf("\nEnter your choice: ");
    int n = getInt();
    switch (n)
    {
    case 1:
        m->allocationStrategy = 1;
        break;
    case 2: 
        m->allocationStrategy = 2;
        break;
    case 3: m->allocationStrategy = 3;
    break;
    default: printf("\nEnter choice between 1 - 3");
        continue;;
    }
    break;
}

    switch(m->allocationStrategy){
        case 1: printf("First Fit"); break;
        case 2: printf("Best Fit"); break;
        case 3: printf("Worst Fit"); break;
    }
}

int main(){
    struct MemoryManager *m = malloc(sizeof(*m));

    if(m == NULL){
        printf("\nMemory Allocation failed!");
        return 1;
    }

    int a = initializePool(m);
    if(!a){
        free(m);
        return 1;
    }

    int choice;

     while(1){
    printf("\n\n========== SMART MEMORY MANAGER ==========");
    printf("\n1. Allocate Memory");
    printf("\n2. Release Memory");
    printf("\n3. Resize Memory");
    printf("\n4. Display Memory Map");
    printf("\n5. Display Memory Statistics");
    printf("\n6. Detect External Fragmentation");
    printf("\n7. Change Allocation Strategy");
    printf("\n8. Exit");
    printf("\nEnter your choice: ");
        
        choice = getInt();

        switch(choice){

            case 1:{
                size_t size;

                printf("\nEnter allocation size: ");
                size = getInt();

                if(allocateMemory(m, size, m->allocationStrategy)){
                    printf("\nMemory allocated successfully!");
                }
                break;
            }

            case 2:{
                size_t allocationId;

                printf("\nEnter Allocation ID: ");
                allocationId = getInt();

                if(releaseMemory(m, allocationId)){
                    printf("\nMemory released successfully!");
                }
                break;
            }

            case 3:{
                int allocationId;
                size_t newSize;

                printf("\nEnter Allocation ID: ");
                allocationId = getInt();

                printf("\nEnter New Size: ");
                newSize = getInt();

                if(resizeMemory(m, allocationId, newSize)){
                    printf("\nMemory resized successfully!");
                }
                break;
            }

            case 4:
                displayMemoryMap(m);
                break;

            case 5:
                displayMemoryStatistics(m);
                break;

            case 6:{
                size_t requestedSize;

                printf("\nEnter requested size: ");
                requestedSize = getInt();

                detectExternalFragmentation(m, requestedSize);
                break;
            }

            case 7:
                changeAllocationStrategy(m);
                break;

            case 8:
                printf("\nExiting Memory Manager...");
                free(m->memoryPool);
                free(m->blocks);
                free(m);
                return 0;

            default:
                printf("\nInvalid choice! Enter a number between 1 and 8.");
        }
    }

    return 0;
}