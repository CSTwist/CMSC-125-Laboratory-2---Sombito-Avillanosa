#include "scheduler.h"
#include "process.h"
#include <stdio.h>

void print_ganttChart(GanttChart *chart) {
    printf("=== Gantt Chart ===\n");
    printf("[");

    for (int i = 0; i < chart->size; i++) {
        if (chart->process_order[i] == NULL) {
            printf("-");
        } else {
            printf("%s", chart->process_order[i]->pid);
        }
    }

    printf("]\n");
    printf("%d\n", chart->size);
}