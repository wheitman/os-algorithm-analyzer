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
            cout << proc.label + " " + to_string(proc.arrivalTime) << endl;
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

class FcfsScheduler : public Scheduler
{
public:
    FcfsScheduler() : Scheduler()
    {
        printProcesses();
    }

    void addProcess(Process p)
    {
        cout << "Adding process " << p.label << endl;
        processes.push_back(p);
        std::sort(processes.begin(), processes.end(), [](Process a, Process b)
                  { return a.arrivalTime < b.arrivalTime; });
    }

    // Find the process with the smallest arrival time and remove it
    void run()
    {
        auto it = processes.begin();
        while (it != processes.end())
        {
            cout << "Running process " + it->label + " until completion. ";
            processes.erase(it);
            cout << "Waiting processes: [";
            for (Process p : processes)
            {
                cout << p.label << " ";
            }
            cout << "]" << endl;
        }
        for (Process proc : processes)
        {
        }
    }

private:
};

int main()
{
    FcfsScheduler s;
    s.addProcess(Process{"A", 10.0, 6.0, 1.0, 1.0});
    s.addProcess(Process{"B", 2.0, 12.0, 2.0, 2.0});
    s.addProcess(Process{"C", 4.0, 8.0, 1.0, 1.0});

    s.printProcesses();
    s.run();

    return 0;
}