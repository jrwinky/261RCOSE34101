#include <stdlib.h>
#include "process.h"

static int used_pids[20] = {0}; 
static int used_count = 0;

// Helper: Check if a PID is on the Guest List
int is_pid_used(int pid) {
    for (int i = 0; i < used_count; i++) {
        if (used_pids[i] == pid) return 1;
    }
    return 0;
}

// Reset function - CALL THIS WHEN CREATING A NEW DATASET
void reset_pid_tracker() {
    used_count = 0;
}

void create_process(Process *p){
    int new_pid;
    do {
        new_pid = (rand() % 9000) + 1000;
    } while (is_pid_used(new_pid));
    p->pid = new_pid;
    used_pids[used_count++] = new_pid;
    p->arrival_time = rand() % 20; // Generate random arrival time between 0 and 19   
    p->cpu_burst_time = (rand() % 11) + 10; // Generate CPU burst between 10 and 20
    p->priority = (rand() % 5) + 1; // Priority between 1 and 5 (1 being highest priority)
    int is_cpu_bound = (rand() % 100); // Roll a number from 0 to 99
    
    if (is_cpu_bound < 20) {
        p->io_frequency = p->cpu_burst_time + 1; 
        p->io_burst_time = 0; 
    } else {
        p->io_frequency = (rand() % 5) + 2; 
        p->io_burst_time = (rand() % 3) + 1; 
    }
    // Initialize trackers for the simulator
    // remaining_time starts equal to the full CPU burst time
    p->remaining_time = p->cpu_burst_time;
    p->waiting_time = 0;
    p->turnaround_time = 0;
    p->ticks_since_io = 0;
    p->current_io_countdown = p->io_burst_time;
}

