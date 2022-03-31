/*
Noah Flores
z1861588
CSCI 480-1
I certify that this is my own work and where appropriate an extension
of the starter code provided for the assignment.
*/
#include <iostream>
#include "unistd.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <string>
#include <cstring>
#include <vector>
#include <fcntl.h>
#include <queue>
#include <deque>
#include <fstream>
#include <ctime>

using namespace std;

class process
{
public:
    string ProcessName;                // Name of process
    int ProcessID;                     // pid of process
    vector<pair<string, int>> History; // Stores the instructions of the process
    uint32_t Sub = 0;                  // index of the history vector
    int CPUTimer = 0;                  // The correct CPU time
    int ITimer = 0;                    // The corrent Input time
    int OTimer = 0;                    // The corrent Output time
    int CPUTotal = 0;                  // Total CPU time
    int ITotal = 0;                    // Total Input time
    int OTotal = 0;                    // Total Output time
    int CPUCount = 0;                  // Number of times on the CPU
    int ICount = 0;                    // Number of times on Input
    int OCount = 0;                    // Number of times on Output
    int Arrivetime = 0;
};
/**
 * Prints out details of a process when it terminates
 *
 * Prints out all the details when a process terminates, with included
 * CPU burst, I/O burst, Totals time on CPU, I/O and total wait time
 *
 * @param p Current Process being printed out
 * @param timer The timer of the scheduler
 *
 * @return The amount of time the process was waiting
 *
 ********************************************************************************/
int printProcesssum(process *&p, int timer)
{
    timer++; // the process terminates on the next tick
    string Cbust = (p->CPUCount <= 1) ? "burst" : "bursts";
    string Ibust = (p->ICount != 1) ? "bursts" : "burst";
    string Obust = (p->OCount != 1) ? "bursts" : "burst";
    int Timewait = (timer - (p->CPUTotal + p->ITotal + p->OTotal + p->Arrivetime)); // The time waiting of the process
    cout << "\nProcess " << p->ProcessID << " has ended.\n"
         << "Name:  " << p->ProcessName << endl
         << "Started at time " << p->Arrivetime << " and ended at time " << timer << endl
         << "Total CPU time = " << p->CPUTotal << " in " << p->CPUCount << " CPU " << Cbust << endl
         << "Total Input time =  " << p->ITotal << " in " << p->ICount << " Input " << Ibust << endl
         << "Total Output time =  " << p->OTotal << " in " << p->OCount << " Output " << Obust << endl
         << "Time spent in waiting:  " << Timewait << endl
         << endl;
    return Timewait;
}
/**
 * Prints out the stats of what queues and whats in the CPU,Input,and Output queues
 *
 * Prints out the stats of what queues and whats in the CPU,Input,and Output queues,
 * and prints out what process is active on the CPU, and I/O.
 *
 * @param Entry The Entry queue
 * @param Ready The Ready queue
 * @param Input The Input queue
 * @param Output The Output queue
 * @param timer The scheduler timer
 * @param Active The process active on the CPU
 * @param IActive The process active on the Input
 * @param OActive The process active on the Output
 *
 ********************************************************************************/
void printStatus(deque<process *> Entry, deque<process *> Ready, deque<process *> Input, deque<process *> Output, int timer, int Active, int IActive, int OActive)
{
    cout << "Active is " << Active << endl
         << "IActive is " << IActive << endl
         << "OActive is " << OActive << endl
         << "Contents of the Entry Queue\n"; // print out whats active on CPU and I/O
    if (Entry.size() == 0)                   // if Queue is empty print empty
    {
        cout << "(Empty)";
    }
    else
    {
        // loop though queue and print out
        for (size_t i = 0; i < Entry.size(); i++)
        {
            cout << Entry[i]->ProcessID << "    ";
        }
    }
    cout << "\nContents of the Ready Queue:\n";
    if (Ready.size() == 0) // if Queue is empty print empty
    {
        cout << "(Empty)";
    }
    else
    {
        // loop though queue and print out
        for (size_t i = 0; i < Ready.size(); i++)
        {
            cout << Ready[i]->ProcessID << "    ";
        }
    }
    cout << "\nContents of the Input Queue:\n";
    if (Input.size() == 0) // if Queue is empty print empty
    {
        cout << "(Empty)";
    }
    else
    {
        // loop though queue and print out
        for (size_t i = 0; i < Input.size(); i++)
        {
            cout << Input[i]->ProcessID << "    ";
        }
    }
    cout << "\nContents of the Output Queue:\n";
    if (Output.size() == 0) // if Queue is empty print empty
    {
        cout << "(Empty)";
    }
    else
    {
        // loop though queue and print out
        for (size_t i = 0; i < Output.size(); i++)
        {
            cout << Output[i]->ProcessID << "    ";
        }
    }
    cout << endl;
}