#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "process.h"
#define MAX_QUEUES 10
#define MAX_TIME 10000

typedef struct {
    int level;              // Queue priority level (0 = highest)
    int time_quantum;       // Time slice for this queue (-1 for FCFS)
    int allotment;          // Max time before demotion (-1 for infinite)
    Process *queue;         // Array or linked list of processes
    int size;               // Current queue size
} MLFQQueue;

typedef struct {
    MLFQQueue *queues;      // Array of queues
    int num_queues;         // Number of priority levels
    int boost_period;       // Period for priority boost (S)
    int last_boost;         // Last boost time
} MLFQScheduler;

typedef enum {
    EVENT_ARRIVAL,
    EVENT_COMPLETION,
    EVENT_QUANTUM_EXPIRE,
    EVENT_PRIORITY_BOOST
} EventType;

typedef struct Event {
    int time;
    EventType type;
    Process *process;
    struct Event *next;
} Event;

typedef struct {
    Process *process_order[MAX_TIME];
    int size;
} GanttChart;

typedef struct {
    Process *processes;     // Array of all processes
    int num_processes;      // Number of processes
    int current_time;       // Current simulation time
    GanttChart gantt_chart;
    // ... additional fields for metrics, Gantt chart, etc.
    // Recall: CMSC 141
} SchedulerState;

typedef enum {
    FCFS,
    SJF,
    STCF,
    RR,
    MLFQ
} SchedulingAlgorithm;

typedef struct {
    int num_queues;
    int quantums[MAX_QUEUES];
    int allotments[MAX_QUEUES];
    int boost_period;

} MLFQConfig;

void simulate_scheduler(SchedulerState *state, SchedulingAlgorithm algorithm);

// Return 0 on success, -1 on error (command line etiquette)
int schedule_fcfs(SchedulerState *state);
int schedule_sjf(SchedulerState *state);
int schedule_stcf(SchedulerState *state);
int schedule_rr(SchedulerState *state, int quantum);
int schedule_mlfq(SchedulerState *state, MLFQConfig *config);

#endif //SCHEDULER_H