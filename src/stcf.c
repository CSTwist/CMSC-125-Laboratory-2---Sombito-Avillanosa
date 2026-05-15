#include <stdio.h>
#include "scheduler.h"
#include "process.h"
#include "metrics.h"
#include "gantt.h"

int schedule_stcf(SchedulerState *state) {
    ProcessQueue ready_queue;
    init_queue(&ready_queue);
    
    Process *current_process = NULL;
    GanttChart *stcf_gantt_chart = &state->gantt_chart;
    stcf_gantt_chart->size = 0;

    int t = 0; 

    printf("\nRunning STCF Scheduler...\n\n");

    while (!all_complete(state->processes, state->num_processes)) {

        if (t >= MAX_TIME) {
            fprintf(stderr, "Error: Gantt chart exceeded MAX_TIME (%d)\n", MAX_TIME);
            return -1;
        }
        
        // Check for new arrivals at time 't'
        for (int i = 0; i < state->num_processes; i++) {
            if (state->processes[i].arrival_time == t) {
                enqueue(&ready_queue, &state->processes[i]);
            }
        }

        // Check for preemption
        // If a process is running, check whether a ready process has shorter remaining time
        if (current_process != NULL && !is_empty(&ready_queue)) {
            int min_rem = current_process->remaining_time;
            int should_preempt = 0;
            
            for (int i = 0; i < ready_queue.size; i++) {
                int idx = (ready_queue.front + i) % MAX_QUEUE_SIZE;

                if (ready_queue.data[idx]->remaining_time < min_rem) {
                    min_rem = ready_queue.data[idx]->remaining_time;
                    should_preempt = 1;
                }
            }
            
            if (should_preempt) {
                enqueue(&ready_queue, current_process);
                current_process = NULL;
            }
        }

        // Select process with shortest remaining time
        if (current_process == NULL && !is_empty(&ready_queue)) {
            current_process = dequeue_shortest_remaining(&ready_queue);
            
            if (current_process->start_time == -1) {
                current_process->start_time = t;
            }
        }

        // Execute for 1 time unit and record Gantt chart data
        if (current_process != NULL) {
            stcf_gantt_chart->process_order[t] = current_process;

            current_process->remaining_time--;

            if (current_process->remaining_time == 0) {
                current_process->finish_time = t + 1;
                current_process = NULL; 
            }
        } else {
            stcf_gantt_chart->process_order[t] = NULL;
        }

        stcf_gantt_chart->size = t + 1;
        t++;
        state->current_time = t;
    }
    
    print_ganttChart(&state->gantt_chart);
    print_metrics(state);
    
    return 0;
}