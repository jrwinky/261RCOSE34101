#include <stdlib.h>
#include <stdio.h>
#include "queue.h"

void initqueue(queue *q){
    q->capacity = MAX_CAPACITY;
    q->size = 0;
    q->front = 0;
    q->rear = -1;
}

void enqueue(queue *q, Process p){
    if (IsFullqueue) return;
    q->array[q->rear] = p;
    q->rear = (q->rear + 1) % q->capacity;
    q->size++;
}

void dequeue(queue *q){
    if (IsEmptyqueue) return;
    q->front = (q->front + 1) % q->capacity;
    q->size--;
}

Process topqueue(queue *q){
    if (IsEmptyqueue) return;
    return q->array[q->front];
}

int IsEmptyqueue(queue *q){
    return (q->size == 0);
}

int IsFullqueue(queue *q){
    return (q->size >= q->capacity);
}