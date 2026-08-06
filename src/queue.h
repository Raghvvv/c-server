#ifndef MYQUEUE_H
#define MYQUEUE_H

#include<stdbool.h>
#include<pthread.h>

typedef struct{
    int *arr;
    int capacity;
    int size;
    int front;
    int back;
    pthread_mutex_t lock;
    
} Myqueue;

//initalization:
Myqueue* queueInit(int k);
void queueFree(Myqueue*obj);

//core functions:
void enqueue(Myqueue*obj,int value);
int dequeue(Myqueue*obj);

#endif