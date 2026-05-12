#include <string.h>
#include <stdio.h>
#include "process.h"
#include "utils.h"

// Function to parse a single line or string chunk: "PID Arrival Burst"
void add_process(const char* pid, int arrival, int burst, int* num_processes, int MAX_PROCESSES, Process* processes) {
    if (*num_processes >= MAX_PROCESSES) return;
    
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
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char *ptr = line;
        while (*ptr == ' ' || *ptr == '\t') ptr++;

        if (*ptr == '# || *ptr == '\n || *ptr == '\r || *ptr == '\0) continue;

        char pid[16];
        int arrival, burst;
        if (sscanf(line, "%15s %d %d", pid, &arrival, &burst) == 3) {
            if (arrival < 0 || burst < 0) {
                fprintf(stderr, "Warning: Skipping process %s with negative arrival/burst time\n", pid);
                continue;
            }
            add_process(pid, arrival, burst, num_processes, MAX_PROCESSES, processes);
        }
    }
}

// Parse workload from command line string (e.g., "A:0:240,B:10:180")
void load_from_string(char* str, int* num_processes, int MAX_PROCESSES, Process* processes) {
    char* token = strtok(str, ",");
    while (token != NULL) {
        char pid[16];
        int arrival, burst;
        // Parse the token using ':' as a delimiter
        if (sscanf(token, "%15[^:]:%d:%d", pid, &arrival, &burst) == 3) {
            add_process(pid, arrival, burst, num_processes, MAX_PROCESSES, processes);
        }
        token = strtok(NULL, ",");
    }
}