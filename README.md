# OS Algorithm Analyzer
(Homework for CE 4348 - Operating System Concepts)

[Discussion and Proposal for Improvements](./DISCUSSION%20AND%20PROPOSAL.md)

## Algorithms analyzed

- First Come, First Served (FCFS)
- Round Robin, with quanta of 1 and 3
- Shortest Remaining Time (SRT)
- Highest Response Ration Next (HRRN)

These are outlined in section 9.2 of "Operating Systems: Internals and Design Principles" by William Stallings on page 431 or thereabouts.
## Results with example data

| Process | Arrival time | Service time | Disk I/O time (total) | Disk I/O activity $\dagger$ |
| ------- | ------------ | ------------ | --------------------- | --------------------------- |
| A       | 0            | 6            | 1                     | 1                           |
| B       | 2            | 12           | 2                     | 2                           |
| C       | 4            | 8            | 1                     | 1                           |
| D       | 6            | 10           | 0                     | 0                           |
| E       | 8            | 4            | 2                     | 2                           |

$\dagger$ At equal intervals during service time

```
=======================
First Come First Served
vvvvvvvvvvvvvvvvvvvvvvv
t |ready|io   |r|done
---------------------
 1|     |     |A|     
 2|    B|     |A|     
 3|    B|     |A|     
 4|    C|    A|B|     
 5|   AC|     |B|     
 6|  ACD|     |B|
 7|  ACD|     |B|
 8| ACDE|     |B|
 9| ACDE|     |B|
10|  CDE|    B|A|
11|  CDE|    B|A|
12| BCDE|     |A|
13|  CDE|     |B|    A
14|  CDE|     |B|    A
15|  CDE|     |B|    A
16|  CDE|     |B|    A
. . .
Finish time:         45   seconds
Avg turnaround time: 25.4 seconds
Max turnaround time: 37   seconds
Avg response time:   12.60 seconds
Max response time:   29   seconds
TAT/Response ratio:  2.02 seconds/second
Throughput: 0.11 processes/second
=======================
Round Robin, Quantum 1
vvvvvvvvvvvvvvvvvvvvvvv
t |ready|io   |r|done
---------------------
 1|     |     |A|
 2|    A|     |B|
 3|    B|     |A|     
 4|   CA|     |B|
 5|   AB|     |C|
 6|  BDC|     |A|
 7|  DCA|     |B|
 8| CAEB|     |D|
. . .
Finish time:         41   seconds
Avg turnaround time: 28.6 seconds
Max turnaround time: 38   seconds
Avg response time:   1.20 seconds
Max response time:    3   seconds
TAT/Response ratio:  23.83 seconds/second
Throughput: 0.12 processes/second
=======================
Round Robin, Quantum 3
vvvvvvvvvvvvvvvvvvvvvvv
t |ready|io   |r|done
---------------------
 1|     |     |A|
 2|    B|     |A|     
 3|    B|     |A|
 4|    C|    A|B|
 5|   AC|     |B|
 6|  ACD|     |B|
 7|  CDB|     |A|
 8| CDBE|     |A|
. . .
Finish time:         41   seconds
Avg turnaround time: 26.2 seconds
Max turnaround time: 38   seconds
Avg response time:   5.20 seconds
Max response time:   11   seconds
TAT/Response ratio:  5.04 seconds/second
Throughput: 0.12 processes/second
=======================
Shortest Remaining Time
vvvvvvvvvvvvvvvvvvvvvvv
t |ready|io   |r|done
---------------------
 1|     |     |A|
 2|    B|     |A|     
 3|    B|     |A|
 4|    B|    A|C|
 5|   AB|     |C|
 6|  CBD|     |A|
 7|   BD|    A|C|
 8| ABDC|     |E|
. . .
Finish time:         53   seconds
Avg turnaround time: 21.8 seconds
Max turnaround time: 51   seconds
Avg response time:   9.80 seconds
Max response time:   27   seconds
TAT/Response ratio:  2.22 seconds/second
vvvvvvvvvvvvvvvvvvvvvvvvvv
t |ready|io   |r|done
---------------------
 1|     |     |A|
 2|    B|     |A|
 3|    B|     |A|
 4|    C|    A|B|
 5|   AC|     |B|
 6|  ACD|     |B|
 7|  ACD|     |B|
 8| ACDE|     |B|
. . .
Finish time:         45   seconds       
Avg turnaround time: 25.4 seconds       
Max turnaround time: 37   seconds       
Avg response time:   12.60 seconds      
Max response time:   29   seconds       
TAT/Response ratio:  2.02 seconds/second

Throughput: 0.11 processes/second
```

## Assumptions

1. The service time will always be divisible by the total disk I/O time.
2. In FCFS, a process returning from I/O activity will be placed in the *front* of the queue. Let's make FCFS as unfair as possible!
3. The IO is a queue and only one process can handle IO at a time. This means that if there are multiple processes in the IO queue, only the front process will have its IO timer decremented.

## Random results

Each scheduler was run in succession on **30 random processes**. The process arrival times ranged from (1-11), service times ranged from (1-41), total IO time from (1-11), and IO blocks from (1-11).

| **Scheduler** | **FCFS** | **RR(1)** | **RR(3)** | **SRT** | **HRRN** |
| ------------- | -------- | --------- | --------- | ------- | -------- |
| Finish time   | 477      | 431       | 431       | 464     | 477      |
| Avg Tat       | 231.7    | 211.5     | 201.6     | 160.9   | 231.7    |
| Avg RT        | 191.27   | 14        | 70.3      | 63.83   | 191.27   |
| TAT/RT        | 1.21     | 15.11     | 2.87      | 2.52    | 1.21     |
| Throughput    | 0.06     | 0.07      | 0.07      | 0.06    | 0.06     |

### Sample output

```
=======================
First Come First Served
vvvvvvvvvvvvvvvvvvvvvvv
t |ready|io   |r|done
---------------------
 1|   IM|     |B|     
 2|IMGQ[\^|     |B|     
 3|IMGQ[\^E|     |B|     
 4|MGQ[\^EDFNTW|    B|I|     
 5|BMGQ[\^EDFNTWUY|     |I|
 6|MGQ[\^EDFNTWUYALO|    I|B|
 7|MGQ[\^EDFNTWUYALOHRXZ]|    I|B|     
 8|MGQ[\^EDFNTWUYALOHRXZ]V|    I|B|    
 9|GQ[\^EDFNTWUYALOHRXZ]VJPS|   IB|M|  
10|IGQ[\^EDFNTWUYALOHRXZ]VJPSK|    B|M|
11|GQ[\^EDFNTWUYALOHRXZ]VJPSKC|   BM|I|
12|BGQ[\^EDFNTWUYALOHRXZ]VJPSKC|    M|I|
. . .
Finish time:         477   seconds     
Avg turnaround time: 231.7 seconds     
Max turnaround time: 466   seconds
Avg response time:   191.27 seconds    
Max response time:   409   seconds     
TAT/Response ratio:  1.21 seconds/second
Throughput: 0.06 processes/second      
=======================
Round Robin, Quantum 1
vvvvvvvvvvvvvvvvvvvvvvv
t |ready|io   |r|done
---------------------
 1|   IM|     |B|
 2|MGQ[\^B|     |I|
 3|GQ[\^BEI|     |M|
 4|Q[\^BEIDFNTWM|     |G|
 5|[\^BEIDFNTWMUY|     |Q|    G
 6|\^BEIDFNTWMUYALO|     |[|   GQ      
 7|^BEIDFNTWMUYALOHRXZ][|     |\|   GQ 
 8|BEIDFNTWMUYALOHRXZ][V\|     |^|   GQ
 9|EIDFNTWMUYALOHRXZ][V\JPS^|     |B|   GQ
10|IDFNTWMUYALOHRXZ][V\JPS^KB|     |E|   GQ
11|DFNTWMUYALOHRXZ][V\JPS^KBCE|     |I|   GQ
12|FNTWMUYALOHRXZ][V\JPS^KBCEI|     |D|   GQ
. . .
Finish time:         431   seconds     
Avg turnaround time: 211.5 seconds     
Max turnaround time: 425   seconds     
Avg response time:   14.00 seconds     
Max response time:   31   seconds      
TAT/Response ratio:  15.11 seconds/second
Throughput: 0.07 processes/second      
=======================
Round Robin, Quantum 3
vvvvvvvvvvvvvvvvvvvvvvv
t |ready|io   |r|done
---------------------
 1|   IM|     |B|
 2|IMGQ[\^|     |B|
 3|IMGQ[\^E|     |B|
 4|MGQ[\^EDFNTW|    B|I|
 5|BMGQ[\^EDFNTWUY|     |I|
 6|MGQ[\^EDFNTWUYALO|    I|B|
 7|MGQ[\^EDFNTWUYALOHRXZ]|    I|B|     
 8|MGQ[\^EDFNTWUYALOHRXZ]V|    I|B|    
 9|GQ[\^EDFNTWUYALOHRXZ]VJPS|   IB|M|  
10|IGQ[\^EDFNTWUYALOHRXZ]VJPSK|    B|M|
11|GQ[\^EDFNTWUYALOHRXZ]VJPSKC|   BM|I|
12|BGQ[\^EDFNTWUYALOHRXZ]VJPSKC|    M|I|
. . .
Finish time:         431   seconds     
Avg turnaround time: 201.6 seconds     
Max turnaround time: 425   seconds     
Avg response time:   70.30 seconds
Max response time:   143   seconds     
TAT/Response ratio:  2.87 seconds/second
Throughput: 0.07 processes/second      
=======================
Shortest Remaining Time
vvvvvvvvvvvvvvvvvvvvvvv
t |ready|io   |r|done
---------------------
 1|   BM|     |I|
 2|BMIQ[\^|     |G|
 3|BMI[\^E|     |Q|    G
 4|BMI[\^EDNTW|     |F|   GQ
 5|BM[\^EDNTWUY|     |I|  GQF
 6|BM[\^EDNTWUYALO|     |I|  GQF
 7|BM[\^EDNWUYALOHRXZ]|    I|T|  GQF   
 8|BM[\^EDNWUYALOHRXZV|   IT|]|  GQF   
 9|BM[\^EDNWUYALOHRXZVJP]|   IT|S|  GQF
10|BM[\^EDNWUYALOHRXZVJP]K|   IT|S|  GQF
11|IBM[\^EDNWUYALOHRXZVJP]C|    T|K| GQFS
12|BM[\^EDNWUYALOHRXZVJP]C|    T|I|GQFSK
. . .
Finish time:         464   seconds     
Avg turnaround time: 160.9 seconds     
Max turnaround time: 458   seconds     
Avg response time:   63.83 seconds     
Max response time:   372   seconds     
TAT/Response ratio:  2.52 seconds/second
Throughput: 0.06 processes/second      
==========================
Highest Response Time Next
vvvvvvvvvvvvvvvvvvvvvvvvvv
t |ready|io   |r|done
---------------------
 1|   IM|     |B|
 2|IMGQ[\^|     |B|
 3|IMGQ[\^E|     |B|
 4|MGQ[\^EDFNTW|    B|I|
 5|BMGQ[\^EDFNTWUY|     |I|
 6|MGQ[\^EDFNTWUYALO|    I|B|     
 7|MGQ[\^EDFNTWUYALOHRXZ]|    I|B|     
 8|MGQ[\^EDFNTWUYALOHRXZ]V|    I|B|    
 9|GQ[\^EDFNTWUYALOHRXZ]VJPS|   IB|M|  
10|IGQ[\^EDFNTWUYALOHRXZ]VJPSK|    B|M|
11|GQ[\^EDFNTWUYALOHRXZ]VJPSKC|   BM|I|
12|BGQ[\^EDFNTWUYALOHRXZ]VJPSKC|    M|I|
. . .
Finish time:         477   seconds   
Avg turnaround time: 231.7 seconds   
Max turnaround time: 466   seconds   
Avg response time:   191.27 seconds  
Max response time:   409   seconds   
TAT/Response ratio:  1.21 seconds/second
Throughput: 0.06 processes/second
```

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
3. If the process reaches $t_{max}$ or is interrupted, it is re-inserted into the tree based on its updated execution time.
4. The new leftmost node is selected, repeating the process.