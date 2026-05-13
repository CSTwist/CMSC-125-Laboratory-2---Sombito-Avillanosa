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
MLFQConfig mlfq_config;


Process processes[MAX_PROCESSES];
int num_processes = 0;


void run_comparison(SchedulerState *original_state) {
    const char* algos[] = {"FCFS", "SJF", "STCF", "RR", "MLFQ"};
    double avg_tt[5] = {0}, avg_wt[5] = {0}, avg_rt[5] = {0};
    
    for (int i = 0; i < 5; i++) {
        Process processes_copy[MAX_PROCESSES];
        memcpy(processes_copy, original_state->processes, sizeof(Process) * original_state->num_processes);
        
        SchedulerState state = {0};
        state.processes = processes_copy;
        state.num_processes = original_state->num_processes;
        state.current_time = 0;
        for (int j = 0; j < MAX_TIME; j++) state.gantt_chart.timestamps[j] = -1;
        
        if (i == 0) schedule_fcfs(&state);
        else if (i == 1) schedule_sjf(&state);
        else if (i == 2) schedule_stcf(&state);
        else if (i == 3) schedule_rr(&state, time_quantum);
        else if (i == 4) schedule_mlfq(&state, &mlfq_config);
        
        double total_tt = 0, total_wt = 0, total_rt = 0;
        for (int p = 0; p < state.num_processes; p++) {
            int tt = state.processes[p].finish_time - state.processes[p].arrival_time;
            int wt = tt - state.processes[p].burst_time;
            int rt = state.processes[p].start_time - state.processes[p].arrival_time;
            total_tt += tt;
            total_wt += wt;
            total_rt += rt;
        }
        avg_tt[i] = total_tt / state.num_processes;
        avg_wt[i] = total_wt / state.num_processes;
        avg_rt[i] = total_rt / state.num_processes;
    }
    
    printf("\n=== Comparison Summary ===\n");
    printf("%-10s | %-10s | %-10s | %-10s\n", "Algorithm", "Avg TT", "Avg WT", "Avg RT");
    printf("-----------|------------|------------|------------\n");
    for (int i = 0; i < 5; i++) {
        printf("%-10s | %10.2f | %10.2f | %10.2f\n", algos[i], avg_tt[i], avg_wt[i], avg_rt[i]);
    }
    printf("\n");
}

int main(int argc, char *argv[]) {

    static struct option long_options[] = {
        {"algorithm", required_argument, 0, 'a'},
        {"input",     required_argument, 0, 'i'},
        {"processes", required_argument, 0, 'p'},
        {"quantum",   required_argument, 0, 'q'},
        {"compare",   no_argument,       0, 'c'},
        {"mlfq-config", required_argument, 0, 'm'},
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
            
            case 'm': 
                strncpy(mlfq_config_file, optarg, 255); 
                mlfq_config_file[255] = '\0'; 
                break;
            case 'c': 
                compare_mode = 1; 
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

    // Set up the state object that we will pass to the schedulers
    SchedulerState state = {0};
    state.processes = processes;
    state.num_processes = num_processes;
    state.current_time = 0;

    for (int i = 0; i < MAX_TIME; i++) {
        state.gantt_chart.timestamps[i] = -1;
    }

    
    mlfq_config.num_queues = 3;
    mlfq_config.quantums[0] = 10;
    mlfq_config.allotments[0] = 50;
    mlfq_config.quantums[1] = 30;
    mlfq_config.allotments[1] = 100;
    mlfq_config.quantums[2] = -1;
    mlfq_config.allotments[2] = -1;
    mlfq_config.boost_period = 200;

    if (strlen(mlfq_config_file) > 0) {
        if (load_mlfq_config(mlfq_config_file, &mlfq_config) != 0) {
            fprintf(stderr, "Failed to load MLFQ config\n");
            return EXIT_FAILURE;
        }
    }


    
    // Route to the appropriate algorithm based on the CLI arguments
    if (compare_mode) {
        run_comparison(&state);
    } else {
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
    }

    return EXIT_SUCCESS;
}
