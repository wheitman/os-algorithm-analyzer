#include <algorithm> // std::sort
#include <cstdio>
#include <iostream>
#include <random>
#include <stdlib.h> // For colored output
#include <string>
#include <vector>

using namespace std;

class Process
{
public:
    Process(string label, float arrivalTime, float serviceTime, float totalIo, float ioBlock)
    {
        this->label = label;
        this->arrivalTime = arrivalTime;
        remainingService = serviceTime;
        remainingIo = totalIo;
        remainingIoBlock = ioBlock;
        this->ioBlock = ioBlock;
        responseTime = -1;

        if (ioBlock == 0)
            serviceBlock = 0;
        else
            serviceBlock = serviceTime / ((totalIo / ioBlock) + 1);
    }
    string label;
    int arrivalTime, remainingService, remainingIo, ioBlock, serviceBlock, remainingIoBlock, finishTime, responseTime;
    int currentRunningTime = 0;
    int waitingTime = 0;
};

class Scheduler
{
public:
    Scheduler()
    {
        time = 1;
        blockStartTime = 1;
    }

    void addProcess(Process p)
    {
        futureQueue.push_back(p);
    }

    void printStatus()
    {
        string readyQueueStr = "", ioQueueStr = "", doneQueueStr = "";

        // Just form some strings with the labels of the processes in each queue
        if (readyQueue.empty())
            readyQueueStr = "";
        else
            for (Process p : readyQueue)
                readyQueueStr += p.label;
        if (ioQueue.empty())
            ioQueueStr = "";
        else
            for (Process p : ioQueue)
                ioQueueStr += p.label;
        if (doneQueue.empty())
            doneQueueStr = "";
        else
            for (Process p : doneQueue)
                doneQueueStr += p.label;
        if (time == 30)
        {
            cout << ". . ." << endl;
        }
        else if (time < 30)
        {
            printf(
                "%2i|%5s|%5s|%1s|%5s\n",
                time,
                readyQueueStr.c_str(),
                ioQueueStr.c_str(),
                runningProcess.empty() ? "" : runningProcess.front().label.c_str(),
                doneQueueStr.c_str());
        }
    }

    void printProcesses()
    {
        cout << "-------------" << endl;
        cout << "Step " << time << endl;
        cout << "Future: " << endl;
        for (Process proc : futureQueue)
        {
            cout << "\t" << proc.label + " arrives at t=" + to_string(proc.arrivalTime) << endl;
        }
        cout << "Ready: " << endl;
        for (Process proc : readyQueue)
        {
            cout << proc.label + " " + to_string(proc.arrivalTime) << endl;
        }
        cout << "IO: " << endl;
        for (Process proc : ioQueue)
        {
            cout << proc.label + " " + to_string(proc.arrivalTime) << endl;
        }
        cout << "-------------" << endl;
    }

    void run()
    {
        throw runtime_error("run() not implemented in base class");
    }

    void analyze(vector<Process> processes)
    {
        int maxFinishTime = 0;
        float totalFinishTime = 0.0;
        int maxResponseTime = 0;
        float totalResponseTime = 0.0;
        for (Process p : processes)
        {
            maxFinishTime = max(maxFinishTime, p.finishTime);
            totalFinishTime += p.finishTime;
            maxResponseTime = max(maxResponseTime, p.responseTime);
            totalResponseTime += p.responseTime;
        }
        printf("Finish time:         %2i   seconds\n", time - 1);
        printf("Avg turnaround time: %2.1f seconds\n", totalFinishTime / processes.size());
        printf("Max turnaround time: %2i   seconds\n", maxFinishTime);
        printf("Avg response time:   %2.2f seconds\n", totalResponseTime / processes.size());
        printf("Max response time:   %2i   seconds\n", maxResponseTime);
        printf("TAT/Response ratio:  %2.2f seconds/second\n", (totalFinishTime / processes.size()) / (totalResponseTime / processes.size()));
        printf("Throughput: %.2f processes/second\n", static_cast<float>(processes.size()) / (time - 1));
    }

    vector<Process> doneQueue;
    vector<Process> futureQueue;
    vector<Process> readyQueue;
    vector<Process> ioQueue;
    vector<Process> runningProcess;
    int time;
    int blockStartTime;
};

class FcfsScheduler : public Scheduler
{
public:
    FcfsScheduler() : Scheduler()
    {
    }

    // Find the process with the smallest arrival time and remove it
    void run()
    {
        printf("t |ready|io   |r|done\n");
        printf("---------------------\n");

        // Continue running until all processes are in "done" queue
        while (!(ioQueue.empty() &&
                 readyQueue.empty() &&
                 futureQueue.empty() &&
                 runningProcess.empty()))
        {
            if (time > 1000)
            {
                throw runtime_error("Maximum iterations reached.");
            }

            // 1. Check for new additions to the Ready queue
            //    from the Future queue

            for (unsigned int i = 0; i < futureQueue.size(); ++i)
            {
                if (futureQueue.at(i).arrivalTime <= time)
                {
                    if (runningProcess.empty())
                    {
                        runningProcess.push_back(futureQueue.at(i));

                        // The process was run immediately upon arrival,
                        // so the responseTime should be zero
                        if (runningProcess.back().responseTime == -1)
                            runningProcess.back().responseTime = 0;
                    }
                    else
                    {
                        readyQueue.push_back(futureQueue.at(i));
                    }
                    futureQueue.erase(futureQueue.begin() + i);
                    --i;
                }
            }

            // Is the running process done?
            if (!runningProcess.empty() && runningProcess.front().remainingService == 0)
            {
                // Add currently running process to "done"
                runningProcess.front().currentRunningTime = 0;
                doneQueue.push_back(runningProcess.front());

                doneQueue.back().finishTime = time - runningProcess.front().arrivalTime;
                // Clear out currently running process
                runningProcess.clear();
                // Add the ready queue's front process to "running"
                if (!readyQueue.empty())
                {

                    runningProcess.push_back(readyQueue.front());

                    if (runningProcess.back().responseTime == -1)
                        runningProcess.back().responseTime = time - runningProcess.back().arrivalTime;
                    blockStartTime = time;
                    // Remove from the ready queue's front
                    readyQueue.erase(readyQueue.begin());
                }
                // Reset blockStartTime
                blockStartTime = time;
            }

            // Is the running process ready for IO?
            if (runningProcess.empty())
            {
                // cerr << "No process is running!" << endl;
            }
            else if (runningProcess.front().serviceBlock != 0 && time - blockStartTime >= runningProcess.front().serviceBlock)
            {
                // printf("%i >= %i, pushing to I/O\n", time - blockStartTime, runningProcess.front().serviceBlock);
                // Move running process to I/O queue
                runningProcess.front().currentRunningTime = 0;
                ioQueue.push_back(runningProcess.front());
                runningProcess.clear();
                if (!readyQueue.empty())
                {
                    runningProcess.push_back(readyQueue.front());

                    if (runningProcess.back().responseTime == -1)
                        runningProcess.back().responseTime = time - runningProcess.back().arrivalTime;
                    blockStartTime = time;
                    readyQueue.erase(readyQueue.begin());
                }
            }

            // Look through IO queue processes.
            // Decrement IO time on front process.
            // If remaining % io_block == 0, pop
            if (!ioQueue.empty())
            {

                if (ioQueue.front().remainingIoBlock == 0)
                {
                    ioQueue.front().remainingIoBlock = ioQueue.front().ioBlock;
                    if (runningProcess.empty())
                        runningProcess.push_back(ioQueue.front());
                    else
                        readyQueue.insert(readyQueue.begin(), ioQueue.front());
                    ioQueue.erase(ioQueue.begin());
                }
                else
                {
                    ioQueue.front().remainingIoBlock--;
                }
                ioQueue.front().remainingIo--;
            }

            printStatus();
            if (!runningProcess.empty())
                runningProcess.front().remainingService--;
            time++;
        }
        analyze(doneQueue);
    }

private:
};

class RoundRobinScheduler : public Scheduler
{
public:
    RoundRobinScheduler(int quantum) : Scheduler()
    {
        _quantum = quantum;
    }

    // Find the process with the smallest arrival time and remove it
    void run()
    {
        printf("t |ready|io   |r|done\n");
        printf("---------------------\n");

        // Continue running until all processes are in "done" queue
        while (!(ioQueue.empty() &&
                 readyQueue.empty() &&
                 futureQueue.empty() &&
                 runningProcess.empty()))
        {
            if (time > 1000)
            {
                throw runtime_error("Maximum iterations reached.");
            }

            // 1. Check for new additions to the Ready queue
            //    from the Future queue

            for (unsigned int i = 0; i < futureQueue.size(); ++i)
            {
                if (futureQueue.at(i).arrivalTime <= time)
                {
                    if (runningProcess.empty())
                    {
                        runningProcess.push_back(futureQueue.at(i));

                        // The process was run immediately upon arrival,
                        // so the responseTime should be zero
                        if (runningProcess.back().responseTime == -1)
                            runningProcess.back().responseTime = 0;
                    }
                    else
                    {
                        readyQueue.push_back(futureQueue.at(i));
                    }
                    futureQueue.erase(futureQueue.begin() + i);
                    --i;
                }
            }

            // Is the running process done?
            if (!runningProcess.empty() && runningProcess.front().remainingService == 0)
            {
                // Add currently running process to "done"
                runningProcess.front().currentRunningTime = 0;
                doneQueue.push_back(runningProcess.front());

                doneQueue.back().finishTime = time - runningProcess.front().arrivalTime;
                // Clear out currently running process
                runningProcess.clear();
                // Add the ready queue's front process to "running"
                if (!readyQueue.empty())
                {

                    runningProcess.push_back(readyQueue.front());

                    if (runningProcess.back().responseTime == -1)
                        runningProcess.back().responseTime = time - runningProcess.back().arrivalTime;
                    blockStartTime = time;
                    // Remove from the ready queue's front
                    readyQueue.erase(readyQueue.begin());
                }
                // Reset blockStartTime
                blockStartTime = time;
            }

            if (runningProcess.empty())
            {
                // cerr << "No process is running!" << endl;
            }
            // Is the running process ready for IO?
            else if (runningProcess.front().serviceBlock != 0 && time - blockStartTime >= runningProcess.front().serviceBlock)
            {
                // printf("%i >= %i, pushing to I/O\n", time - blockStartTime, runningProcess.front().serviceBlock);
                // Move running process to I/O queue
                runningProcess.front().currentRunningTime = 0;
                ioQueue.push_back(runningProcess.front());
                runningProcess.clear();
                if (!readyQueue.empty())
                {
                    runningProcess.push_back(readyQueue.front());

                    if (runningProcess.back().responseTime == -1)
                        runningProcess.back().responseTime = time - runningProcess.back().arrivalTime;
                    blockStartTime = time;
                    readyQueue.erase(readyQueue.begin());
                }
            }
            // Has the running service met its quantum?
            else if (runningProcess.front().currentRunningTime >= _quantum)
            {
                // cout << "Quantum met, moving to ready queue" <<endl;
                runningProcess.front().currentRunningTime = 0;
                readyQueue.push_back(runningProcess.front());
                runningProcess.clear();
                if (!readyQueue.empty())
                {
                    runningProcess.push_back(readyQueue.front());

                    if (runningProcess.back().responseTime == -1)
                        runningProcess.back().responseTime = time - runningProcess.back().arrivalTime;
                    blockStartTime = time;
                    readyQueue.erase(readyQueue.begin());
                }
            }

            // Look through IO queue processes.
            // Decrement IO time on front process.
            // If remaining % io_block == 0, pop
            if (!ioQueue.empty())
            {

                if (ioQueue.front().remainingIoBlock == 0)
                {
                    ioQueue.front().remainingIoBlock = ioQueue.front().ioBlock;
                    if (runningProcess.empty())
                        runningProcess.push_back(ioQueue.front());
                    else
                        readyQueue.insert(readyQueue.begin(), ioQueue.front());
                    ioQueue.erase(ioQueue.begin());
                }
                else
                {
                    ioQueue.front().remainingIoBlock--;
                }
                ioQueue.front().remainingIo--;
            }

            printStatus();
            if (!runningProcess.empty())
            {
                runningProcess.front().remainingService--;
                runningProcess.front().currentRunningTime++;
            }
            time++;
        }
        analyze(doneQueue);
    }

private:
    int _quantum;
};

// Shortest Remaining Time
class SrtScheduler : public Scheduler
{
public:
    SrtScheduler() : Scheduler()
    {
    }

    int getShortestRemainingIdx()
    {
        vector<Process> processes = readyQueue;
        int shortestIdx = 0; // Start by assuming first process
        int shortestVal = processes.front().remainingService;

        for (int i = 0; i < processes.size(); i++)
        {
            Process p = processes.at(i);

            if (p.remainingService < shortestVal)
            {
                shortestVal = p.remainingService;
                shortestIdx = i;
            }
        }
        Process srtProc = processes.at(shortestIdx);
        // printf("SRT process is %s with RT = %i\n", srtProc.label.c_str(), shortestVal);
        // printf("Current running is %i\n", runningProcess.front().remainingService);
        return (shortestIdx);
    }

    // Find the process with the smallest arrival time and remove it
    void run()
    {
        printf("t |ready|io   |r|done\n");
        printf("---------------------\n");

        // Continue running until all processes are in "done" queue
        while (!(ioQueue.empty() &&
                 readyQueue.empty() &&
                 futureQueue.empty() &&
                 runningProcess.empty()))
        {
            if (time > 1000)
            {
                throw runtime_error("Maximum iterations reached.");
            }

            // 1. Check for new additions to the Ready queue
            //    from the Future queue

            for (unsigned int i = 0; i < futureQueue.size(); ++i)
            {
                if (futureQueue.at(i).arrivalTime <= time)
                {
                    if (runningProcess.empty())
                    {
                        runningProcess.push_back(futureQueue.at(i));

                        // The process was run immediately upon arrival,
                        // so the responseTime should be zero
                        if (runningProcess.back().responseTime == -1)
                            runningProcess.back().responseTime = 0;
                    }
                    else
                    {
                        readyQueue.push_back(futureQueue.at(i));
                    }
                    futureQueue.erase(futureQueue.begin() + i);
                    --i;
                }
            }

            // Is the running process done?
            if (!runningProcess.empty() && runningProcess.front().remainingService == 0)
            {
                // Add currently running process to "done"
                runningProcess.front().currentRunningTime = 0;
                doneQueue.push_back(runningProcess.front());

                doneQueue.back().finishTime = time - runningProcess.front().arrivalTime;
                // Clear out currently running process
                runningProcess.clear();
                // Add the SRT process to "running"
                if (!readyQueue.empty())
                {
                    int srtIdx = getShortestRemainingIdx();
                    runningProcess.push_back(readyQueue.at(srtIdx));

                    if (runningProcess.back().responseTime == -1)
                        runningProcess.back().responseTime = time - runningProcess.back().arrivalTime;
                    blockStartTime = time;
                    // Remove from the ready queue's srtIdx
                    readyQueue.erase(readyQueue.begin() + srtIdx);
                }
                // Reset blockStartTime
                blockStartTime = time;
            }

            if (runningProcess.empty())
            {
                // cerr << "No process is running!" << endl;
            }
            // Is the running process ready for IO?
            else if (runningProcess.front().serviceBlock != 0 && time - blockStartTime >= runningProcess.front().serviceBlock)
            {
                // printf("%i >= %i, pushing to I/O\n", time - blockStartTime, runningProcess.front().serviceBlock);
                // Move running process to I/O queue
                runningProcess.front().currentRunningTime = 0;
                ioQueue.push_back(runningProcess.front());
                runningProcess.clear();
                if (!readyQueue.empty())
                {
                    int srtIdx = getShortestRemainingIdx();
                    runningProcess.push_back(readyQueue.at(srtIdx));

                    if (runningProcess.back().responseTime == -1)
                        runningProcess.back().responseTime = time - runningProcess.back().arrivalTime;
                    blockStartTime = time;
                    // Remove from the ready queue's srtIdx
                    readyQueue.erase(readyQueue.begin() + srtIdx);
                }
            }

            // Finally, find the SRT in the ready queue.
            // If its RT is < runningProc.RT, swap them
            if (!readyQueue.empty())
            {
                int readySrtIdx = getShortestRemainingIdx();
                if (readyQueue.at(readySrtIdx).remainingService < runningProcess.front().remainingService)
                {
                    // Swap the two
                    // printf("Swapping %s and %s\n",
                    // readyQueue.at(readySrtIdx).label.c_str(),
                    // runningProcess.front().label.c_str()
                    // );
                    swap(readyQueue.at(readySrtIdx), runningProcess.front());
                }
            }

            // Look through IO queue processes.
            // Decrement IO time on front process.
            // If remaining % io_block == 0, pop
            if (!ioQueue.empty())
            {

                if (ioQueue.front().remainingIoBlock == 0)
                {
                    ioQueue.front().remainingIoBlock = ioQueue.front().ioBlock;
                    if (runningProcess.empty())
                        runningProcess.push_back(ioQueue.front());
                    else
                        readyQueue.insert(readyQueue.begin(), ioQueue.front());
                    ioQueue.erase(ioQueue.begin());
                }
                else
                {
                    ioQueue.front().remainingIoBlock--;
                }
                ioQueue.front().remainingIo--;
            }

            printStatus();
            if (!runningProcess.empty())
            {
                runningProcess.front().remainingService--;
                runningProcess.front().currentRunningTime++;
            }
            time++;
        }
        analyze(doneQueue);
    }

private:
    int _quantum;
};

// Shortest Remaining Time
class HrrnScheduler : public Scheduler
{
public:
    HrrnScheduler() : Scheduler()
    {
    }

    int getHrrnIdx()
    {
        vector<Process> processes = readyQueue;
        int highestResponseRatioIdx = 0; // Start by assuming first process
        int highestVal = processes.front().remainingService;

        for (int i = 0; i < processes.size(); i++)
        {
            Process p = processes.at(i);

            float responseRatio = static_cast<float>(p.waitingTime) / p.remainingService;

            if (responseRatio > highestVal)
            {
                highestVal = p.remainingService;
                highestResponseRatioIdx = i;
            }
        }
        Process srtProc = processes.at(highestResponseRatioIdx);
        // printf("SRT process is %s with RT = %i\n", srtProc.label.c_str(), shortestVal);
        // printf("Current running is %i\n", runningProcess.front().remainingService);
        return (highestResponseRatioIdx);
    }

    // Find the process with the smallest arrival time and remove it
    void run()
    {
        printf("t |ready|io   |r|done\n");
        printf("---------------------\n");

        // Continue running until all processes are in "done" queue
        while (!(ioQueue.empty() &&
                 readyQueue.empty() &&
                 futureQueue.empty() &&
                 runningProcess.empty()))
        {
            if (time > 1000)
            {
                throw runtime_error("Maximum iterations reached.");
            }

            // 1. Check for new additions to the Ready queue
            //    from the Future queue

            for (unsigned int i = 0; i < futureQueue.size(); ++i)
            {
                if (futureQueue.at(i).arrivalTime <= time)
                {
                    if (runningProcess.empty())
                    {
                        runningProcess.push_back(futureQueue.at(i));

                        // The process was run immediately upon arrival,
                        // so the responseTime should be zero
                        if (runningProcess.back().responseTime == -1)
                            runningProcess.back().responseTime = 0;
                    }
                    else
                    {
                        readyQueue.push_back(futureQueue.at(i));
                    }
                    futureQueue.erase(futureQueue.begin() + i);
                    --i;
                }
            }

            // Is the running process done?
            if (!runningProcess.empty() && runningProcess.front().remainingService == 0)
            {
                // Add currently running process to "done"
                runningProcess.front().currentRunningTime = 0;
                doneQueue.push_back(runningProcess.front());

                doneQueue.back().finishTime = time - runningProcess.front().arrivalTime;
                // Clear out currently running process
                runningProcess.clear();
                // Add the SRT process to "running"
                if (!readyQueue.empty())
                {
                    int hrrnIdx = getHrrnIdx();
                    runningProcess.push_back(readyQueue.at(hrrnIdx));

                    if (runningProcess.back().responseTime == -1)
                        runningProcess.back().responseTime = time - runningProcess.back().arrivalTime;
                    blockStartTime = time;
                    // Remove from the ready queue's hrrnIdx
                    readyQueue.erase(readyQueue.begin() + hrrnIdx);
                }
                // Reset blockStartTime
                blockStartTime = time;
            }

            if (runningProcess.empty())
            {
                // cerr << "No process is running!" << endl;
            }
            // Is the running process ready for IO?
            else if (runningProcess.front().serviceBlock != 0 && time - blockStartTime >= runningProcess.front().serviceBlock)
            {
                // printf("%i >= %i, pushing to I/O\n", time - blockStartTime, runningProcess.front().serviceBlock);
                // Move running process to I/O queue
                runningProcess.front().currentRunningTime = 0;
                ioQueue.push_back(runningProcess.front());
                runningProcess.clear();
                if (!readyQueue.empty())
                {
                    int hrrnIdx = getHrrnIdx();
                    runningProcess.push_back(readyQueue.at(hrrnIdx));

                    if (runningProcess.back().responseTime == -1)
                        runningProcess.back().responseTime = time - runningProcess.back().arrivalTime;
                    blockStartTime = time;
                    // Remove from the ready queue's hrrnIdx
                    readyQueue.erase(readyQueue.begin() + hrrnIdx);
                }
            }

            // Finally, find the SRT in the ready queue.
            // If its RT is < runningProc.RT, swap them
            if (!readyQueue.empty())
            {
                int readyHrrnIdx = getHrrnIdx();
                if (readyQueue.at(readyHrrnIdx).remainingService == 0)
                    throw runtime_error("Ready queue process has zero remaining service");
                float queuedHrrn = (readyQueue.at(readyHrrnIdx).waitingTime / readyQueue.at(readyHrrnIdx).remainingService);
                float runningHrrn = (runningProcess.front().waitingTime / runningProcess.front().remainingService);
                if (queuedHrrn > runningHrrn)
                {
                    // Swap the two
                    printf("Swapping %s and %s, which have RRs of %f and %f\n",
                           readyQueue.at(readyHrrnIdx).label.c_str(),
                           runningProcess.front().label.c_str(),
                           queuedHrrn, runningHrrn);
                    swap(readyQueue.at(readyHrrnIdx), runningProcess.front());
                }
            }

            // Look through IO queue processes.
            // Decrement IO time on front process.
            // If remaining % io_block == 0, pop
            if (!ioQueue.empty())
            {

                if (ioQueue.front().remainingIoBlock == 0)
                {
                    ioQueue.front().remainingIoBlock = ioQueue.front().ioBlock;
                    if (runningProcess.empty())
                        runningProcess.push_back(ioQueue.front());
                    else
                        readyQueue.insert(readyQueue.begin(), ioQueue.front());
                    ioQueue.erase(ioQueue.begin());
                }
                else
                {
                    ioQueue.front().remainingIoBlock--;
                }
                ioQueue.front().remainingIo--;
            }

            // Look through ready queue processes.
            // Increment waiting time for each
            for (Process p : readyQueue)
            {
                p.waitingTime++;
            }

            printStatus();
            if (!runningProcess.empty())
            {
                runningProcess.front().remainingService--;
                runningProcess.front().currentRunningTime++;
            }
            time++;
        }
        analyze(doneQueue);
    }

private:
    int _quantum;
};

int main()
{
    FcfsScheduler fcfs;
    RoundRobinScheduler rrs1(1);
    RoundRobinScheduler rrs3(3);
    SrtScheduler srt;
    HrrnScheduler hrrn;

    size_t num_proceses = 30;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> distr(2, 22);

    for (int i = 0; i < num_proceses; i++)
    {
        string label = string(1, char(i + 65)); // i=0 -> 65 = 'A'

        int arrivalTime = distr(gen) / 2;

        int totalIoTime = (distr(gen) / 2);
        int ioBlockTime = totalIoTime / max(((distr(gen) / 3)), 1);

        // Find a service time that can be cleanly divided into IO segments
        int serviceTime = ioBlockTime == 0 ? distr(gen) : (((totalIoTime / ioBlockTime) + 1) * (distr(gen) / 4)) + 1;

        Process p = Process(label, arrivalTime, serviceTime, totalIoTime, ioBlockTime);
        printf("Adding process %s with {%i,%i,%i,%i}\n", p.label.c_str(), p.arrivalTime, p.remainingService, p.remainingIo, p.ioBlock);
        fcfs.addProcess(p);
        rrs1.addProcess(p);
        rrs3.addProcess(p);
        srt.addProcess(p);
        hrrn.addProcess(p);
    }
    cout << "=======================" << endl;
    cout << "First Come First Served" << endl;
    cout << "vvvvvvvvvvvvvvvvvvvvvvv" << endl;
    fcfs.run();
    cout << "=======================" << endl;
    cout << "Round Robin, Quantum 1" << endl;
    cout << "vvvvvvvvvvvvvvvvvvvvvvv" << endl;
    rrs1.run();
    cout << "=======================" << endl;
    cout << "Round Robin, Quantum 3" << endl;
    cout << "vvvvvvvvvvvvvvvvvvvvvvv" << endl;
    rrs3.run();
    cout << "=======================" << endl;
    cout << "Shortest Remaining Time" << endl;
    cout << "vvvvvvvvvvvvvvvvvvvvvvv" << endl;
    srt.run();
    cout << "==========================" << endl;
    cout << "Highest Response Time Next" << endl;
    cout << "vvvvvvvvvvvvvvvvvvvvvvvvvv" << endl;
    hrrn.run();

    return 0;
}