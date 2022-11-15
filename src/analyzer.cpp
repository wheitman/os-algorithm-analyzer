#include <algorithm> // std::sort
#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct Process
{
    string label;
    float arrivalTime, serviceTime, ioTime, ioActivity;
};

class Scheduler
{
public:
    Scheduler()
    {
        cout << "Created a scheduler!" << endl;
    }

    void addProcess(Process p)
    {
        cout << "Adding process " << p.label << endl;
        processes.push_back(p);
    }

    void printProcesses()
    {
        for (Process proc : processes)
        {
            cout << proc.label << endl;
        }
    }

    void run()
    {
        while (processes.size() > 0)
        {
            step();
        }
    }

    void step()
    {
    }

    vector<Process> processes;
};

class RoundRobin : public Scheduler
{
public:

    RoundRobin() : Scheduler() {
        sort(processes.begin(), processes.end(), &_arrivalTimeSorter);
        printProcesses();
    }
    // Find the process with the smallest arrival time and execute it
    void step()
    {
        int minIdx = 0;

        for (Process proc : processes)
        {
            
        }
    }
private:
    bool _arrivalTimeSorter(Process const& lhs, Process const& rhs) {
        return lhs.arrivalTime < rhs.arrivalTime;
    }
};

int main()
{
    RoundRobin s;
    s.addProcess(Process{"A", 0.0, 6.0, 1.0, 1.0});
    s.addProcess(Process{"B", 2.0, 12.0, 2.0, 2.0});
    s.addProcess(Process{"C", 4.0, 8.0, 1.0, 1.0});
    // Process b{2.0, 12.0, 1.0, 1.0};
    // Process c{4.0, 8.0, 1.0, 1.0};
    // Process d{6.0, 10.0, 1.0, 1.0};
    // Process e{8.0, 4.0, 1.0, 1.0};

    s.printProcesses();

    return 0;
}