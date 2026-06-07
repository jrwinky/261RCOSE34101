#ifndef PQUEUE_H
#define PQUEUE_H

#include "process.h"

#define MAX_CAPACITY 100

typedef struct{
    Process array[MAX_CAPACITY];
    int capacity;
    int size;
} pqueue;

void initpqueue(pqueue *q);

void insert(pqueue *q, Process p, int priority);

void remove(pqueue *q);

Process toppqueue(pqueue *q);

int IsEmpty(pqueue *q);

int IsFull(pqueue *q);

#endif