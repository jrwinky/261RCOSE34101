#ifndef PROCESS_H
#define PROCESS_H

typedef struct{
    int pid;
    int arrival_time;
    int cpu_burst_time;
    int io_burst_time;
    int io_frequency;
    int priority;

    int remaining_time;
    int waiting_time;
    int turnaround_time;
    int sort_key;
    int ticks_since_io;
    int current_io_countdown;
} Process;

void create_process(Process *p);
void reset_pid_tracker();
int is_pid_used(int pid);

#endif