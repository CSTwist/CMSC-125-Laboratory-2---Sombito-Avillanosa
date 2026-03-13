#include <stdio.h>
#include "scheduler.h"
#include "process.h"
#include "metrics.h"

// External declarations for our queue functions
void init_queue(ProcessQueue *q);
int is_empty(ProcessQueue *q);
void enqueue(ProcessQueue *q, Process *p);
Process* dequeue_shortest_remaining(ProcessQueue *q);
int all_complete(Process *processes, int num_processes);

int schedule_stcf(SchedulerState *state) {
    ProcessQueue ready_queue;
    init_queue(&ready_queue);
    
    Process *current_process = NULL;
    int t = 0; 

    printf("\nRunning STCF Scheduler...\n\n");
    printf("=== Gantt Chart ===\n[");

    while (!all_complete(state->processes, state->num_processes)) {
        
        // 1. Check for new arrivals at time 't'
        for (int i = 0; i < state->num_processes; i++) {
            if (state->processes[i].arrival_time == t) {
                enqueue(&ready_queue, &state->processes[i]);
            }
        }

        // 2. Preemption Logic!
        // If a process is running, check if we need to preempt it
        if (current_process != NULL && !is_empty(&ready_queue)) {
            // Peek into the queue to find the absolute shortest remaining time
            int min_rem = current_process->remaining_time;
            int should_preempt = 0;
            
            for (int i = 0; i < ready_queue.size; i++) {
                int idx = (ready_queue.front + i) % MAX_QUEUE_SIZE;
                if (ready_queue.data[idx]->remaining_time < min_rem) {
                    min_rem = ready_queue.data[idx]->remaining_time;
                    should_preempt = 1;
                }
            }
            
            // If we found a shorter job, kick the current process back to the queue
            if (should_preempt) {
                enqueue(&ready_queue, current_process);
                current_process = NULL; // CPU is now "idle" so the next block grabs the new shortest
            }
        }

        // 3. Select the process with the shortest remaining time
        if (current_process == NULL && !is_empty(&ready_queue)) {
            current_process = dequeue_shortest_remaining(&ready_queue);
            
            if (current_process->start_time == -1) {
                current_process->start_time = t;
            }
        }

        // 4. Execute for 1 time unit
        if (current_process != NULL) {
            printf("%s", current_process->pid);
            current_process->remaining_time--;

            if (current_process->remaining_time == 0) {
                current_process->finish_time = t + 1;
                current_process = NULL; 
            }
        } else {
            printf("-");
        }

        t++;
        state->current_time = t;
    }
    
    printf("]\nTime: %d\n\n", t);
    
    print_metrics(state);
    
    return 0;
}