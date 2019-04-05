//LinkedHashMap was NOT developed by oleg or nikita.
//We found the LinkedHashMap on: https://github.com/carlossdparaujo/linked-hash-map
//Use of outside code is approved by Ioanis Nikolaidis.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include "linkedhashmap.h"

const char* policy;
const char* flushPeriod;
int pgsize;
int tblsize;
int tlbHitCount;
int tlbMissCount;

const unsigned pgsize_nbits = 16;
char buf[100] = {0};
char addr_str[9] = {0};
unsigned int addr;

/*void printSTRUCT(ELEMENT *element){
    printf("KEY: %d ||| VALUE: %f",element->key,element->value);
}*/


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
                    flushPeriod = optarg;
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

    //not enough arguments provided by user.
    if ((argc <5) || (p_flag ==0)){
        printf("Not enough arguments for TLB.\n");
        exit(EXIT_FAILURE);
    }

    //simulate the TLB based FIFO or LRU policy.
    if(strcmp(policy,"FIFO")==0){
        int key = tblsize;
        //set capacity to 16 for now
        LinkedHashMap* linkedHashMap = createLinkedHashMap(16, 0.5);
        //put(linkedHashMap, key, 56);
        //Element* element = get(linkedHashMap,key);
        
        while (fgets(buf, sizeof(buf), stdin)) {
            if (buf[0] == '=')
                continue;
            if (i_flag == 1){
                if (buf[0] != 'I'){
                    strncpy(addr_str, buf + 3, 8);
                    sscanf(addr_str, "%x", &addr);
                    addr = addr >> pgsize_nbits;
                    printf("%d\n",addr);
            
                    put(linkedHashMap, addr, 1);
                    //Element* element = get(linkedHashMap,key);
                    //printf("%x\n",element->value);
                    //Element* element2 = get(linkedHashMap,tblsize);
                    //printf("THE TBLSIZE KEY: %x\n",element2->value);


                    
                    
                    
                    while(key>0){
                        key--;
                    }
                    if(key == 0){
                        delete(linkedHashMap,key);
                    }
                    //key++;
                    memset(buf, 0, 100);
                }
            
            
            
            
            
            }else{
                strncpy(addr_str, buf + 3, 8);
                sscanf(addr_str, "%x", &addr);
                addr = addr >> pgsize_nbits;
                printf("%x\n", addr);
                memset(buf, 0, 100);
            }
        }
    
    
    
    
    
    
    
    
    
    
    }else if(strcmp(policy,"LRU")==0){
        while (fgets(buf, sizeof(buf), stdin)) {
            if (buf[0] == '=')
                continue;
            if (i_flag == 1){
                if (buf[0] != 'I'){
                    strncpy(addr_str, buf + 3, 8);
                    sscanf(addr_str, "%x", &addr);
                    addr = addr >> pgsize_nbits;
                    printf("%x\n", addr);
                    memset(buf, 0, 100);
                }
            }else{
                strncpy(addr_str, buf + 3, 8);
                sscanf(addr_str, "%x", &addr);
                addr = addr >> pgsize_nbits;
                printf("%x\n", addr);
                memset(buf, 0, 100);
            }
        }
    
    
    
    
    }

    return 0;
}
