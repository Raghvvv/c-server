#include<stdio.h>
// #include<stdbool.h>
#include<stdlib.h>
#include<pthread.h>
#include"queue.h"




Myqueue* queueInit(int k){
    Myqueue* obj=(Myqueue*)malloc(sizeof(Myqueue));
    obj->capacity=k;
    obj->size=0;
    obj->front=0;
    obj->back=-1;
    obj->arr=(int*)malloc(k*sizeof(int));
    pthread_mutex_init(&obj->lock,NULL);
    return obj;
}
static bool isFull(Myqueue*obj){
    return (obj->size==obj->capacity);
}
static bool isEmpty(Myqueue*obj){
    return (obj->size==0);
}
void enqueue(Myqueue*obj,int value){
    pthread_mutex_lock(&obj->lock);
    if(isFull(obj)){
        pthread_mutex_unlock(&obj->lock);
        return;
    }
    int insertIdx=(obj->back+1)%(obj->capacity);
    obj->back=insertIdx;
    obj->arr[obj->back]=value;
    obj->size++;
    pthread_mutex_unlock(&obj->lock);


}
int dequeue(Myqueue*obj){
    pthread_mutex_lock(&obj->lock);
    if(isEmpty(obj)){
        pthread_mutex_unlock(&obj->lock);
        return -1;
    }
    int fd=obj->arr[obj->front];
    obj->front=(obj->front+1)%(obj->capacity);
    obj->size--;
    pthread_mutex_unlock(&obj->lock);
    return fd;

}


void queueFree(Myqueue*obj){
    pthread_mutex_destroy(&obj->lock);
    free(obj->arr);
    free(obj);
}
    
