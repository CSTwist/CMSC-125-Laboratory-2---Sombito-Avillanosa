#include "scheduler.h"
#include "process.h"
#include <stdio.h>

void print_ganttChart(GanttChart *chart) {
    printf("=== Gantt Chart ===\n");
    printf("[");

    for (int i = 0; i < chart->size; i++) {
        if (chart->process_order[i] == NULL || chart->process_order[i-1]->pid == chart->process_order[i]->pid) {
            printf("-");
        }
        else {
            printf("%s", chart->process_order[i]->pid);
        }
    }

    printf("]\n");
    printf("%d\n", chart->size);
}