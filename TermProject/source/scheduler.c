#include <stdio.h>
#include "queue.h"
#include "pqueue.h"
#include "process.h"
#include "scheduler.h"



void record_gantt(GanttChart *chart, int pid, int start, int end) {
    chart->intervals[chart->count].pid = pid;
    chart->intervals[chart->count].start_time = start;
    chart->intervals[chart->count].end_time = end;
    chart->count++;
}

void print_chart(GanttChart *chart) {
    // 1. Print Gantt Chart
    printf("\n--- FINAL GANTT CHART ---\n");
    for (int i = 0; i < chart->count; i++) {
        printf("(%d, %d) P%d", chart->intervals[i].start_time, chart->intervals[i].end_time, chart->intervals[i].pid);
        if (i < chart->count - 1) {
            printf(" -> ");
        }
    }
    printf("\n");
}

void compute_metrics(Process processes[], int num_processes){
    float total_turnaround = 0;
    float total_wait = 0;
    float total_busy_time = 0;
    float total_simulation_time = 0;

    printf("\n--- PERFORMANCE METRICS ---\n");
    printf("PID\tTurnaround Time\tWaiting Time\n");
    printf("--------------------------------------\n");
    
    for (int i = 0; i < num_processes; i++) {
        printf("%d\t%d\t\t%d\n", processes[i].pid, processes[i].turnaround_time, processes[i].waiting_time);
        total_turnaround += processes[i].turnaround_time;
        total_wait += processes[i].waiting_time;
        total_busy_time += processes[i].cpu_burst_time;
        int finish_time = processes[i].arrival_time + processes[i].turnaround_time;
        if (finish_time > total_simulation_time) {
            total_simulation_time = finish_time;
        }
    }

    printf("--------------------------------------\n");
    printf("Average Turnaround Time: %.2f\n", total_turnaround / num_processes);
    printf("Average Waiting Time:    %.2f\n", total_wait / num_processes);
    printf("Average CPU Utilization: %.2f\n", total_busy_time / num_processes);
    printf("Average Throughput: %.2f\n", total_simulation_time / num_processes);
}

void simulate_FCFS(Process processes[], int num_processes, int silent_mode) {
    GanttChart chart;
    chart.count = 0;
    int current_process_start = 0;

    queue ready_queue;
    queue io_queue;
    
    initqueue(&ready_queue);
    initqueue(&io_queue);

    int arrived[100] = {0};
    int total_arrived = 0; 
    int current_time = 0;
    int completed_processes = 0;
    
    int is_cpu_busy = 0;
    Process active_cpu_process;

    printf("\n============================================\n");
    printf("   STARTING FCFS WITH I/O QUEUE SIMULATION\n");
    printf("============================================\n");

    // The loop runs until every single process has finished its full CPU burst
    while (completed_processes < num_processes) {
        
        

        // -----------------------------------------------------------------
        // STEP 1: CHECK FOR NEW EXTERNAL ARRIVALS
        // -----------------------------------------------------------------
        if (total_arrived < num_processes){
            for (int i = 0; i < num_processes; i++) {
                if (arrived[i] == 0 && processes[i].arrival_time == current_time) {
                    enqueue(&ready_queue, processes[i]);
                    arrived[i] = 1; 
                    total_arrived++;
                   // printf("[Time %02d] Process %d arrived to Ready Queue.\n", current_time, processes[i].pid);
                }
            }
        }

        // -----------------------------------------------------------------
        // STEP 2: ALLOCATING THE CPU
        // -----------------------------------------------------------------
        if (!is_cpu_busy && !IsEmptyqueue(&ready_queue)) {
            active_cpu_process = topqueue(&ready_queue);
            dequeue(&ready_queue);

            is_cpu_busy = 1;
            
            current_process_start = current_time;
        }
        for (int count = 0; count < ready_queue.size; count++) {
            
            int index = (ready_queue.front + count) % ready_queue.capacity; 
            
            ready_queue.array[index].waiting_time++;
        }
        // -----------------------------------------------------------------
        // STEP 4: EXECUTING THE CPU TICK
        // -----------------------------------------------------------------
        for (int i = 0; i < io_queue.size; ) {
            io_queue.array[i].current_io_countdown--; // Decrement everyone's timer!
            if (io_queue.array[i].current_io_countdown == 0) {
                enqueue(&ready_queue, io_queue.array[i]);
                                
                //printf("[Time %02d] Process %d finished I/O and returned to Ready Queue.\n", 
                       //current_time + 1, io_queue.array[i].pid);
                
                for (int j = i; j < io_queue.size - 1; j++) {
                    io_queue.array[j] = io_queue.array[j + 1];
                }
                io_queue.size--;
                
            } else {
                i++;
            }
        }
        if (is_cpu_busy) {
            active_cpu_process.remaining_time--;
            active_cpu_process.ticks_since_io++;

            if (active_cpu_process.remaining_time == 0) {
                is_cpu_busy = 0;
                record_gantt(&chart, active_cpu_process.pid, current_process_start, current_time + 1);
                completed_processes++;
                for (int k = 0; k < num_processes; k++) {
                    if (processes[k].pid == active_cpu_process.pid) {
                        processes[k].turnaround_time = (current_time + 1) - processes[k].arrival_time;
                        processes[k].waiting_time = active_cpu_process.waiting_time;
                    }
                }
            }
           
            else if (active_cpu_process.ticks_since_io == active_cpu_process.io_frequency) {
                is_cpu_busy = 0;
                record_gantt(&chart, active_cpu_process.pid, current_process_start, current_time + 1);
                active_cpu_process.ticks_since_io = 0;
                active_cpu_process.current_io_countdown = active_cpu_process.io_burst_time;
                
                // Evict from CPU and send to I/O Waiting Queue
                io_queue.array[io_queue.size] = active_cpu_process;
                io_queue.size++;
                //printf("[Time %02d] Process %d reached I/O frequency limit. Evicted to I/O Queue.\n", 
                       //current_time + 1, active_cpu_process.pid);
            }
        }
        
        current_time++;
        
        // Failsafe timeout for safety
        if (current_time > 2000) {
            printf("Error: Simulator exceeded maximum safe runtime execution limit.\n");
            break;
        }
    }
    if(!silent_mode){
    print_chart(&chart);
    printf("============================================\n");
    printf("   SIMULATION COMPLETE AT TIME %d\n", current_time);
    printf("============================================\n");
    }
    compute_metrics(processes, num_processes);
    
}

void simulate_SJF(Process processes[], int num_processes, int silent_mode) {
    GanttChart chart;
    chart.count = 0;
    int current_process_start = 0; // Tracks when the current CPU process started

    pqueue ready_queue;   // NEW: Using the Priority Heap!
    queue io_queue;       // I/O stays standard FIFO
    
    initpqueue(&ready_queue);
    initqueue(&io_queue);

    int arrived[100] = {0};
    int total_arrived = 0; 
    int current_time = 0;
    int completed_processes = 0;
    
    int is_cpu_busy = 0;
    Process active_cpu_process;

    printf("\n============================================\n");
    printf("   STARTING SJF (NON-PREEMPTIVE) SIMULATION\n");
    printf("============================================\n");

    while (completed_processes < num_processes) {

        // 1. CHECK FOR ARRIVALS
        if (total_arrived < num_processes){
            for (int i = 0; i < num_processes; i++) {
                if (arrived[i] == 0 && processes[i].arrival_time == current_time) {
                    insertpqueue(&ready_queue, processes[i], processes[i].remaining_time);
                    arrived[i] = 1; 
                    total_arrived++;
                    //printf("[Time %02d] Process %d arrived to Ready Queue.\n", current_time, processes[i].pid);
                }
            }
        }

        // 2. ALLOCATING THE CPU
        if (!is_cpu_busy && !IsEmptypqueue(&ready_queue)) {
            // NEW: Read the root of the Min-Heap
            active_cpu_process = toppqueue(&ready_queue); 
            removepqueue(&ready_queue); // Bubble down!
            
            is_cpu_busy = 1;
            current_process_start = current_time;
            //printf("[Time %02d] Process %d loaded onto CPU.\n", current_time, active_cpu_process.pid);
        }
        for (int i = 0; i < ready_queue.size; i++) {
            ready_queue.array[i].waiting_time++;
        }
        for (int i = 0; i < io_queue.size; ) {
            io_queue.array[i].current_io_countdown--;

            if (io_queue.array[i].current_io_countdown == 0) {
                
                insertpqueue(&ready_queue, io_queue.array[i], io_queue.array[i].remaining_time);
                //if(!silent_mode){
                //printf("[Time %02d] Process %d finished I/O and returned to Ready Queue.\n", 
                //       current_time + 1, io_queue.array[i].pid);
                //}
                
                for (int j = i; j < io_queue.size - 1; j++) {
                    io_queue.array[j] = io_queue.array[j + 1];
                }
                io_queue.size--;
            } else {
                i++;
            }
        }
        // 4. EXECUTING THE CPU TICK
        if (is_cpu_busy) {
            active_cpu_process.remaining_time--;
            active_cpu_process.ticks_since_io++;

            if (active_cpu_process.remaining_time == 0) {
                is_cpu_busy = 0;
                completed_processes++;
                record_gantt(&chart, active_cpu_process.pid, current_process_start, current_time + 1);
                for (int k = 0; k < num_processes; k++) {
                    if (processes[k].pid == active_cpu_process.pid) {
                        processes[k].turnaround_time = (current_time + 1) - processes[k].arrival_time;
                        processes[k].waiting_time = active_cpu_process.waiting_time;
                    }
                }
                //printf("[Time %02d] Process %d COMPLETELY FINISHED.\n", current_time + 1, active_cpu_process.pid);
            }
            else if (active_cpu_process.ticks_since_io == active_cpu_process.io_frequency) {
                is_cpu_busy = 0;
                record_gantt(&chart, active_cpu_process.pid, current_process_start, current_time + 1);
                active_cpu_process.ticks_since_io = 0;
                active_cpu_process.current_io_countdown = active_cpu_process.io_burst_time;
                io_queue.array[io_queue.size] = active_cpu_process;
                io_queue.size++; // Send to standard FIFO I/O
                //printf("[Time %02d] Process %d hit I/O frequency. Evicted to I/O Queue.\n", 
                       //current_time + 1, active_cpu_process.pid);
            }
        }
        
        current_time++;
        if (current_time > 2000) {
            printf("Error: Simulator exceeded maximum safe runtime execution limit.\n");
            break;
        }
    }
    if(!silent_mode){
    print_chart(&chart);
    printf("============================================\n");
    printf("   SIMULATION COMPLETE AT TIME %d\n", current_time);
    printf("============================================\n");
    }
    compute_metrics(processes, num_processes);
}

void simulate_Priority(Process processes[], int num_processes, int silent_mode) {
    GanttChart chart;
    chart.count = 0;
    int current_process_start = 0; // Tracks when the current CPU process started

    pqueue ready_queue;   // NEW: Using the Priority Heap!
    queue io_queue;       // I/O stays standard FIFO
    
    initpqueue(&ready_queue); 
    initqueue(&io_queue);

    int arrived[100] = {0};
    int total_arrived = 0; 
    int current_time = 0;
    int completed_processes = 0;
    
    int is_cpu_busy = 0;
    Process active_cpu_process;

    printf("\n============================================\n");
    printf("   STARTING SJF (NON-PREEMPTIVE) SIMULATION\n");
    printf("============================================\n");

    while (completed_processes < num_processes) {

        // 1. CHECK FOR ARRIVALS
        if (total_arrived < num_processes){
            for (int i = 0; i < num_processes; i++) {
                if (arrived[i] == 0 && processes[i].arrival_time == current_time) {
                    insertpqueue(&ready_queue, processes[i], processes[i].priority);
                    arrived[i] = 1; 
                    total_arrived++;
                    //printf("[Time %02d] Process %d arrived to Ready Queue.\n", current_time, processes[i].pid);
                }
            }
        }

        // 2. ALLOCATING THE CPU
        if (!is_cpu_busy && !IsEmptypqueue(&ready_queue)) {
            // NEW: Read the root of the Min-Heap
            active_cpu_process = toppqueue(&ready_queue); 
            removepqueue(&ready_queue); // Bubble down!
            
            is_cpu_busy = 1;
            current_process_start = current_time;
            //printf("[Time %02d] Process %d loaded onto CPU.\n", current_time, active_cpu_process.pid);
        }
        for (int i = 0; i < ready_queue.size; i++) {
            ready_queue.array[i].waiting_time++;
        }
        for (int i = 0; i < io_queue.size; ) {
            io_queue.array[i].current_io_countdown--;

            if (io_queue.array[i].current_io_countdown == 0) {
                // NEW: insertpqueue back into Heap sorted by remaining time
                insertpqueue(&ready_queue, io_queue.array[i], io_queue.array[i].priority);
                
                //printf("[Time %02d] Process %d finished I/O and returned to Ready Queue.\n", 
                       //current_time + 1, io_queue.array[i].pid);
                
                for (int j = i; j < io_queue.size - 1; j++) {
                    io_queue.array[j] = io_queue.array[j + 1];
                }
                io_queue.size--;
            } else {
                i++;
            }
        }
        // 4. EXECUTING THE CPU TICK
        if (is_cpu_busy) {
            active_cpu_process.remaining_time--;
            active_cpu_process.ticks_since_io++;

            if (active_cpu_process.remaining_time == 0) {
                is_cpu_busy = 0;
                record_gantt(&chart, active_cpu_process.pid, current_process_start, current_time + 1);
                completed_processes++;
                for (int k = 0; k < num_processes; k++) {
                    if (processes[k].pid == active_cpu_process.pid) {
                        processes[k].turnaround_time = (current_time + 1) - processes[k].arrival_time;
                        processes[k].waiting_time = active_cpu_process.waiting_time;
                    }
                }
                //printf("[Time %02d] Process %d COMPLETELY FINISHED.\n", current_time + 1, active_cpu_process.pid);
            }
            else if (active_cpu_process.ticks_since_io == active_cpu_process.io_frequency) {
                is_cpu_busy = 0;
                record_gantt(&chart, active_cpu_process.pid, current_process_start, current_time + 1);
                active_cpu_process.ticks_since_io = 0;
                active_cpu_process.current_io_countdown = active_cpu_process.io_burst_time;
                io_queue.array[io_queue.size] = active_cpu_process;
                io_queue.size++; // Send to standard FIFO I/O
                //printf("[Time %02d] Process %d hit I/O frequency. Evicted to I/O Queue.\n", 
                       //current_time + 1, active_cpu_process.pid);
            }
        }
        current_time++;
        if (current_time > 2000) {
            printf("Error: Simulator exceeded maximum safe runtime execution limit.\n");
            break;
        }
    }
    
    if(!silent_mode){
    print_chart(&chart);
    printf("============================================\n");
    printf("   SIMULATION COMPLETE AT TIME %d\n", current_time);
    printf("============================================\n");
    }
    compute_metrics(processes, num_processes);
}

void simulate_SJF_preempt(Process processes[], int num_processes, int silent_mode) {
    GanttChart chart;
    chart.count = 0;
    int current_process_start = 0; // Tracks when the current CPU process started

    pqueue ready_queue;
    queue io_queue;
    
    ready_queue.size = 0; 
    initqueue(&io_queue);

    int arrived[100] = {0};
    int total_arrived = 0; 
    int current_time = 0;
    int completed_processes = 0;
    
    int is_cpu_busy = 0;
    Process active_cpu_process;

    printf("\n============================================\n");
    printf("   STARTING PREEMPTIVE SJF SIMULATION\n");
    printf("============================================\n");

    while (completed_processes < num_processes) {
        // 2. CHECK FOR ARRIVALS
        if (total_arrived < num_processes){
            for (int i = 0; i < num_processes; i++) {
                if (arrived[i] == 0 && processes[i].arrival_time == current_time) {
                    insertpqueue(&ready_queue, processes[i], processes[i].remaining_time);
                    arrived[i] = 1; 
                    total_arrived++;
                    //printf("[Time %02d] Process %d arrived to Ready Queue.\n", current_time, processes[i].pid);
                }
            }
        }

        // STEP 3: THE PREEMPTION CHECK
        if (is_cpu_busy && !IsEmptypqueue(&ready_queue)) {
            Process top_waiting = toppqueue(&ready_queue);
            
            // If the top waiting process has strictly LESS remaining time...
            if (top_waiting.remaining_time < active_cpu_process.remaining_time) {
                //printf("[Time %02d] PREEMPTION! Process %d kicked off CPU by Process %d.\n", 
                       //current_time, active_cpu_process.pid, top_waiting.pid);
                record_gantt(&chart, active_cpu_process.pid, current_process_start, current_time);
                // Throw the running process back into the heap!
                insertpqueue(&ready_queue, active_cpu_process, active_cpu_process.remaining_time);
                
                // Free the CPU so the new process can be loaded in Step 4
                is_cpu_busy = 0; 
            }
        }

        // 4. ALLOCATING THE CPU
        if (!is_cpu_busy && !IsEmptypqueue(&ready_queue)) {
            active_cpu_process = toppqueue(&ready_queue); 
            removepqueue(&ready_queue);
            
            is_cpu_busy = 1;
            current_process_start = current_time;
            //printf("[Time %02d] Process %d loaded onto CPU.\n", current_time, active_cpu_process.pid);
        }
        for (int i = 0; i < ready_queue.size; i++) {
            ready_queue.array[i].waiting_time++;
        }
        for (int i = 0; i < io_queue.size; ) {
            io_queue.array[i].current_io_countdown--;

            if (io_queue.array[i].current_io_countdown == 0) {
                insertpqueue(&ready_queue, io_queue.array[i], io_queue.array[i].remaining_time);
                //printf("[Time %02d] Process %d finished I/O and returned to Ready Queue.\n", 
                       //current_time + 1, io_queue.array[i].pid);
                
                for (int j = i; j < io_queue.size - 1; j++) {
                    io_queue.array[j] = io_queue.array[j + 1];
                }
                io_queue.size--;
            } else {
                i++;
            }
        }
        // 5. EXECUTING THE CPU TICK
        if (is_cpu_busy) {
            active_cpu_process.remaining_time--;
            active_cpu_process.ticks_since_io++;

            if (active_cpu_process.remaining_time == 0) {
                is_cpu_busy = 0;
                completed_processes++;
                record_gantt(&chart, active_cpu_process.pid, current_process_start, current_time + 1);
                for (int k = 0; k < num_processes; k++) {
                    if (processes[k].pid == active_cpu_process.pid) {
                        processes[k].turnaround_time = (current_time + 1) - processes[k].arrival_time;
                        processes[k].waiting_time = active_cpu_process.waiting_time;
                    }
                }
                //printf("[Time %02d] Process %d COMPLETELY FINISHED.\n", current_time + 1, active_cpu_process.pid);
            }
            else if (active_cpu_process.ticks_since_io == active_cpu_process.io_frequency) {
                is_cpu_busy = 0;
                record_gantt(&chart, active_cpu_process.pid, current_process_start, current_time + 1);
                active_cpu_process.ticks_since_io = 0;
                active_cpu_process.current_io_countdown = active_cpu_process.io_burst_time;
                io_queue.array[io_queue.size] = active_cpu_process;
                io_queue.size++;
            }
        }
        current_time++;
        if (current_time > 2000) {
            printf("Error: Simulator exceeded maximum safe runtime execution limit.\n");
            break;
        }
    }
    
    if(!silent_mode){
    print_chart(&chart);
    printf("============================================\n");
    printf("   SIMULATION COMPLETE AT TIME %d\n", current_time);
    printf("============================================\n");
    }
    compute_metrics(processes, num_processes);
}

void simulate_Priority_preempt(Process processes[], int num_processes, int silent_mode) {
    GanttChart chart;
    chart.count = 0;
    int current_process_start = 0; // Tracks when the current CPU process started

    pqueue ready_queue;
    queue io_queue;
    
    initpqueue(&ready_queue); 
    initqueue(&io_queue);

    int arrived[100] = {0};
    int total_arrived = 0; 
    int current_time = 0;
    int completed_processes = 0;
    
    int is_cpu_busy = 0;
    Process active_cpu_process;

    printf("\n============================================\n");
    printf("   STARTING PREEMPTIVE SJF SIMULATION\n");
    printf("============================================\n");

    while (completed_processes < num_processes) {
        
        // 1. I/O PROGRESS (Parallel)

        // 2. CHECK FOR ARRIVALS
        if (total_arrived < num_processes){
            for (int i = 0; i < num_processes; i++) {
                if (arrived[i] == 0 && processes[i].arrival_time == current_time) {
                    insertpqueue(&ready_queue, processes[i], processes[i].priority);
                    arrived[i] = 1; 
                    total_arrived++;
                    //printf("[Time %02d] Process %d arrived to Ready Queue.\n", current_time, processes[i].pid);
                }
            }
        }

        // -----------------------------------------------------------------
        // STEP 3: THE PREEMPTION CHECK (NEW!)
        // -----------------------------------------------------------------
        if (is_cpu_busy && !IsEmptypqueue(&ready_queue)) {
            Process top_waiting = toppqueue(&ready_queue);
            
            // If the top waiting process has strictly LESS remaining time...
            if (top_waiting.priority < active_cpu_process.priority) {
                //printf("[Time %02d] PREEMPTION! Process %d kicked off CPU by Process %d.\n", 
                       //current_time, active_cpu_process.pid, top_waiting.pid);
                record_gantt(&chart, active_cpu_process.pid, current_process_start, current_time);
                // Throw the running process back into the heap!
                insertpqueue(&ready_queue, active_cpu_process, active_cpu_process.priority);
                
                // Free the CPU so the new process can be loaded in Step 4
                is_cpu_busy = 0; 
            }
        }

        // 4. ALLOCATING THE CPU
        if (!is_cpu_busy && !IsEmptypqueue(&ready_queue)) {
            active_cpu_process = toppqueue(&ready_queue); 
            removepqueue(&ready_queue);
            
            is_cpu_busy = 1;
            current_process_start = current_time;
            //printf("[Time %02d] Process %d loaded onto CPU.\n", current_time, active_cpu_process.pid);
        }
        for (int i = 0; i < ready_queue.size; i++) {
            ready_queue.array[i].waiting_time++;
        }
        for (int i = 0; i < io_queue.size; ) {
            io_queue.array[i].current_io_countdown--;

            if (io_queue.array[i].current_io_countdown == 0) {
                insertpqueue(&ready_queue, io_queue.array[i], io_queue.array[i].priority);
                //printf("[Time %02d] Process %d finished I/O and returned to Ready Queue.\n", 
                       //current_time, io_queue.array[i].pid);
                
                for (int j = i; j < io_queue.size - 1; j++) {
                    io_queue.array[j] = io_queue.array[j + 1];
                }
                io_queue.size--;
            } else {
                i++;
            }
        }
        // 5. EXECUTING THE CPU TICK
        if (is_cpu_busy) {
            active_cpu_process.remaining_time--;
            active_cpu_process.ticks_since_io++;

            if (active_cpu_process.remaining_time == 0) {
                is_cpu_busy = 0;
                completed_processes++;
                record_gantt(&chart, active_cpu_process.pid, current_process_start, current_time + 1);
                //printf("[Time %02d] Process %d COMPLETELY FINISHED.\n", current_time + 1, active_cpu_process.pid);
                for (int k = 0; k < num_processes; k++) {
                    if (processes[k].pid == active_cpu_process.pid) {
                        processes[k].turnaround_time = (current_time + 1) - processes[k].arrival_time;
                        processes[k].waiting_time = active_cpu_process.waiting_time;
                    }
                }
            }
            else if (active_cpu_process.ticks_since_io == active_cpu_process.io_frequency) {
                is_cpu_busy = 0;
                record_gantt(&chart, active_cpu_process.pid, current_process_start, current_time + 1);
                active_cpu_process.ticks_since_io = 0;
                active_cpu_process.current_io_countdown = active_cpu_process.io_burst_time;
                io_queue.array[io_queue.size] = active_cpu_process;
                io_queue.size++; 
                //printf("[Time %02d] Process %d hit I/O frequency. Evicted to I/O Queue.\n", 
                       //current_time + 1, active_cpu_process.pid);
            }
        }
        current_time++;
        if (current_time > 2000) {
            printf("Error: Simulator exceeded maximum safe runtime execution limit.\n");
            break;
        }
    }
    
    if(!silent_mode){
    print_chart(&chart);
    printf("============================================\n");
    printf("   SIMULATION COMPLETE AT TIME %d\n", current_time);
    printf("============================================\n");
    }
    compute_metrics(processes, num_processes);
}

void simulate_RR(Process processes[], int num_processes, int time_quantum, int silent_mode) {
    GanttChart chart;
    chart.count = 0;
    int current_process_start = 0;

    // Round Robin uses standard FIFO queues!
    queue ready_queue;
    queue io_queue;
    
    initqueue(&ready_queue);
    initqueue(&io_queue);

    int arrived[100] = {0};
    int total_arrived = 0; 
    int current_time = 0;
    int completed_processes = 0;
    
    int is_cpu_busy = 0;
    Process active_cpu_process;
    
    // We need TWO trackers now: one for I/O limits, one for the Quantum limit       
    int current_quantum_ticks = 0; 

    printf("\n============================================\n");
    printf("   STARTING ROUND ROBIN (TQ = %d) SIMULATION\n", time_quantum);
    printf("============================================\n");

    while (completed_processes < num_processes) {
        
        // 1. I/O PROGRESS (Parallel)
        
        // 2. CHECK FOR ARRIVALS
        if (total_arrived < num_processes){
            for (int i = 0; i < num_processes; i++) {
                if (arrived[i] == 0 && processes[i].arrival_time == current_time) {
                    enqueue(&ready_queue, processes[i]);
                    arrived[i] = 1; 
                    total_arrived++;
                    //printf("[Time %02d] Process %d arrived to Ready Queue.\n", current_time, processes[i].pid);
                }
            }
        }
        
        // 4. ALLOCATING THE CPU
        if (!is_cpu_busy && !IsEmptyqueue(&ready_queue)) {
            active_cpu_process = topqueue(&ready_queue); 
            dequeue(&ready_queue);
            
            is_cpu_busy = 1;
            current_quantum_ticks = 0; 
            current_process_start = current_time;
            
            //printf("[Time %02d] Process %d loaded onto CPU.\n", current_time, active_cpu_process.pid);
        }
        for (int count = 0; count < ready_queue.size; count++) {
            
            int index = (ready_queue.front + count) % ready_queue.capacity; 
            
            ready_queue.array[index].waiting_time++;
        }
        for (int i = 0; i < io_queue.size; ) {
            io_queue.array[i].current_io_countdown--;

            if (io_queue.array[i].current_io_countdown == 0) {
                // Send back to standard Ready Queue
                enqueue(&ready_queue, io_queue.array[i]);
                //printf("[Time %02d] Process %d finished I/O and returned to Ready Queue.\n", 
                       //current_time, io_queue.array[i].pid);
                
                for (int j = i; j < io_queue.size - 1; j++) {
                    io_queue.array[j] = io_queue.array[j + 1];
                }
                io_queue.size--;
            } else {
                i++;
            }
        }

        // 5. EXECUTING THE CPU TICK
        if (is_cpu_busy) {
            active_cpu_process.remaining_time--;
            active_cpu_process.ticks_since_io++;
            current_quantum_ticks++;

            if (active_cpu_process.remaining_time == 0) {
                is_cpu_busy = 0;
                completed_processes++;
                record_gantt(&chart, active_cpu_process.pid, current_process_start, current_time + 1);
                active_cpu_process.current_io_countdown = active_cpu_process.io_burst_time;
                for (int k = 0; k < num_processes; k++) {
                    if (processes[k].pid == active_cpu_process.pid) {
                        processes[k].turnaround_time = (current_time + 1) - processes[k].arrival_time;
                        processes[k].waiting_time = active_cpu_process.waiting_time;
                    }
                }
                //printf("[Time %02d] Process %d COMPLETELY FINISHED.\n", current_time + 1, active_cpu_process.pid);
            }
            // Check I/O frequency
            // Note: If I/O frequency and Time Quantum hit on the exact same tick, 
            // the I/O takes priority here since it actually blocks the process.
            else if (active_cpu_process.ticks_since_io == active_cpu_process.io_frequency) {
                is_cpu_busy = 0;
                record_gantt(&chart, active_cpu_process.pid, current_process_start, current_time + 1);
                active_cpu_process.ticks_since_io = 0;
                active_cpu_process.current_io_countdown = active_cpu_process.io_burst_time;
                io_queue.array[io_queue.size] = active_cpu_process;
                io_queue.size++; 
                //printf("[Time %02d] Process %d hit I/O frequency. Evicted to I/O Queue.\n", 
                       //current_time + 1, active_cpu_process.pid);
            }
            else if (current_quantum_ticks == time_quantum) {
                if (!IsEmptyqueue(&ready_queue)){
                    is_cpu_busy = 0; 
                    //printf("[Time %02d] TIME QUANTUM EXPIRED! Process %d evicted to back of Ready Queue.\n", 
                       //current_time, active_cpu_process.pid);
                    record_gantt(&chart, active_cpu_process.pid, current_process_start, current_time + 1);
                    // Throw the running process to the back of the FIFO line
                    enqueue(&ready_queue, active_cpu_process);
                }
                else{
                    current_quantum_ticks = 0; 
                    //printf("[Time %02d] TIME QUANTUM EXPIRED! Process %d gets an extension (Ready Queue is empty).\n", 
                           //current_time + 1, active_cpu_process.pid);
                }
            }
        }
        current_time++;
        if (current_time > 2000) {
            printf("Error: Simulator exceeded maximum safe runtime execution limit.\n");
            break;
        }
    }
    
    if(!silent_mode){
    print_chart(&chart);
    printf("============================================\n");
    printf("   SIMULATION COMPLETE AT TIME %d\n", current_time);
    printf("============================================\n");
    }
    compute_metrics(processes, num_processes);
}