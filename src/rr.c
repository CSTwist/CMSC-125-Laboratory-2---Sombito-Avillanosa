#include <stdio.h>
#include "scheduler.h"
#include "process.h"
#include "metrics.h"
#include "gantt.h"

int schedule_rr(SchedulerState *state, int quantum) {
    if (quantum <= 0) {
        fprintf(stderr, "Error: Round Robin quantum must be > 0\n");
        return -1;
    }

    ProcessQueue ready_queue;
    init_queue(&ready_queue);

    Process *current_process = NULL;
    GanttChart *rr_gantt_chart = &state->gantt_chart;
    rr_gantt_chart->size = 0;

    int simulation_time = 0;
    int quantum_used = 0;

    printf("\nRunning RR Scheduler...\n");
    printf("Using time quantum q=%d\n\n", quantum);


    Process *preempted_process = NULL;

    while (!all_complete(state->processes, state->num_processes)) {

        if (simulation_time >= MAX_TIME) {
            fprintf(stderr, "Error: Gantt chart exceeded MAX_TIME (%d)\n", MAX_TIME);
            return -1;
        }

        // 1. Check for new arrivals at exactly time 'simulation_time'
        for (int i = 0; i < state->num_processes; i++) {
            if (state->processes[i].arrival_time == simulation_time) {
                enqueue(&ready_queue, &state->processes[i]);
            }
        }

        if (preempted_process != NULL) {
            enqueue(&ready_queue, preempted_process);
            preempted_process = NULL;
        }


        // 2. If CPU is idle, pick next process from ready queue
        if (current_process == NULL && !is_empty(&ready_queue)) {
            current_process = dequeue(&ready_queue);
            quantum_used = 0;

            // Record first start time for response time
            if (current_process->start_time == -1) {
                current_process->start_time = simulation_time;
            }
        }

        // 3. Execute current process for 1 time unit
        if (current_process != NULL) {
            rr_gantt_chart->process_order[simulation_time] = current_process;

            current_process->remaining_time--;
            quantum_used++;

            // 4a. If process finished, record finish time
            if (current_process->remaining_time == 0) {
                current_process->finish_time = simulation_time + 1;
                current_process = NULL;
                quantum_used = 0;
            }

            // 4b. If quantum expired and process not finished, preempt and requeue
            else if (quantum_used == quantum) {
                preempted_process = current_process;
                current_process = NULL;
                quantum_used = 0;
            }

        } else {
            rr_gantt_chart->process_order[simulation_time] = NULL;
        }

        rr_gantt_chart->size = simulation_time + 1;
        simulation_time++;
        state->current_time = simulation_time;
    }

    print_ganttChart(&state->gantt_chart);
    print_metrics(state);

    return 0;
}