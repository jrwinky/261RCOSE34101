#ifndef PROCESS_H
#define PROCESS_H

typedef struct{
    int pid;
    int arrival_time;
    int cpu_burst_time;
    int io_burst_time;
    int io_frequency;
    int priority;
    int sort_key;

    int remaining_time;
    int waiting_time;
    int turnaround_time;
} Process;

void create_process(Process *p);

#endif