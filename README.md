# CMSC-125-Laboratory-2---Sombito-Avillanosa

**Created by:**
Chakinzo N. Sombito & Walton Karl L. Avillanosa

---

# Project Overview

This repository contains the implementation of schedsim, a CPU scheduling simulator written in C.

The simulator demonstrates how operating systems schedule processes competing for CPU time using various scheduling algorithms. It models how processes arrive, wait in the ready queue, execute on the CPU, and complete execution while computing important scheduling metrics.

The project is designed as a discrete-event simulator that reads a workload specification and simulates how different CPU scheduling algorithms handle the workload.

The simulator also generates Gantt charts, calculates scheduling metrics, and allows comparative analysis between algorithms.

This program can be executed as a standalone executable and is compatible with the custom Unix Shell (mysh) developed in Laboratory Task 1, where it can be launched using fork() and exec().

---

# Scheduling Algorithms Implemented

The simulator will support the following CPU scheduling algorithms:

### 1. First-Come First-Serve (FCFS)

* Non-preemptive scheduling
* Processes run in order of arrival
* Each process runs until completion
* Simple queue-based implementation

### 2. Shortest Job First (SJF)

* Non-preemptive scheduling
* Processes with the shortest burst time execute first
* Reduces average turnaround time compared to FCFS

### 3. Shortest Time-to-Completion First (STCF)

* Preemptive version of SJF
* Scheduler always runs the process with the shortest remaining time
* Current process may be preempted when a shorter job arrives

### 4. Round Robin (RR)

* Preemptive scheduling
* Each process receives a fixed time quantum
* Processes are rotated through the ready queue
* Improves responsiveness for interactive workloads

### 5. Multi-Level Feedback Queue (MLFQ)

A custom MLFQ scheduler will be designed with the following properties:

* Multiple priority queues
* Different time quantums per queue
* Allotment tracking to prevent gaming
* Downward priority demotion
* Periodic priority boosting
* No prior knowledge of total burst time

The scheduler will infer process behavior dynamically through execution observation.

---

# Scheduling Metrics

For each scheduling algorithm, the simulator computes the following metrics:

| Metric                   | Description                                  |
| ------------------------ | -------------------------------------------- |
| Arrival Time (AT)    | Time when the process enters the ready queue |
| Burst Time (BT)      | Total CPU time required by the process       |
| Finish Time (FT)     | Time when the process completes execution    |
| Turnaround Time (TT) | 'TT = FT − AT'                               |
| Waiting Time (WT)    | 'WT = TT − BT'                               |
| Response Time (RT)   | 'RT = FirstExecution − AT'                   |

These metrics are fundamental to evaluating scheduling performance. 

The simulator also calculates average metrics across all processes.

---

# Input Format

Workloads may be provided through an input file:

```bash
# Process workload specification
# Format: PID ArrivalTime BurstTime

A 0 240
B 10 180
C 20 150
D 25 80
E 30 130
```

Where:

| Field       | Description                             |
| ----------- | --------------------------------------- |
| PID         | Process identifier                      |
| ArrivalTime | Time the process enters the ready queue |
| BurstTime   | Total CPU time required                 |

Lines beginning with '#' are treated as comments.

---

# Command Line Usage

The simulator will support several command-line options.

### Run FCFS

```bash
./schedsim --algorithm=FCFS --input=workload.txt
```

### Run Round Robin

```bash
./schedsim --algorithm=RR --quantum=30 --input=workload.txt
```

### Run MLFQ

```bash
./schedsim --algorithm=MLFQ --mlfq-config=config.txt --input=workload.txt
```

### Provide Workload via CLI

```bash
./schedsim --algorithm=FCFS \
--processes="A:0:240,B:10:180,C:20:150"
```

---

# Output

The simulator will produce:

### 1. Gantt Chart

Example:

```bash
=== Gantt Chart ===
[A--------------][B--------][C------]

Time: 0       240       420       570
```

### 2. Scheduling Metrics Table

```bash
Process | AT | BT | FT | TT | WT | RT
-------------------------------------
A       | 0  |240 |240 |240 | 0  | 0
B       |10  |180 |420 |410 |230 |230
C       |20  |150 |570 |550 |400 |400
```

### 3. Average Metrics

```bash
Average Turnaround Time: 400
Average Waiting Time: 210
Average Response Time: 210
```

---

# Comparison Mode

The simulator will also support algorithm comparison.

```bash
./schedsim --compare --input=workload.txt
```

Example output:

```bash
Algorithm | Avg TT | Avg WT | Avg RT
------------------------------------
FCFS      | 400.0  | 210.0  | 210.0
SJF       | 340.0  | 150.0  | 150.0
STCF      | 280.0  |  90.0  |  25.0
RR        | 385.0  | 195.0  |  45.0
MLFQ      | 310.0  | 120.0  |  38.0
```

---

# Project Structure

```bash
schedsim/
│
├── Makefile
├── README.md
│
├── include/
│   ├── process.h
│   ├── scheduler.h
│   ├── metrics.h
│   └── gantt.h
│
├── src/
│   ├── main.c
│   ├── process.c
│   ├── fcfs.c
│   ├── sjf.c
│   ├── stcf.c
│   ├── rr.c
│   ├── mlfq.c
│   ├── metrics.c
│   └── gantt.c
│
├── tests/
│   ├── workload1.txt
│   └── workload2.txt
│
└── docs/
    └── mlfq_design.md
```

---

# Compilation

Compile the simulator using:

```bash
make
```

Clean compiled files:

```bash
make clean
```

Run test suite:

```bash
make test
```

---

# Known Limitations and Assumptions

* **Static Limits:** The simulator uses fixed-size arrays for process limits (MAX_PROCESSES=100) and maximum simulation time (MAX_TIME=10000). Large workloads exceeding these boundaries will cause a warning or error.
* **MLFQ Constraints:** The Multi-Level Feedback Queue scheduler determines process lengths strictly by execution time and queue placement rather than relying on burst times, simulating a true dynamic scheduling algorithm.
* **Memory Management:** For simplicity and safety, the simulator largely utilizes static memory allocation over dynamic allocation, inherently avoiding memory leaks without imposing heap management overheads.

---

# Current Progress

- **Completed:** Repository setup and modular file structure.
- **Completed:** Data structure modeling for queues and process scheduling.
- **Completed:** Implementation of the main simulation engine.
- **Completed:** Implementation of scheduling algorithms: FCFS, SJF, STCF, RR, and a custom MLFQ.
- **Completed:** Gantt Chart rendering and accurate simulation metrics generation.
- **Completed:** Automated testing pipeline using make test.
