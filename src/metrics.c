#include <stdio.h>
#include "scheduler.h"
#include "process.h"
#include "metrics.h"

void print_metrics(SchedulerState *state){
    double total_tt = 0;
    double total_wt = 0;
    double total_rt = 0;

    printf("=== Metrics ===\n");
    printf("%-7s | %-3s | %-3s | %-3s | %-3s | %-3s | %-3s\n", 
           "Process", "AT", "BT", "FT", "TT", "WT", "RT");
    printf("--------|-----|-----|-----|-----|-----|-----\n");

    for (int i = 0; i < state->num_processes; i++) {
        Process *p = &state->processes[i];

        // Calculate the metrics
        int tt = p->finish_time - p->arrival_time;
        p->waiting_time = tt - p->burst_time; // Stored back in your struct
        int rt = p->start_time - p->arrival_time;

        // Add to running totals for average calculation
        total_tt += tt;
        total_wt += p->waiting_time;
        total_rt += rt;

        // Print the row for this process
        printf("%-7s | %3d | %3d | %3d | %3d | %3d | %3d\n", 
               p->pid, 
               p->arrival_time, 
               p->burst_time, 
               p->finish_time, 
               tt, 
               p->waiting_time, 
               rt);
    }

    printf("--------|-----|-----|-----|-----|-----|-----\n");

    // Calculate and print averages
    double avg_tt = total_tt / state->num_processes;
    double avg_wt = total_wt / state->num_processes;
    double avg_rt = total_rt / state->num_processes;

    printf("%-7s |     |     |     | %3.1f | %3.1f | %3.1f\n\n", 
           "Average", avg_tt, avg_wt, avg_rt);
}