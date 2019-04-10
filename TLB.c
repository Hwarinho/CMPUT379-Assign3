//LinkedHashMap was NOT developed by oleg or nikita.
//We found the LinkedHashMap on: https://www.geeksforgeeks.org/lru-cache-implementation/
//Use of outside code is approved by Ioanis Nikolaidis.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <math.h>

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

// A Queue Node (Queue is implemented using Doubly Linked List) 
typedef struct QNode{ 
    struct QNode *prev, *next; 
    unsigned pageNumber;
} QNode; 

// A Queue (A FIFO collection of Queue Nodes) 
typedef struct Queue{ 
    unsigned count;  
    unsigned numberOfFrames; 
    QNode *front, *rear; 
} Queue; 
  
// A hash (Collection of pointers to Queue Nodes) 
typedef struct Hash{ 
    int capacity; 
    QNode* *array;
} Hash;

QNode* newQNode( unsigned pageNumber){ 
    QNode* temp = (QNode *)malloc( sizeof( QNode ) ); 
    temp->pageNumber = pageNumber; 

    temp->prev = temp->next = NULL; 
  
    return temp; 
} 

Queue* createQueue( int numberOfFrames ) { 
    Queue* queue = (Queue *)malloc( sizeof( Queue ) ); 
  
    queue->count = 0; 
    queue->front = queue->rear = NULL; 
  
    queue->numberOfFrames = numberOfFrames; 
  
    return queue; 
} 

Hash* createHash( int capacity){ 
    // Allocate memory for hash 
    Hash* hash = (Hash *) malloc( sizeof( Hash ) ); 
    hash->capacity = capacity; 
  
    // Create an array of pointers for refering queue nodes 
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

// A utility function to delete a frame from queue 
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
  
    // decrement the number of full frames by 1 
    queue->count--; 
}

void Enqueue( Queue* queue, Hash* hash, unsigned pageNumber){ 
    if (AreAllFramesFull(queue)){ 
        hash->array[ queue->rear->pageNumber ] = NULL; 
        deQueue( queue ); 
    } 
  
    QNode* temp = newQNode( pageNumber ); 
    temp->next = queue->front; 
  
    if ( isQueueEmpty( queue ) ) 
        queue->rear = queue->front = temp; 
    else{ 
        queue->front->prev = temp; 
        queue->front = temp; 
    } 
  
    hash->array[ pageNumber ] = temp; 
  
    queue->count++; 
} 

void LRU(Queue* queue, Hash* hash, unsigned pageNumber){ 
    QNode* reqPage = hash->array[ pageNumber ]; 
  
    if ( reqPage == NULL ){
        Enqueue( queue, hash, pageNumber );
        tlbMissCount++;

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
    } else if (reqPage == queue->front){
        tlbHitCount++;
    }
} 

void FIFO(Queue* queue, Hash* hash, unsigned pageNumber){ 
    QNode* reqPage = hash->array[ pageNumber ]; 
  
    if ( reqPage == NULL ){
        tlbMissCount++;
        Enqueue( queue, hash, pageNumber );

    }else if (reqPage != NULL){ 
        tlbHitCount++;
    } 
} 


int main(int argc, char *argv[]){
    //Identify the flags specified by the user and obtain their respective inputs.
    int flag;
    int i_flag =0;
    int f_flag =0;
    int p_flag =0;
    

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

    const unsigned pgsize_nbits = log(pgsize)/log(2);
    //printf("%d",pgsize_nbits);

    //not enough arguments provided by user.
    if ((argc <5) || (p_flag ==0)){
        printf("Not enough arguments for TLB.\n");
        exit(EXIT_FAILURE);
    }

    //simulate the TLB based FIFO or LRU policy.
    if(strcmp(policy,"FIFO")==0){
        // The queue is the TLB table so set the queue size to the tlbsize 
        Queue* queue = createQueue(tblsize); 
  
        //Possible number of buckets in our modulo hash that the value might belong to.
        Hash* hash = createHash(1000003); 
        
        int flushCount =0;
        char *last_char;
        
        while (fgets(buf, sizeof(buf), stdin)) {
            totalReferences++;
            if (buf[0] == '=')
                continue;
            if (i_flag == 1){
                if (buf[0] != 'I'){
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
                    FIFO(queue,hash,pageNumber);
                    
                    if(flushCount != flushPeriod){
                        flushCount++;
                    } else if(flushCount == flushPeriod){
                        flushCount =0;
                    }
                    
                    memset(buf, 0, 100);
                }
            }else{
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
    }else if(strcmp(policy,"LRU")==0){
        // The queue is the TLB table so set the queue size to the tlbsize 
        Queue* queue = createQueue(tblsize); 
  
        //Possible number of buckets in our modulo hash that the value might belong to.
        Hash* hash = createHash(1000003); 

        int flushCount =0;
        char *last_char;
        
        while (fgets(buf, sizeof(buf), stdin)) {
            totalReferences++;
            if (buf[0] == '=')
                continue;
            if (i_flag == 1){
                if (buf[0] != 'I'){
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
                //printf ("%x\n", queue->front->pageNumber);

                if(flushCount != flushPeriod){
                    flushCount++;
                } else if(flushCount == flushPeriod){
                    flushCount =0;
                }
                
                memset(buf, 0, 100);
                
            }
        }
    
    }

    printf("\n");
    if(i_flag >0){
        printf("Total Reference: %d\n",totalReferences_i);    
    }else if(i_flag ==0){
        printf("Total Reference: %d\n",totalReferences);
    }
    printf("Miss Count: %d\n",tlbMissCount);
    printf("Hit Count: %d\n",tlbHitCount);

    float miss_rate;

    if(i_flag>0){
        miss_rate = ((float) tlbMissCount/(float)totalReferences_i) *100;
    }else if(i_flag==0){
        miss_rate = ((float) tlbMissCount/(float) totalReferences) *100;
    }   
    printf("Miss rate: %f\n",miss_rate);

    FILE *pFile;
    pFile = fopen("statistics.txt", "a");
    fprintf(pFile, "NIKITA IS A CUCKERDINHO" );

    return 0;
}
