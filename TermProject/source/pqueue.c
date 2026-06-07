#include <stdio.h>
#include <stdlib.h>
#include "pqueue.h"


void initpqueue(pqueue *q){
    q->capacity = MAX_CAPACITY;
    q->size = 0;
}

void insert(pqueue *q, Process p, int sort_value){
    if (IsFull) return;
    p.sort_key = sort_value;
    int pos = q->size;
    while (pos > 0){
        int parent = (pos - 1) / 2;
        if (p.sort_key > q->array[parent].sort_key) break;
        q->array[pos] = q->array[parent];
        pos = parent;
    }
    q->array[pos] = p;
    q->size++;
};

void remove(pqueue *q){
    if (IsEmpty) return;
    int pos = 0;
    q->array[0] = q->array[q->size - 1];
    q->size--;
    while (pos < q->size){
        int left = pos * 2 + 1;
        int right = pos * 2 + 2;
        int smallest = pos;
        if (left < q->size){
            if (q->array[left].sort_key < q->array[smallest].sort_key) {
                smallest = left;
            }
        }
        if (right < q->size){
            if (q->array[right].sort_key < q->array[smallest].sort_key) {
                smallest = right;
            }
        }
        if (smallest == pos) {
            break;
        }
        Process temp = q->array[pos];
        q->array[pos] = q->array[smallest];
        q->array[smallest] = temp;
        pos = smallest;
    }
};

Process toppqueue(pqueue *q){
    if (IsEmpty) return;
    return q->array[0];
};

int IsEmpty(pqueue *q){
    return (q -> size == 0);
}

int IsFull(pqueue *q){
    return (q -> size >= q->capacity);
}



