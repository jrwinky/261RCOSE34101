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

void insertpqueue(pqueue *q, Process p, int priority);

void removepqueue(pqueue *q);

Process toppqueue(pqueue *q);

int IsEmptypqueue(pqueue *q);

int IsFullpqueue(pqueue *q);

#endif