#ifndef PROCESS_H
#define PROCESS_H

typedef struct {
    char pid[16];           // Process identifier
    int arrival_time;       // When process arrives
    int burst_time;         // Total CPU time needed
    int remaining_time;     // For preemptive algorithms
    int start_time;         // When first executed (for RT)
    int finish_time;        // When completed (for TT)
    int waiting_time;       // Time spent waiting
    int priority;           // For MLFQ
    int time_in_queue;      // For MLFQ allotment tracking
} Process;

#define MAX_QUEUE_SIZE 100

typedef struct {
    Process *data[MAX_QUEUE_SIZE];
    int front;
    int rear;
    int size;
} ProcessQueue;

void init_queue(ProcessQueue *q);
int is_empty(ProcessQueue *q);
void enqueue(ProcessQueue *q, Process *p);
Process* dequeue(ProcessQueue *q);
int all_complete(Process *processes, int num_processes);

#endif // PROCESS_H