//QUEUE using linked list: https://www.geeksforgeeks.org/queue-set-2-linked-list-implementation/
//QUEUE code found from programiz.com/dsa/queue and edited for our purpose.


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

void enQueue(int);
void deQueue();
void display();

int list[10], front = -1, rear = -1;

int main(){
    int end = 1;
    while(end==1){
        int inputValue;
        printf("Enter a value: ");
        scanf("%d",&inputValue);
        enQueue(inputValue);
    }

    return 0;
}

void enQueue(int number){
    if(front == -1)
        front = 0;
    rear++;
    list[rear]=number;
}

void deQueue(){
    printf("%d",list[front]);
    front++;
    if(front>rear){
        front = -1;
        rear = -1;
    }
}

void display(){
    int i;
    for(i=0; i<=rear;i++){
        printf("%d",list[i]);
    }
}
