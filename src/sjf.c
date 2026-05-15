#include <stdio.h>
#include "scheduler.h"
#include "process.h"
#include "metrics.h"
#include "gantt.h"

int schedule_sjf(SchedulerState *state) {
    ProcessQueue ready_queue;
    init_queue(&ready_queue);
    
    Process *current_process = NULL;
    GanttChart *fcfs_gantt_chart = &state->gantt_chart;
    fcfs_gantt_chart->size = 0;
    int simulation_time = 0; // Current simulation time

    printf("\nRunning SJF Scheduler...\n\n");

    // Core discrete-event simulation loop
    while (!all_complete(state->processes, state->num_processes)) {

        if (simulation_time >= MAX_TIME) {
            fprintf(stderr, "Error: Gantt chart exceeded MAX_TIME (%d)\n", MAX_TIME);
            return -1;
        }
        
        // Check for new arrivals at exactly time 'simulation_time'
        for (int i = 0; i < state->num_processes; i++) {
            if (state->processes[i].arrival_time == simulation_time) {
                enqueue(&ready_queue, &state->processes[i]);
            }
        }

        // If CPU is idle, select the next process from the queue
        if (current_process == NULL && !is_empty(&ready_queue)) {
            current_process = dequeue_shortest(&ready_queue);
            
            // If this is the very first time it runs, record start_time
            if (current_process->start_time == -1) {
                current_process->start_time = simulation_time;
            }
        }

        // Execute the running process for 1 time unit
        if (current_process != NULL) {
            // Saves the process ID character to the Gantt chart
            fcfs_gantt_chart->process_order[simulation_time] = current_process;
            
            current_process->remaining_time--;

            // Check if the process just finished
            if (current_process->remaining_time == 0) {
                current_process->finish_time = simulation_time + 1; // It finishes at the end of this tick
                current_process = NULL; // Evict from CPU so next process can run
            }
        } else {
            fcfs_gantt_chart->process_order[simulation_time] = NULL;
        }
        
        fcfs_gantt_chart->size = simulation_time + 1;
        simulation_time++; // Advance time
        state->current_time = simulation_time;
    }
    
    print_ganttChart(&state->gantt_chart);
    print_metrics(state);
    
    return 0; // Success
}