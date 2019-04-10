/*
 * Translation Look-aside Buffer Simulator.
 *      created by Oleg and Nikita.
 */

/* 
 * Double-linked list with hash was NOT created by oleg or nikita.
 * We found the DoubleLinkedHashMap on: https://www.geeksforgeeks.org/lru-cache-implementation/
 * Use of outside code is approved by Ioanis Nikolaidis as confirmed by email
 * We used the code for the data structure and the LRU. (the latter of which we edited for our purposes)
 */

/*
 * Our Translation Look-aside Buffer Simulator can be run with the arguments: 
 * valgrind --tool=lackey --trace-mem=yes ./inputProgram 2>&1 | ./valtlb379 -i -f (flush period) -p (Policy) pagesize tablesize
 * INSTRUCTIONS: -i flag removes all instructions from feed; -f flush flushes the data structure every n references; policy uses either a FIFO or LRU policy on table;
 * pagesize is a power of 2 (from 16 to 65,536); tablesize is number of page table entries in (always a power of 2 and no more than 2,048)
 * 
 * We implemented a Double linked list as a queue for keeping order of our page numbers and a Hash for quick lookup.
 * Our code allows the data structure to function in a FIFO or least recently used manner.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <math.h>

//define variables
const char* policy;
int flushPeriod;
int pgsize;
int tblsize;
int totalReferences=0;
int totalReferences_i=0;
int tlbHitCount=0;
int tlbMissCount=0;
char buf[100] = {0};
char addr_str[9] = {0};
unsigned int addr;

// A Queue Node that is implemented using Double Linked List
typedef struct QNode{ 
    struct QNode *prev, *next; 
    unsigned pageNumber;
} QNode; 

// define Queue
typedef struct Queue{ 
    unsigned count;  
    unsigned numberOfFrames; 
    QNode *front, *rear; 
} Queue; 
  
// define Hash
typedef struct Hash{ 
    int capacity; 
    QNode* *array;
} Hash;

// Queue node that will store a pageNumber
QNode* newQNode( unsigned pageNumber){ 
    // allocates memory and assigns pageNumber
    QNode* temp = (QNode *)malloc( sizeof( QNode ) ); 
    temp->pageNumber = pageNumber; 
    // next and previous is NULL
    temp->prev = temp->next = NULL; 
  
    return temp; 
} 

// Initialize an empty queue
Queue* createQueue( int numberOfFrames ) { 
    Queue* queue = (Queue *)malloc( sizeof( Queue ) ); 
    queue->count = 0; 
    queue->front = queue->rear = NULL; 
    // capacity of queue
    queue->numberOfFrames = numberOfFrames; 
  
    return queue; 
} 

// Initialize Hash table
Hash* createHash( int capacity){ 
    Hash* hash = (Hash *) malloc( sizeof( Hash ) ); 
    hash->capacity = capacity; 
  
    // Link hash to queue
    hash->array = (QNode **) malloc( hash->capacity * sizeof( QNode* ) ); 
  
    // Initialize all hash entries as empty 
    int i; 
    for( i = 0; i < hash->capacity; ++i ) 
        hash->array[i] = NULL; 
  
    return hash; 
} 

// A function to check if there is slot available in memory 
int AreAllFramesFull( Queue* queue){ 
    return queue->count == queue->numberOfFrames; 
} 
  
// A utility function to check if queue is empty 
int isQueueEmpty( Queue* queue){ 
    return queue->rear == NULL; 
}

// Delete a node from queue
void deQueue( Queue* queue){ 
    if( isQueueEmpty( queue ) ) 
        return; 
  
    // If this is the only node in list, then change front 
    if (queue->front == queue->rear) 
        queue->front = NULL; 
  
    // Change rear and remove the previous rear 
    QNode* temp = queue->rear; 
    queue->rear = queue->rear->prev; 
  
    if (queue->rear) 
        queue->rear->next = NULL; 
  
    free( temp ); 
  
    // Decrement the number of full frames by 1 
    queue->count--; 
}

// Puts a new element in queue and adds its page number to the hash
void Enqueue( Queue* queue, Hash* hash, unsigned pageNumber){ 
    //if no spot available in memory, remove the rear in queue and clear its corresponding hash value.
    if (AreAllFramesFull(queue)){ 
        hash->array[ queue->rear->pageNumber ] = NULL; 
        deQueue(queue); 
    } 
    
    //make temp node to put in the front
    QNode* temp = newQNode( pageNumber ); 
    temp->next = queue->front; 
    
    //sets temp to front
    if ( isQueueEmpty( queue ) ) 
        queue->rear = queue->front = temp; 
    else{ 
        queue->front->prev = temp; 
        queue->front = temp; 
    } 
  
    //input new nodes pageNumber into the Hash
    hash->array[ pageNumber ] = temp; 
  
    queue->count++; 
} 

// LRU checks for node in queue, if it isn't in the queue, it deletes the least recently used node and inserts the new node to the front of the queue
void LRU(Queue* queue, Hash* hash, unsigned pageNumber){ 
    //check if the pageNumber of the node exists in the hash.
    QNode* reqPage = hash->array[ pageNumber ]; 

    //if not in queue, add to the top; TLB Miss
    if ( reqPage == NULL ){
        Enqueue( queue, hash, pageNumber );
        tlbMissCount++;
    //if not in the front of the queue, move to the front; TLB Hit
    }else if (reqPage != queue->front){ 
        tlbHitCount++;
        reqPage->prev->next = reqPage->next; 
        if (reqPage->next) 
           reqPage->next->prev = reqPage->prev; 
  
        if (reqPage == queue->rear){ 
           queue->rear = reqPage->prev; 
           queue->rear->next = NULL; 
        } 
  
        reqPage->next = queue->front; 
        reqPage->prev = NULL; 
  
        reqPage->next->prev = reqPage; 
  
        queue->front = reqPage; 
    //if at the front of the queue, keep it there; TLB Hit
    } else if (reqPage == queue->front){
        tlbHitCount++;
    }
} 

// FIFO Enqueue's the new element if not in the queue and adds a miss, if it is in the queue, it simply records hit and moves on.
void FIFO(Queue* queue, Hash* hash, unsigned pageNumber){ 
    // checks if the nodes pageNumber exists in the Hash
    QNode* reqPage = hash->array[ pageNumber ]; 
  
    // if page is not in queue, enqueue it; TLB Miss
    if ( reqPage == NULL ){
        tlbMissCount++;
        Enqueue( queue, hash, pageNumber );
    // if page is in queue, leave it; TLB Hit
    }else if (reqPage != NULL){ 
        tlbHitCount++;
    } 
} 

// Main takes in arguments, raises flags and stores info, then it performs FIFO and LRU depending on the arguments
int main(int argc, char *argv[]){
    //input flags
    int flag;
    int i_flag =0;
    int f_flag =0;
    int p_flag =0;
    
    //getopt on arguments in order to raise flags that the user is asking for certain features.
    while ((flag = getopt(argc, argv, "if:p:")) != -1){
        switch(flag){
            case 'i':
                if (i_flag == 0){
                    i_flag++;
                }
                break;
            case 'f': 
                if (f_flag == 0){
                    flushPeriod = atoi(optarg);
                }
                f_flag++;
                break;
            case 'p': 
                if (p_flag == 0){
                    policy = optarg;
                }
                p_flag++;
                break;
        }
    }

    //obtain pgsize and tblsize from command line input.
    pgsize = atoi(argv[argc-2]);
    tblsize = atoi(argv[argc-1]);

    //does calculation for how many bits we will have to shift depending on page size
    const unsigned pgsize_nbits = log(pgsize)/log(2);

    //not enough arguments provided by user.
    if ((argc <5) || (p_flag ==0)){
        printf("Not enough arguments for TLB.\n");
        exit(EXIT_FAILURE);
    }

    //simulate the TLB based FIFO policy.
    if(strcmp(policy,"FIFO")==0){
        // The queue is the TLB table so set the queue size to the tlbsize 
        Queue* queue = createQueue(tblsize); 
  
        //Possible number of buckets in our modulo hash that the value might belong to.
        Hash* hash = createHash(1000003); 
        
        int flushCount =0;
        char *last_char;
        
        //streams value from inputProgram and seperate based on w/ instructions or without.
        while (fgets(buf, sizeof(buf), stdin)) {
            totalReferences++;
            //stops while loop as soon as you see '=' lines
            if (buf[0] == '=')
                continue;
            //if instruction flag is raised and we don't count instructions
            if (i_flag == 1){
                if (buf[0] != 'I'){
                    totalReferences_i++;
                    //if f flag is raised and we must keep count when to flush TLB
                    if (f_flag >0){
                        if (flushPeriod == flushCount){
                            //flushes TLB
                            while(!isQueueEmpty(queue)){
                                deQueue(queue);
                            }
                            int i; 
                            for( i = 0; i < hash->capacity; ++i ) 
                                hash->array[i] = NULL; 
                        }
                    }
                    //obtains line from valgrind, cuts off the string after ',' and does bit shifting based on pgsize.
                    strncpy(addr_str, buf + 3, 50);

                    last_char = strchr(addr_str, ',');
                    if (last_char != NULL) {
                        *last_char = '\0';
                    }

                    sscanf(addr_str, "%x", &addr);
                    unsigned int pageNumber = addr >> pgsize_nbits;
                    //make sure the page number will fit into a bucket in our modulo Hash table
                    pageNumber = pageNumber % 1000003;
                    //call FIFO with this pageNumber (simulate TLB)
                    FIFO(queue,hash,pageNumber);
                    
                    //incrementer for flush period.
                    if(flushCount != flushPeriod){
                        flushCount++;
                    } else if(flushCount == flushPeriod){
                        flushCount =0;
                    }
                    //clear memory
                    memset(buf, 0, 100);
                }
            }else{
                //does the same thing as FIFO above but w/ instruction references included
                if (f_flag >0){
                    if (flushPeriod == flushCount){
                        while(!isQueueEmpty(queue)){
                            deQueue(queue);
                        }
                        int i; 
                        for( i = 0; i < hash->capacity; ++i ) 
                            hash->array[i] = NULL; 
                    }
                }
                strncpy(addr_str, buf + 3, 50);

                last_char = strchr(addr_str, ',');
                if (last_char != NULL) {
                    *last_char = '\0';
                }

                sscanf(addr_str, "%x", &addr);
                unsigned int pageNumber = addr >> pgsize_nbits;
                pageNumber = pageNumber % 1000003;
                FIFO(queue,hash,pageNumber);

                if(flushCount != flushPeriod){
                    flushCount++;
                } else if(flushCount == flushPeriod){
                    flushCount =0;
                }
                memset(buf, 0, 100);
            }
        }
    //simulate TLB using Least Recently Used method    
    }else if(strcmp(policy,"LRU")==0){
        //similar to FIFO above
        Queue* queue = createQueue(tblsize); 
  
        Hash* hash = createHash(1000003); 

        int flushCount =0;
        char *last_char;
        
        //gets string from pipe from inputProgram w/ instructions or without
        while (fgets(buf, sizeof(buf), stdin)) {
            totalReferences++;
            //stops while loop as soon as you reach "=" from valgrind
            if (buf[0] == '=')
                continue;
            //checks i flag in order to ignore or not ignore instruction references.
            if (i_flag == 1){
                if (buf[0] != 'I'){
                    //EXACTLY the same as for FIFO above but calls LRU instead of FIFO
                    totalReferences_i++;
                    if (f_flag >0){
                        if (flushPeriod == flushCount){
                            while(!isQueueEmpty(queue)){
                                deQueue(queue);
                            }
                            int i; 
                            for( i = 0; i < hash->capacity; ++i ) 
                                hash->array[i] = NULL; 
                        }
                    }
                    
                    strncpy(addr_str, buf + 3, 50);

                    last_char = strchr(addr_str, ',');
                    if (last_char != NULL) {
                        *last_char = '\0';
                    }

                    sscanf(addr_str, "%x", &addr);
                    unsigned int pageNumber = addr >> pgsize_nbits;
                    pageNumber = pageNumber % 1000003;
                    LRU(queue,hash,pageNumber);

                    if(flushCount != flushPeriod){
                        flushCount++;
                    } else if(flushCount == flushPeriod){
                        flushCount =0;
                    }
        
                    memset(buf, 0, 100);
                }
            
            }else{
                //EXACTLY the same as above but include instruction references.
                if (f_flag >0){
                    if (flushPeriod == flushCount){
                        while(!isQueueEmpty(queue)){
                            deQueue(queue);
                        }
                        int i; 
                        for( i = 0; i < hash->capacity; ++i ) 
                            hash->array[i] = NULL; 
                    }
                }
                strncpy(addr_str, buf + 3, 50);

                last_char = strchr(addr_str, ',');
                if (last_char != NULL) {
                    *last_char = '\0';
                }

                sscanf(addr_str, "%x", &addr);
                unsigned int pageNumber = addr >> pgsize_nbits;
                pageNumber = pageNumber % 1000003;
                LRU(queue,hash,pageNumber);

                if(flushCount != flushPeriod){
                    flushCount++;
                } else if(flushCount == flushPeriod){
                    flushCount =0;
                }
                
                memset(buf, 0, 100);
                
            }
        }
    
    }

    // printing of results to stdout
    printf("\n");
    if(i_flag >0){
        printf("Total Reference: %d\n",totalReferences_i);    
    }else if(i_flag ==0){
        printf("Total Reference: %d\n",totalReferences);
    }
    printf("Miss Count: %d\n",tlbMissCount);
    printf("Hit Count: %d\n",tlbHitCount);
    printf("\n");

    return 0;
}
