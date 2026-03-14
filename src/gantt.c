#include "scheduler.h"
#include "process.h"
#include <stdio.h>

void print_ganttChart(GanttChart *chart) {
    printf("=== Gantt Chart ===\n");
    printf("[");

    for (int i = 0; i < chart->size; i++) {
        if (i == 0) {
            printf("%s", chart->process_order[i]->pid);
            chart->timestamps[i] = i;
        } else if (chart->process_order[i] == NULL || 
                chart->process_order[i-1]->pid == chart->process_order[i]->pid) {
            printf("-");
        } else {
            printf("%s", chart->process_order[i]->pid);
            chart->timestamps[i] = i;
        }
    }

    printf("]");
    printf("\nTime: ");

    for (int i = 0; i < chart->size; i++) {
        if (chart->timestamps[i] >= 0) {
            printf("%d ", chart->timestamps[i]);
        }
    }
    
    printf("\n");
}