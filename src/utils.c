#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "process.h"
#include "utils.h"
#include "scheduler.h"

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
    if (file == NULL) {
        perror("open MLFQ config");
        return -1;
    }

    // Initialize config to known values
    config->num_queues = 0;
    config->boost_period = -1;

    for (int i = 0; i < MAX_QUEUES; i++) {
        config->quantums[i] = 0;
        config->allotments[i] = 0;
    }

    char line[256];

    while (fgets(line, sizeof(line), file) != NULL) {
        // Skip leading whitespace
        char *p = line;
        while (isspace((unsigned char)*p)) {
            p++;
        }

        // Skip blank lines and comments
        if (*p == '\0' || *p == '\n' || *p == '#') {
            continue;
        }

        // Parse priority queue lines: Q0 10 50
        if (*p == 'Q') {
            int queue_id;
            int quantum;
            int allotment;

            if (sscanf(p, "Q%d %d %d", &queue_id, &quantum, &allotment) != 3) {
                fprintf(stderr, "Error: Invalid MLFQ queue config line: %s", line);
                fclose(file);
                return -1;
            }

            if (queue_id < 0 || queue_id >= MAX_QUEUES) {
                fprintf(stderr, "Error: Invalid MLFQ queue id Q%d\n", queue_id);
                fclose(file);
                return -1;
            }

            if (quantum == 0 || quantum < -1) {
                fprintf(stderr, "Error: Invalid quantum for Q%d: %d\n", queue_id, quantum);
                fclose(file);
                return -1;
            }

            if (allotment == 0 || allotment < -1) {
                fprintf(stderr, "Error: Invalid allotment for Q%d: %d\n", queue_id, allotment);
                fclose(file);
                return -1;
            }

            config->quantums[queue_id] = quantum;
            config->allotments[queue_id] = allotment;

            if (queue_id + 1 > config->num_queues) {
                config->num_queues = queue_id + 1;
            }
        }
        // Parse boost line: BOOST_PERIOD 200
        else if (strncmp(p, "BOOST_PERIOD", 12) == 0) {
            int boost_period;

            if (sscanf(p, "BOOST_PERIOD %d", &boost_period) != 1) {
                fprintf(stderr, "Error: Invalid BOOST_PERIOD line: %s", line);
                fclose(file);
                return -1;
            }

            if (boost_period <= 0) {
                fprintf(stderr, "Error: BOOST_PERIOD must be > 0\n");
                fclose(file);
                return -1;
            }

            config->boost_period = boost_period;
        }
        else {
            fprintf(stderr, "Error: Unknown MLFQ config line: %s", line);
            fclose(file);
            return -1;
        }
    }

    fclose(file);

    if (config->num_queues <= 0) {
        fprintf(stderr, "Error: MLFQ config must define at least one queue\n");
        return -1;
    }

    if (config->boost_period <= 0) {
        fprintf(stderr, "Error: MLFQ config must define BOOST_PERIOD\n");
        return -1;
    }

    // Ensure all queues from Q0 to Q(num_queues-1) were configured
    for (int i = 0; i < config->num_queues; i++) {
        if (config->quantums[i] == 0 || config->allotments[i] == 0) {
            fprintf(stderr, "Error: Missing configuration for Q%d\n", i);
            return -1;
        }
    }

    return 0;
}