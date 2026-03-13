#include <stdio.h>
#include "process.h"

// Initialize an empty queue
void init_queue(ProcessQueue *q) {
    q->front = 0;
    q->rear = -1;
    q->size = 0;
}

// Check if the queue is empty
int is_empty(ProcessQueue *q) {
    return q->size == 0;
}

// Add a process pointer to the back of the queue (FIFO)
void enqueue(ProcessQueue *q, Process *p) {
    if (q->size >= MAX_QUEUE_SIZE) {
        fprintf(stderr, "Error: Queue overflow\n");
        return;
    }
    q->rear = (q->rear + 1) % MAX_QUEUE_SIZE;
    q->data[q->rear] = p;
    q->size++;
}

// Remove and return the process pointer at the front of the queue
Process* dequeue(ProcessQueue *q) {
    if (is_empty(q)) {
        return NULL;
    }
    Process *p = q->data[q->front];
    q->front = (q->front + 1) % MAX_QUEUE_SIZE;
    q->size--;
    return p;
}

// Helper to check if all processes are completely finished
int all_complete(Process *processes, int num_processes) {
    for (int i = 0; i < num_processes; i++) {
        if (processes[i].remaining_time > 0) {
            return 0; // At least one process still has work to do
        }
    }
    return 1; // All finished
}