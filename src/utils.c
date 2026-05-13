#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "process.h"
#include "utils.h"

// Function to parse a single line or string chunk: "PID Arrival Burst"
void add_process(const char* pid, int arrival, int burst, int* num_processes, int MAX_PROCESSES, Process* processes) {
    if (*num_processes >= MAX_PROCESSES) {
        fprintf(stderr, "Warning: Maximum number of processes (%d) reached. Skipping %s.\n", MAX_PROCESSES, pid);
        return;
    }
    
    Process* p = &processes[*num_processes];
    strncpy(p->pid, pid, 15);
    p->pid[15] = '\0';
    p->arrival_time = arrival;
    p->burst_time = burst;
    p->remaining_time = burst;
    p->start_time = -1; // -1 indicates it hasn't started yet
    p->finish_time = 0;
    p->priority = 0;
    p->time_in_queue = 0;
    
    (*num_processes)++;
}

// Parse workload from text file
int load_from_file(const char* filename, int* num_processes, int MAX_PROCESSES, Process* processes) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening input file");
        return -1;
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // Skip leading whitespace
        char *ptr = line;
        while (isspace((unsigned char)*ptr)) ptr++;

        // Skip comments and empty lines
        if (*ptr == '#' || *ptr == '\0') continue;

        char pid[16];
        int arrival, burst;
        if (sscanf(ptr, "%15s %d %d", pid, &arrival, &burst) == 3) {
            // Validate arrival and burst times
            if (arrival < 0 || burst < 0) {
                fprintf(stderr, "Warning: Skipping process %s with negative arrival (%d) or burst (%d) time.\n", pid, arrival, burst);
                continue;
            }
            add_process(pid, arrival, burst, num_processes, MAX_PROCESSES, processes);
        }
    }
    fclose(file);
    return 0;
}

// Parse workload from command line string (e.g., "A:0:240,B:10:180")
void load_from_string(char* str, int* num_processes, int MAX_PROCESSES, Process* processes) {
    char* token = strtok(str, ",");
    while (token != NULL) {
        char pid[16];
        int arrival, burst;
        // Parse the token using ':' as a delimiter
        if (sscanf(token, "%15[^:]:%d:%d", pid, &arrival, &burst) == 3) {
            if (arrival < 0 || burst < 0) {
                fprintf(stderr, "Warning: Skipping process %s with negative arrival (%d) or burst (%d) time.\n", pid, arrival, burst);
            } else {
                add_process(pid, arrival, burst, num_processes, MAX_PROCESSES, processes);
            }
        }
        token = strtok(NULL, ",");
    }
}

int load_mlfq_config(const char *filename, MLFQConfig *config) {
    FILE *file = fopen(filename, "r");
    if (!file) {
        perror("Error opening MLFQ config file");
        return -1;
    }

    config->num_queues = 0;
    config->boost_period = 0;

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char *ptr = line;
        while (isspace((unsigned char)*ptr)) ptr++;
        if (*ptr == '#' || *ptr == '\0') continue;

        if (strncmp(ptr, "BOOST_PERIOD", 12) == 0) {
            sscanf(ptr + 12, "%d", &config->boost_period);
        } else if (*ptr == 'Q') {
            int qid, quantum, allotment;
            if (sscanf(ptr, "Q%d %d %d", &qid, &quantum, &allotment) == 3) {
                if (qid >= 0 && qid < MAX_QUEUES) {
                    config->quantums[qid] = quantum;
                    config->allotments[qid] = allotment;
                    if (qid + 1 > config->num_queues) {
                        config->num_queues = qid + 1;
                    }
                }
            }
        }
    }
    fclose(file);
    return 0;
}
