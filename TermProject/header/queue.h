#ifndef QUEUE_H
#define QUEUE_H

#include "process.h"

#define MAX_CAPACITY 100

typedef struct{
    Process array[MAX_CAPACITY];
    int capacity;
    int front;
    int rear;
    int size;
} queue;

void initqueue(queue *q);

void enqueue(queue *q, Process p);

void dequeue(queue *q);

Process topqueue(queue *q);

int IsEmptyqueue(queue *q);

void destroyqueue(queue *q);

#endif