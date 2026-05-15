# MLFQ Design Justification

This document outlines the design decisions for the Multi-Level Feedback Queue (MLFQ) scheduling algorithm.

## Queue Levels
**3 Levels:**
The scheduler utilizes 3 priority queues. This number provides a balance between responsiveness for short, interactive jobs and throughput for long-running, CPU-intensive tasks.

## Quantums & Allotments
**Quantums (10, 30, FCFS):**
- **Queue 0:** Time quantum of 10. Short quantum ensures high responsiveness. Allotment is 50.
- **Queue 1:** Time quantum of 30. For jobs that require more CPU time than Queue 0 allows. Allotment is 100.
- **Queue 2:** First-Come, First-Served (FCFS) basis. Jobs that reach this level are CPU-bound and run until completion or priority boost.

Increasing quantums and allotments help reduce context switching overhead while allowing shorter jobs to finish quickly.

## Priority Boost Period
**Starvation Prevention:**
A priority boost occurs every 200 time units. This prevents low-priority jobs from being starved by a constant stream of high-priority arrivals. All jobs are moved back to Queue 0 periodically.

## Anti-Gaming Mechanism
To prevent jobs from yielding right before their quantum expires to stay in a high-priority queue, the scheduler tracks the total time spent at each level (allotment). Demotion occurs once the allotment is exceeded, regardless of yields.
