# Discussion and proposal

| **Scheduler** | **FCFS** | **RR(1)** | **RR(3)** | **SRT** | **HRRN** |
| ------------- | -------- | --------- | --------- | ------- | -------- |
| Finish time   | 477      | 431       | 431       | 464     | 477      |
| Avg Tat       | 231.7    | 211.5     | 201.6     | 160.9   | 231.7    |
| Avg RT        | 191.27   | 14        | 70.3      | 63.83   | 191.27   |
| TAT/RT        | 1.21     | 15.11     | 2.87      | 2.52    | 1.21     |
| Throughput    | 0.06     | 0.07      | 0.07      | 0.06    | 0.06     |

<small>Above: Results of random sample of 30 processes</small>

## Discussion

**Do these results align with our expectations?** Let's refer to Table 9.3 of the textbook. It compares the expected behavior of each of these algorithms.

The table makes no note of finish time. Observe that the **finish time** is almost unchanged across schedulers. Since each scheduler uses the CPU at nearly every time step, we conclude that each takes high advantage of the CPU. These patterns align with the **throughput** data as well, since the two are proportional. The increases in finish time occur when the scheduler is waiting on a process in the I/O queue, and where the CPU is therefore left idle for a brief period of time.

**Response time** for each scheduler is also listed as either "good," except for FCFS. The table notes that the response time will be high if there is a high variance in execution times. I tested with this very condition, and the high response time is reflected in my results.

The **TAT** for all fell between (160-231) seconds. Note that *this large number is due* to the high values that I fed to each process's random distribution. The SRT scheduler had the lowest TAT on average. This is because this scheduler will always choose the "low hanging fruit," and is therefore effective at completing processes.

There was high variance in the **TAT/RT** ratio. This ratio reflects the scheduler's focus on responsiveness versus total completion. The RR(1) scheduler had the highest value, and this was matched with the lowest RT. In other words, the RR(1) was effective at addressing each process quickly, but was poor at completing them in efficiently.

## Proposal for improvement

Each of these algorithms lies on a scale from being **responsive** to **efficient**. I mean that responsive schedulers will address new processes quickly, and will therefore be more pleasant for users (such as with GUIs). Efficient schedulers may be less smooth/uniform, but they will complete individual processes more quickly overall.

FCFS is already easy to implement and has minimal overhead, yet it penalizes short or IO-bound processes. I have two improvements:

1. Add priority queues based on a process's runtime so far. This will add minimal overhead to the existing algorithm, but it prevents short processes from being essentially starved waiting in the queue behind longer processes.
2. When a process returns from an IO operation, allow it to skip to the front of the "ready" queue.

Round Robin is also easy to implement and has minimum overhead. It treats processes fairly and doesn't even come close to starvation. Its best quality is its high response time. My proposed improvement would be to *increment or decrement the quantum dynamically* in response to throughput constraints. This would mean that the RT benefits can be kept within reason.

SRT has a high overhead, but it provided good response time. Like Round Robin, its focus is on responsiveness. To take full advantage of this, SRT's overhead can be improved by *sorting the ready processes by remaining times*. That means that the list wouldn't have to be iterated through entirely to find the lowest response time, and this would greatly reduce the overhead for schedulers with lots of processes.

Finally, HRRN achieves a good balance between RT and throughput, yet it has a very high overhead. My improvement would be to once again sort the ready processes in the ready queue by their response ratio (RR). Processes with a low RR can have their RRs recalculated in batches, only periodically. This is because processes with low response ratio at one timestep are unlikely to have the *highest* RR at the next timestep. By batching and delaying the recalculation of RRs for low-RR ready processes, the overhead could be greatly reduced, depending on what threshold is set.