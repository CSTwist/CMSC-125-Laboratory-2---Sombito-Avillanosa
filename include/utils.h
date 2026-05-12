#ifndef UTILS_H
#define UTILS_H

#include "process.h"
#include "scheduler.h"

int load_from_file(const char* filename, int* num_processes, int MAX_PROCESSES, Process* processes);
void load_from_string(char* str, int* num_processes, int MAX_PROCESSES, Process* processes);
void add_process(const char* pid, int arrival, int burst, int* num_processes, int MAX_PROCESSES, Process* processes);
int load_mlfq_config(const char *filename, MLFQConfig *config);

#endif