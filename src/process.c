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

// Remove and return the process with the shortest burst time
Process* dequeue_shortest(ProcessQueue *q) {
    if (is_empty(q)) return NULL;

    int min_index = q->front;
    int min_burst = q->data[min_index]->burst_time;

    // Find the index of the process with the shortest burst time
    for (int i = 0; i < q->size; i++) {
        int curr_index = (q->front + i) % MAX_QUEUE_SIZE;
        // If there's a tie, the one that arrived earlier (closer to front) wins implicitly
        if (q->data[curr_index]->burst_time < min_burst) {
            min_burst = q->data[curr_index]->burst_time;
            min_index = curr_index;
        }
    }

    // Extract the shortest process
    Process *shortest = q->data[min_index];

    // Shift elements down to fill the gap without breaking the circular buffer
    int curr = min_index;
    while (curr != q->front) {
        int prev = (curr - 1 + MAX_QUEUE_SIZE) % MAX_QUEUE_SIZE;
        q->data[curr] = q->data[prev];
        curr = prev;
    }

    // Move the front pointer forward and decrease size
    q->front = (q->front + 1) % MAX_QUEUE_SIZE;
    q->size--;

    return shortest;
}

// Remove and return the process with the shortest remaining time
Process* dequeue_shortest_remaining(ProcessQueue *q) {
    if (is_empty(q)) return NULL;

    int min_index = q->front;
    int min_rem = q->data[min_index]->remaining_time;

    // Find the index of the process with the shortest remaining time
    for (int i = 0; i < q->size; i++) {
        int curr_index = (q->front + i) % MAX_QUEUE_SIZE;
        if (q->data[curr_index]->remaining_time < min_rem) {
            min_rem = q->data[curr_index]->remaining_time;
            min_index = curr_index;
        }
    }

    Process *shortest = q->data[min_index];

    // Shift elements down to fill the gap
    int curr = min_index;
    while (curr != q->front) {
        int prev = (curr - 1 + MAX_QUEUE_SIZE) % MAX_QUEUE_SIZE;
        q->data[curr] = q->data[prev];
        curr = prev;
    }

    q->front = (q->front + 1) % MAX_QUEUE_SIZE;
    q->size--;

    return shortest;
}