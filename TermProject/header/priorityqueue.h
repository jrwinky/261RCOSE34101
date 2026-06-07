#ifndef PRIORITYQUEUE_H
#define PRIORITYQUEUE_H

#include "process.h"

#define MAX_CAPACITY 100

typedef struct{
    Process array[MAX_CAPACITY];
    int capacity;
    int front;
    int rear;
    int size;
} pqueue;

void initpqueue(pqueue *q);

void insert(pqueue *q, Process p);

void remove(pqueue *q);

Process toppqueue(pqueue *q);

int IsEmpty(pqueue *q);

void destroypqueue(pqueue *q);











#endif