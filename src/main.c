#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include "process.h"
#include "scheduler.h"
#include "utils.h"

#define MAX_PROCESSES 100

// Global configuration variables
char algorithm[32] = "";
char input_file[256] = "";
char process_string[512] = "";
int time_quantum = 30; // Default qt
int compare_mode = 0;
char mlfq_config_file[256] = "";

Process processes[MAX_PROCESSES];
int num_processes = 0;

// Comparison helper functions
typedef struct {
    char name[16];
    double avg_tt;
    double avg_wt;
    double avg_rt;
    int success;
} CompareResult;

void reset_processes(Process *dest, Process *src, int n) {
    for (int i = 0; i < n; i++) {
        dest[i] = src[i];

        // Reset runtime fields before each scheduler run
        dest[i].remaining_time = dest[i].burst_time;
        dest[i].start_time = -1;
        dest[i].finish_time = -1;
        dest[i].waiting_time = 0;
        dest[i].priority = 0;
        dest[i].time_in_queue = 0;
    }
}

void init_scheduler_state(SchedulerState *state, Process *processes, int n) {
    *state = (SchedulerState){0};

    state->processes = processes;
    state->num_processes = n;
    state->current_time = 0;

    for (int i = 0; i < MAX_TIME; i++) {
        state->gantt_chart.timestamps[i] = -1;
        state->gantt_chart.process_order[i] = NULL;
    }
}

void compute_average_metrics(SchedulerState *state, CompareResult *result) {
    double total_tt = 0;
    double total_wt = 0;
    double total_rt = 0;

    for (int i = 0; i < state->num_processes; i++) {
        Process *p = &state->processes[i];

        int tt = p->finish_time - p->arrival_time;
        int wt = tt - p->burst_time;
        int rt = p->start_time - p->arrival_time;

        total_tt += tt;
        total_wt += wt;
        total_rt += rt;
    }

    result->avg_tt = total_tt / state->num_processes;
    result->avg_wt = total_wt / state->num_processes;
    result->avg_rt = total_rt / state->num_processes;
}

// runs all algos and prints a comparison table
void run_comparison(Process *original_processes,
                    int num_processes,
                    int rr_quantum,
                    MLFQConfig *mlfq_config) {
    CompareResult results[5] = {
        {"FCFS", 0, 0, 0, 0},
        {"SJF",  0, 0, 0, 0},
        {"STCF", 0, 0, 0, 0},
        {"RR",   0, 0, 0, 0},
        {"MLFQ", 0, 0, 0, 0}
    };

    Process temp_processes[MAX_PROCESSES];
    SchedulerState state;

    printf("\n=== Running Algorithm Comparison ===\n");

    // FCFS
    reset_processes(temp_processes, original_processes, num_processes);
    init_scheduler_state(&state, temp_processes, num_processes);
    results[0].success = (schedule_fcfs(&state) == 0);
    compute_average_metrics(&state, &results[0]);

    // SJF
    reset_processes(temp_processes, original_processes, num_processes);
    init_scheduler_state(&state, temp_processes, num_processes);
    results[1].success = (schedule_sjf(&state) == 0);
    compute_average_metrics(&state, &results[1]);

    // STCF
    reset_processes(temp_processes, original_processes, num_processes);
    init_scheduler_state(&state, temp_processes, num_processes);
    results[2].success = (schedule_stcf(&state) == 0);
    compute_average_metrics(&state, &results[2]);

    // RR
    reset_processes(temp_processes, original_processes, num_processes);
    init_scheduler_state(&state, temp_processes, num_processes);
    results[3].success = (schedule_rr(&state, rr_quantum) == 0);
    compute_average_metrics(&state, &results[3]);

    // MLFQ
    reset_processes(temp_processes, original_processes, num_processes);
    init_scheduler_state(&state, temp_processes, num_processes);
    results[4].success = (schedule_mlfq(&state, mlfq_config) == 0);
    compute_average_metrics(&state, &results[4]);

    printf("\n=== Algorithm Comparison ===\n");
    printf("%-10s | %-8s | %-8s | %-8s\n",
           "Algorithm", "Avg TT", "Avg WT", "Avg RT");
    printf("-----------|----------|----------|----------\n");

    for (int i = 0; i < 5; i++) {
        if (results[i].success) {
            printf("%-10s | %8.1f | %8.1f | %8.1f\n",
                   results[i].name,
                   results[i].avg_tt,
                   results[i].avg_wt,
                   results[i].avg_rt);
        } else {
            printf("%-10s | %8s | %8s | %8s\n",
                   results[i].name,
                   "ERROR",
                   "ERROR",
                   "ERROR");
        }
    }
}


int main(int argc, char *argv[]) {
    static struct option long_options[] = {
        {"algorithm", required_argument, 0, 'a'},
        {"input",     required_argument, 0, 'i'},
        {"processes", required_argument, 0, 'p'},
        {"quantum",   required_argument, 0, 'q'},
        {"compare",   no_argument,       0, 'c'},
        {"mlfq-config",  required_argument, 0, 'm'},
        {0, 0, 0, 0}
    };

    int opt;
    int option_index = 0;

    while ((opt = getopt_long(argc, argv, "a:i:p:q:cm:", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'a': 
                strncpy(algorithm, optarg, 31); 
                algorithm[31] = '\0'; 
                break;
            case 'i': 
                strncpy(input_file, optarg, 255); 
                input_file[255] = '\0'; 
                break;
            case 'p': 
                strncpy(process_string, optarg, 511); 
                process_string[511] = '\0'; 
                break;
            case 'q': 
                time_quantum = atoi(optarg); 
                if (time_quantum <= 0) {
                    fprintf(stderr, "Error: Quantum must be positive\n");
                    exit(EXIT_FAILURE);
                }
                break;
            case 'c': 
                compare_mode = 1; 
                break;
            case 'm':
                strncpy(mlfq_config_file, optarg, sizeof(mlfq_config_file) - 1);
                mlfq_config_file[sizeof(mlfq_config_file) - 1] = '\0';
                break;
            default:
                fprintf(stderr, "Usage: %s --algorithm=<alg> [--input=<file> | --processes=<str>]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    if (strlen(input_file) > 0) {
        if (load_from_file(input_file, &num_processes, MAX_PROCESSES, processes) != 0) return EXIT_FAILURE;
    } else if (strlen(process_string) > 0) {
        load_from_string(process_string, &num_processes, MAX_PROCESSES, processes);
    } else {
        fprintf(stderr, "Error: Must provide workload via --input or --processes\n");
        return EXIT_FAILURE;
    }

    // Save original processes for comparison mode
    Process original_processes[MAX_PROCESSES];
    reset_processes(original_processes, processes, num_processes);


    // Set up the state object that we will pass to the schedulers
    SchedulerState state = {0};
    state.processes = processes;
    state.num_processes = num_processes;
    state.current_time = 0;

    for (int i = 0; i < MAX_TIME; i++) {
        state.gantt_chart.timestamps[i] = -1;
    }

    // Hardcoded MLFQ configuration
    // (This is currently the default fallback)
    MLFQConfig mlfq_config;
    mlfq_config.num_queues = 3;

    mlfq_config.quantums[0] = 2;    // Queue 0 (highest priority)
    mlfq_config.allotments[0] = 4;
    
    mlfq_config.quantums[1] = 4;    // Queue 1
    mlfq_config.allotments[1] = 8;
    
    mlfq_config.quantums[2] = -1;   // Queue 2 (lowest - FCFS)
    mlfq_config.allotments[2] = -1;

    mlfq_config.boost_period = 10;      // Boost period

    if (strlen(mlfq_config_file) > 0) {
        if (load_mlfq_config(mlfq_config_file, &mlfq_config) != 0) {
            return EXIT_FAILURE;
        }
    }

    // If in compare mode, run all algos & print results
    if (compare_mode) {
        run_comparison(original_processes, num_processes, time_quantum, &mlfq_config);
        return EXIT_SUCCESS;
    }


    // Route to the appropriate algorithm based on the CLI arguments
    if (strcmp(algorithm, "FCFS") == 0) {
        schedule_fcfs(&state);
    } else if (strcmp(algorithm, "SJF") == 0) {
        schedule_sjf(&state);
    } else if (strcmp(algorithm, "STCF") == 0) {
        schedule_stcf(&state);
    } else if (strcmp(algorithm, "RR") == 0) {
        schedule_rr(&state, time_quantum);
    } else if (strcmp(algorithm, "MLFQ") == 0) {
        schedule_mlfq(&state, &mlfq_config);
    } else {
        fprintf(stderr, "Error: Unknown algorithm '%s'\n", algorithm);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
