#include <stdio.h>
#include <stdlib.h>
#include "pqueue.h"


void initpqueue(pqueue *q){
    q->capacity = MAX_CAPACITY;
    q->size = 0;
}

void insertpqueue(pqueue *q, Process p, int sort_value){
    if (IsFullpqueue(q)) return;
    p.sort_key = sort_value;
    int pos = q->size;
    while (pos > 0){
        int parent = (pos - 1) / 2;
        if (p.sort_key > q->array[parent].sort_key) break;
        else if (p.sort_key == q->array[parent].sort_key){
            if (p.arrival_time > q->array[parent].arrival_time) break;
            else if (p.arrival_time == q->array[parent].arrival_time){
                if (p.pid > q->array[parent].pid) break;
            }
        }
        q->array[pos] = q->array[parent];
        pos = parent;
    }
    q->array[pos] = p;
    q->size++;
};

void removepqueue(pqueue *q){
    if (IsEmptypqueue(q)) return;
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
            else if (q->array[left].sort_key == q->array[smallest].sort_key){
                if (q->array[left].arrival_time < q->array[smallest].arrival_time){
                    smallest = left;
                }
                else if(q->array[left].arrival_time < q->array[smallest].arrival_time){
                    if (q->array[left].pid < q->array[smallest].pid){
                    smallest = left;
                    }
                }
            }
        }
        if (right < q->size){
            if (q->array[right].sort_key < q->array[smallest].sort_key) {
                smallest = right;
            }
            else if (q->array[right].sort_key == q->array[smallest].sort_key){
                if (q->array[right].arrival_time < q->array[smallest].arrival_time){
                    smallest = right;
                }
                else if(q->array[right].arrival_time < q->array[smallest].arrival_time){
                    if (q->array[right].pid < q->array[smallest].pid){
                    smallest = right;
                    }
                }
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
}

Process toppqueue(pqueue *q){
    if (IsEmptypqueue(q)){
        Process empty = {0}; // This safely initializes EVERY field in the struct to 0
        empty.pid = -1;
        return empty;
    } 
    return q->array[0];
}

int IsEmptypqueue(pqueue *q){
    return (q -> size == 0);
}

int IsFullpqueue(pqueue *q){
    return (q -> size >= q->capacity);
}

