#include <stdio.h>
#include "scheduler.h"
#include "process.h"
#include "metrics.h"
#include "gantt.h"

int schedule_mlfq(SchedulerState *state, MLFQConfig *config) {
    ProcessQueue queues[MAX_QUEUES];
    for (int i = 0; i < config->num_queues; i++) {
        init_queue(&queues[i]);
    }

    for (int i = 0; i < state->num_processes; i++) {
        state->processes[i].priority = 0;
        state->processes[i].time_in_queue = 0;
    }

    Process *current_process = NULL;
    GanttChart *mlfq_gantt_chart = &state->gantt_chart;
    mlfq_gantt_chart->size = 0;

    int simulation_time = 0;
    int quantum_used = 0;

    printf("\nRunning MLFQ Scheduler...\n\n");

    while (!all_complete(state->processes, state->num_processes)) {
        if (simulation_time >= MAX_TIME) {
            fprintf(stderr, "Error: Gantt chart exceeded MAX_TIME (%d)\n", MAX_TIME);
            return -1;
        }

        // 1. Handle arrivals: all new jobs enter highest-priority queue
        for (int i = 0; i < state->num_processes; i++) {
            if (state->processes[i].arrival_time == simulation_time) {
                state->processes[i].priority = 0;
                state->processes[i].time_in_queue = 0;
                enqueue(&queues[0], &state->processes[i]);

                // // Debug trace print
                // printf("t=%d: %s arrives -> Q0\n", 
                //     simulation_time, 
                //     state->processes[i].pid);
            }
            
            
        }


        // 2. Priority boost
        if (config->boost_period > 0 &&
            simulation_time > 0 &&
            simulation_time % config->boost_period == 0) {

            // // Debug trace print
            // printf("\n=== BOOST at t=%d ===\n", simulation_time);

            // Move all queued processes back to Q0
            for (int level = 1; level < config->num_queues; level++) {
                while (!is_empty(&queues[level])) {
                    Process *p = dequeue(&queues[level]);

                    // // Debug trace print
                    // printf("t=%d: %s boosted to Q0\n", simulation_time, p->pid);

                    p->priority = 0;
                    p->time_in_queue = 0;
                    enqueue(&queues[0], p);
                }
            }

            // Also boost current running process
            if (current_process != NULL) {
                
                // // Debug trace print
                // printf("t=%d: %s boosted (was running)\n",
                //     simulation_time,
                //     current_process->pid);
                
                current_process->priority = 0;
                current_process->time_in_queue = 0;
                enqueue(&queues[0], current_process);
                
                current_process = NULL;
                quantum_used = 0;
            }
        }

        // 3. If CPU idle, pick from highest-priority non-empty queue
        if (current_process == NULL) {
            for (int level = 0; level < config->num_queues; level++) {
                if (!is_empty(&queues[level])) {
                    current_process = dequeue(&queues[level]);
                    quantum_used = 0;

                    if (current_process->start_time == -1) {
                        current_process->start_time = simulation_time;
                    }
                    
                    // // Debug trace print
                    // printf("t=%d: %s scheduled from Q%d\n",
                    //     simulation_time,
                    //     current_process->pid,
                    //     current_process->priority);

                    break;
                }
            }
        }

        // 4. Execute one tick
        if (current_process != NULL) {

            // // Debug trace print
            // printf("t=%d: running %s (Q%d)\n",
            //     simulation_time,
            //     current_process->pid,
            //     current_process->priority);

            mlfq_gantt_chart->process_order[simulation_time] = current_process;

            current_process->remaining_time--;
            current_process->time_in_queue++;
            quantum_used++;

            int level = current_process->priority;
            int quantum = config->quantums[level];
            int allotment = config->allotments[level];

            // Process finished
            if (current_process->remaining_time == 0) {
                current_process->finish_time = simulation_time + 1;
                // // Debug trace print
                // printf("t=%d: %s finished\n", simulation_time + 1, current_process->pid);

                current_process = NULL;
                quantum_used = 0;
            }
            // Lowest queue FCFS behavior (quantum = -1)
            else if (quantum == -1) {
                // keep running unless finished or boosted later
            }
            // Quantum expired
            else if (quantum_used == quantum) {
                // Check if process should be demoted
                if (allotment != -1 &&
                    current_process->time_in_queue >= allotment &&
                    current_process->priority < config->num_queues - 1) {

                    current_process->priority++;
                    current_process->time_in_queue = 0;
                    // // Debug trace print
                    // printf("t=%d: %s DEMOTED to Q%d\n",
                    //     simulation_time + 1,
                    //     current_process->pid,
                    //     current_process->priority);
                }

                // // Debug trace print
                // printf("t=%d: %s requeued in Q%d\n",
                //     simulation_time + 1,
                //     current_process->pid,
                //     current_process->priority);

                enqueue(&queues[current_process->priority], current_process);
                current_process = NULL;
                quantum_used = 0;
            }
        } else {
            mlfq_gantt_chart->process_order[simulation_time] = NULL;
        }

        mlfq_gantt_chart->size = simulation_time + 1;
        simulation_time++;
        state->current_time = simulation_time;
    }

    print_ganttChart(&state->gantt_chart);
    print_metrics(state);

    return 0;
}