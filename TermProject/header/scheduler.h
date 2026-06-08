#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process.h"

typedef struct {
    int pid;
    int start_time;
    int end_time;
} GanttInterval;

typedef struct {
    GanttInterval intervals[1000];
    int count;
} GanttChart;

void simulate_FCFS(Process processes[], int num_processes, int silent_mode);
void simulate_SJF(Process processes[], int num_processes, int silent_mode);
void simulate_Priority(Process processes[], int num_processes, int silent_mode);
void simulate_SJF_preempt(Process processes[], int num_processes, int silent_mode);
void simulate_Priority_preempt(Process processes[], int num_processes, int silent_mode);
void simulate_RR(Process processes[], int num_processes, int time_quantum, int silent_mode);

void record_gantt(GanttChart *chart, int pid, int start, int end);
void print_chart(GanttChart *chart);
void compute_metrics(Process processes[], int num_processes);
#endif