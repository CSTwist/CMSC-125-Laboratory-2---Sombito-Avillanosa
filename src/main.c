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

Process processes[MAX_PROCESSES];
int num_processes = 0;

int main(int argc, char *argv[]) {
    static struct option long_options[] = {
        {"algorithm", required_argument, 0, 'a'},
        {"input",     required_argument, 0, 'i'},
        {"processes", required_argument, 0, 'p'},
        {"quantum",   required_argument, 0, 'q'},
        {"compare",   no_argument,       0, 'c'},
        {0, 0, 0, 0}
    };

    int opt;
    int option_index = 0;

    while ((opt = getopt_long(argc, argv, "a:i:p:q:c", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'a': strncpy(algorithm, optarg, 31); break;
            case 'i': strncpy(input_file, optarg, 255); break;
            case 'p': strncpy(process_string, optarg, 511); break;
            case 'q': time_quantum = atoi(optarg); break;
            case 'c': compare_mode = 1; break;
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

    // Set up the state object that we will pass to the schedulers
    SchedulerState state = {0};
    state.processes = processes;
    state.num_processes = num_processes;
    state.current_time = 0;

    for (int i = 0; i < MAX_TIME; i++) {
        state.gantt_chart.timestamps[i] = -1;
    }

    // Hardcoded MLFQ configuration for testing
    // (Temporary config)
    MLFQConfig mlfq_config;
    mlfq_config.num_queues = 3;

    mlfq_config.quantums[0] = 2;    // Queue 0 (highest priority)
    mlfq_config.allotments[0] = 4;
    
    mlfq_config.quantums[1] = 4;    // Queue 1
    mlfq_config.allotments[1] = 8;
    
    mlfq_config.quantums[2] = -1;   // Queue 2 (lowest - FCFS)
    mlfq_config.allotments[2] = -1;

    mlfq_config.boost_period = 10;      // Boost period

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
