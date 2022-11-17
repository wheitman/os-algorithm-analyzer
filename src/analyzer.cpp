#include <algorithm> // std::sort
#include <cstdio>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Process
{
public:
    Process(char label, float arrivalTime, float serviceTime, float totalIo, float ioBlock)
    {
        this->label = label;
        this->arrivalTime = arrivalTime;
        remainingService = serviceTime;
        remainingIo = totalIo;
        remainingIoBlock = ioBlock;
        this->ioBlock = ioBlock;

        if (ioBlock == 0)
            serviceBlock = 0;
        else
            serviceBlock = serviceTime / ((totalIo / ioBlock) + 1);
    }
    string label;
    int arrivalTime, remainingService, remainingIo, ioBlock, serviceBlock, remainingIoBlock;
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
        cout << "Adding process " << p.label << endl;
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

        printf(
            "%2i|%5s|%5s|%1s|%5s\n",
            time,
            readyQueueStr.c_str(),
            ioQueueStr.c_str(),
            runningProcess.empty() ? "" : runningProcess.front().label.c_str(),
            doneQueueStr.c_str());
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
        while (readyQueue.size() > 0)
        {
            step();
        }
    }

    void step()
    {
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
        while (time < 50)
        {
            // 1. Check for new additions to the Ready queue
            //    from the Future queue
            auto it = futureQueue.begin();
            while (it != futureQueue.end())
            {
                // cout << it->label << endl;
                if (it->arrivalTime <= time)
                {
                    if (runningProcess.empty())
                    {
                        runningProcess.push_back(*it);
                        blockStartTime = time;
                    }
                    else
                    {
                        readyQueue.push_back(*it);
                    }
                    futureQueue.erase(it);
                }
                if (futureQueue.empty())
                    break;
                else
                    it++;
            }

            // Is the running process done?
            if (!runningProcess.empty() && runningProcess.front().remainingService == 0)
            {
                // Add currently running process to "done"
                doneQueue.push_back(runningProcess.front());
                // Clear out currently running process
                runningProcess.clear();
                // Add the ready queue's front process to "running"
                if (!readyQueue.empty())
                {
                    runningProcess.push_back(readyQueue.front());
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
                // Move running process to I/O queue
                ioQueue.push_back(runningProcess.front());
                runningProcess.clear();
                if (!readyQueue.empty())
                {
                    runningProcess.push_back(readyQueue.front());
                    blockStartTime = time;
                    readyQueue.erase(readyQueue.begin());
                }
            }

            // Look through IO queue processes.
            // Decrement IO time on front process.
            // If remaining % io_block == 0, pop
            if (!ioQueue.empty()) {
                
                if (ioQueue.front().remainingIoBlock == 0)
                {
                    ioQueue.front().remainingIoBlock = ioQueue.front().ioBlock;
                    if (runningProcess.empty())
                        runningProcess.push_back(ioQueue.front());
                    else   
                        readyQueue.insert(readyQueue.begin(), ioQueue.front());
                    ioQueue.erase(ioQueue.begin());
                } else 
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
    }

private:
};

int main()
{
    FcfsScheduler s;

    s.addProcess(Process('A', 0, 6, 1, 1));
    s.addProcess(Process('B', 2, 12, 2, 2));
    s.addProcess(Process('C', 4, 8, 1, 1));
    s.addProcess(Process('D', 6, 10, 0, 0));
    s.addProcess(Process('E', 8, 4, 2, 2));
    // s.addProcess(Process{"B", 2.0, 12.0, 2.0, 2.0});
    // s.addProcess(Process{"C", 4.0, 8.0, 1.0, 1.0});
    s.run();

    return 0;
}