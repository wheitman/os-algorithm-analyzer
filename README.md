# OS Algorithm Analyzer
(Homework for CE 4348 - Operating System Concepts)

## Algorithms to be analyzed
These are outlined in section 9.2 of "Operating Systems: Internals and Design Principles" by William Stallings on page 431 or thereabouts.
### First-come-first-served (FCFS)

Select the process with the smallest arrival time and run it until completion.

### Round Robin
### Shortest Response Time (SRT)

### Highest Response Ratio Next (HRRN)

$$R=\frac{w+s}{s}$$

where $R$ is the response ratio, $w$ is the time spent waiting on the processor, and $s$ is the expected service time.

## Example data

| Process | Arrival time | Service time | Disk I/O time (total) | Disk I/O activity $$\dagger$$ |
| ------- | ------------ | ------------ | --------------------- | ----------------------------- |
| A       | 0            | 6            | 1                     | 1                             |
| B       | 2            | 12           | 2                     | 2                             |
| C       | 4            | 8            | 1                     | 1                             |
| D       | 6            | 10           | 0                     | 0                             |
| E       | 8            | 4            | 2                     | 2                             |

$$\dagger$$ At equal intervals during service time

## Performance criteria

1. Finish time
2. Response time
3. Turnaround time (TAT)
4. Ratio of TAT to service time
5. Throughput

## Overview of Linux scheduling

Initially, Linux used a Round-Robin scheduling algorithm. This was replaced with the priority queue-based O(N) scheduler, which was then replaced by a similar O(1) scheduler in kernel 2.4. Finally, Linux's default algorithm (that is, the one used for `SCHED_NORMAL`, non-realtime tasks) is the Completely Fair Scheduler, or CFS.

How does CFS work? It replaces the early queue with a red-black tree, which is a self-balancing tree. 

- Whenever a new process is created, the CFS inserts it into the red-black tree.
- Processes are sorted into the tree using their virtual runtime (VRT).
- CFS selects the left-most node as the next process to be executed, since this leftmost task has the smallest VRT and therefore has received the smallest slice of CPU time.

Each process has a *maximum execution time*, equal to the time that the process has been waiting in the tree divided by the total number of processes:
$$
t_{max}=\frac{t_{waiting}}{n_{processes}}
$$
When the scheduler is invoked to run a new process:

1. The leftmost node is select in the rbtree.
2. If the selected process completes, it is removed from the tree.
3. If the process reaches $$t_{max}$$ or is interrupted, it is re-inserted into the tree based on its updated execution time.
4. The new leftmost node is selected, repeating the process.