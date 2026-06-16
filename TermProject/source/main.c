#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "process.h"
#include "queue.h"
#include "pqueue.h"
#include "scheduler.h"

#define MAX_BUFFER 10
#define TIME_QUANTUM 4

typedef struct {
    char name[30];
    float avg_turnaround;
    float avg_waiting;
    float cpu_utilization;
    float throughput;
} AlgorithmStats;

int main() {
    srand(time(NULL)); // Initialize random seed ONCE
    Process original_dataset[MAX_BUFFER];
    Process temp_dataset[MAX_BUFFER];
    int created = 0;
    int num_processes = 0;
    int choice;

    while (1) {
        printf("\n--- OS SCHEDULER SIMULATOR MENU ---\n");
        printf("0. Exit\n");
        printf("1. Create Test Dataset\n");
        printf("2. Run FCFS\n");
        printf("3. Run SJF (Non-preemptive)\n");
        printf("4. Run Priority (Non-preemptive)\n");
        printf("5. Run SJF (Preemptive)\n");
        printf("6. Run Priority (Preemptive)\n");
        printf("7. Run Round Robin\n");
        printf("8. Run All\n");
        printf("Enter choice: ");
        scanf("%d", &choice);

        if (choice == 0) break;

        if (choice >= 2 && choice <= 8 && !created) {
            printf("Error: You must create a dataset first (Option 1)!\n");
            continue; 
        }

        switch (choice) {
            case 1:
                reset_pid_tracker();
                num_processes = (rand() % 6) + 4; // 4 to 9 processes
                for (int i = 0; i < num_processes; i++) {
                    create_process(&original_dataset[i]);
                }
                created = 1;
                printf("New dataset created with %d processes.\n", num_processes);
                printdataset(original_dataset, num_processes);
                break;

            case 2:
                memcpy(temp_dataset, original_dataset, sizeof(Process) * num_processes);
                simulate_FCFS(temp_dataset, num_processes, 0);
                break;

            case 3:
                memcpy(temp_dataset, original_dataset, sizeof(Process) * num_processes);
                simulate_SJF(temp_dataset, num_processes, 0);
                break;

            case 4:
                memcpy(temp_dataset, original_dataset, sizeof(Process) * num_processes);
                simulate_Priority(temp_dataset, num_processes, 0);
                break;

            case 5:
                memcpy(temp_dataset, original_dataset, sizeof(Process) * num_processes);
                simulate_SJF_preempt(temp_dataset, num_processes, 0); 
                break;
            
            case 6:
                memcpy(temp_dataset, original_dataset, sizeof(Process) * num_processes);
                simulate_Priority_preempt(temp_dataset, num_processes, 0); 
                break;

            case 7:
                memcpy(temp_dataset, original_dataset, sizeof(Process) * num_processes);
                simulate_RR(temp_dataset, num_processes, TIME_QUANTUM, 0); 
                break;

            case 8: {
                int num_runs;
                printf("How many random datasets do you want to test? ");
                scanf("%d", &num_runs);

                if (num_runs <= 0) {
                    printf("Invalid number of runs.\n");
                    break;
                }

                AlgorithmStats macro_results[6]; 
                
                // 1. Initialize names and set accumulators to 0
                strcpy(macro_results[0].name, "FCFS");
                strcpy(macro_results[1].name, "SJF (Non-Preempt)");
                strcpy(macro_results[2].name, "SRTF (Preempt)");
                strcpy(macro_results[3].name, "Priority");
                strcpy(macro_results[4].name, "Priority (Preempt)");
                strcpy(macro_results[5].name, "Round Robin (TQ=4)");

                for(int i = 0; i < 6; i++) {
                    macro_results[i].avg_turnaround = 0;
                    macro_results[i].avg_waiting = 0;
                    macro_results[i].cpu_utilization = 0;
                    macro_results[i].throughput = 0;
                }

                printf("\n>>> RUNNING MACRO SIMULATION ON %d DATASETS <<<\n", num_runs);

                // 2. The Grand Outer Loop (Creates new datasets)
                for (int run = 0; run < num_runs; run++) {
                    
                    // Generate a totally fresh dataset for this specific run
                    reset_pid_tracker();
                    num_processes = (rand() % 6) + 4; 
                    for (int i = 0; i < num_processes; i++) {
                        create_process(&original_dataset[i]);
                    }

                    // 3. The Inner Loop (Tests all 6 algorithms on this dataset)
                    for (int algo = 0; algo < 6; algo++) {
                        memcpy(temp_dataset, original_dataset, sizeof(Process) * num_processes);

                        switch(algo) {
                            case 0: simulate_FCFS(temp_dataset, num_processes, 1); break;
                            case 1: simulate_SJF(temp_dataset, num_processes, 1); break;
                            case 2: simulate_SJF_preempt(temp_dataset, num_processes, 1); break;
                            case 3: simulate_Priority(temp_dataset, num_processes, 1); break;
                            case 4: simulate_Priority_preempt(temp_dataset, num_processes, 1); break;
                            case 5: simulate_RR(temp_dataset, num_processes, TIME_QUANTUM, 1); break;
                        }
            
                        float temp_tat, temp_wait, temp_util, temp_through;

                        // Call the new shared function!
                        calculate_core_metrics(temp_dataset, num_processes, &temp_tat, &temp_wait, &temp_util, &temp_through);

                        // Accumulate for your massive dataset averages...
                        macro_results[algo].avg_turnaround += temp_tat;
                        macro_results[algo].avg_waiting += temp_wait;
                        macro_results[algo].cpu_utilization += temp_util;
                        macro_results[algo].throughput += temp_through;
                    }
                }

                // 4. Print the final professional comparison table
                printf("\n==========================================================\n");
                printf("   MACRO COMPARISON TABLE (AVERAGED OVER %d DATASETS)\n", num_runs);
                printf("==========================================================\n");
                printf("%-20s | %-15s | %-15s | %-15s | %-15s\n", "Algorithm", "Macro Avg TAT", "Macro Avg Wait", "Macro CPU Util", "Macro Throughput");
                printf("----------------------------------------------------------\n");
                for(int i = 0; i < 6; i++) {
                    // Divide the accumulated totals by num_runs to get the true average
                    printf("%-20s | %-15.2f | %-15.2f | %-15.5f | %15.5f\n", 
                        macro_results[i].name, 
                        macro_results[i].avg_turnaround / num_runs, 
                        macro_results[i].avg_waiting / num_runs,
                        macro_results[i].cpu_utilization / num_runs,
                        macro_results[i].throughput / num_runs);
                }
                break;
            } // End of case 8 scope
                
            default:
                printf("Invalid choice.\n");
        }
    }
    return 0;
}