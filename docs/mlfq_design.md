# MLFQ Design Justification

This document outlines the design decisions for the Multi-Level Feedback Queue (MLFQ) scheduling algorithm implemented in this simulator.

## Queue Levels
**Justification for 3 Levels:**
We chose exactly 3 priority queues for our MLFQ scheduler. Three levels strike the right balance between responsiveness for short, interactive jobs (which will stay in the higher queues) and fairness for long-running, CPU-intensive jobs (which will sink to the bottom queue). A deeper queue system could introduce unnecessary overhead and complexity without significantly improving throughput or response times for standard workloads. 

## Quantums & Allotments
**Rationale for Quantums (10, 30, FCFS):**
- **Queue 0 (High Priority):** Time quantum of 10. A short quantum ensures high responsiveness for interactive or I/O-bound jobs. Its allotment is set to 50.
- **Queue 1 (Medium Priority):** Time quantum of 30. If a job is too CPU-bound for Queue 0, it sinks to Queue 1. The larger quantum allows it to execute more work while still periodically yielding. Its allotment is set to 100.
- **Queue 2 (Low Priority):** Operates on a First-Come, First-Served (FCFS) basis (or an effectively infinite quantum/allotment). Jobs that reach this level are purely CPU-bound and run until completion, or until a priority boost occurs.

The increasing quantums and allotments help avoid preempting CPU-bound jobs too frequently, which minimizes context switching overhead while still giving shorter jobs a chance to run.

## Priority Boost Period
**Preventing CPU Starvation:**
A priority boost period of 200 time units is implemented. Starvation occurs when low-priority jobs (in Queue 1 or Queue 2) are starved of CPU time because there are always high-priority jobs entering or remaining in Queue 0. By periodically boosting all jobs back to Queue 0 every 200 time units, we guarantee that all jobs will eventually receive CPU time, thus resolving the starvation problem.

## Anti-Gaming Mechanism
**Preventing Scheduler Gaming:**
To prevent jobs from "gaming" the scheduler (e.g., yielding right before their time quantum expires to remain in a high-priority queue), our MLFQ tracks the *total* time a job has spent executing at a given priority level (its allotment). A job is demoted to the next lower priority queue once its total accumulated run time at that level exceeds its allotment, regardless of how many times it yielded or how many short quantums it consumed. This ensures fairness and prevents processes from monopolizing the high-priority queue.
