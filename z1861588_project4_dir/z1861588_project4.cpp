/*
Noah Flores
z1861588
CSCI 480-1
I certify that this is my own work and where appropriate an extension
of the starter code provided for the assignment.
*/

/***************************************************************
This Program will simulates a CPU scheduler. The program will
read in from a file and add process to a Ready queue. If the
process read in from the file are suppose to arrive later then
move them into the ready queue. The program will loop until the
all process have terminated. Each time though the loop the timer
tick a ms. Each process will be in a queue or be active on
the CPU or I/O. Only 5 process can be active at a time.
***************************************************************/
#include "z1861588_project4.h"

int main(int argc, char **argv)
{
    const int MAX_TIME = 500; // max number the timer can be at before exiting
    const int IN_USE = 5;     // The number of process that can be in the use.
    const int HOW_OFTEN = 25; // How often the status of the queues should be printed
    deque<process *> Entry;   // Were all process before the ready queue
    deque<process *> Ready;   // Process ready to go onto the CPU
    deque<process *> Input;   // Process ready to go onto the Input
    deque<process *> Output;  // Process ready to go onto the Output
    int CPUIdle = 0;          // The amount of time the CPU was idle
    int Pid = 100;            // The pid of each process
    int Timer = 0;            // Timer of the scheduler
    int Active = 0;           // What process is active on the CPU
    int IActive = 0;          // What process is active on the Input
    int OActive = 0;          // What process is active on the Output
    int Arrival = 0;          // The arrival time of the process
    int Time = 0;             // The time a process runs on the CPU or I/O
    int ActInUse = 0;         // Keeps track of the number of process cant be greater than 5
    int TotalProcesses = 0;   // The total number of process that ran
    int TotalProcesswait = 0; // The total wait time of all the process
    int Quantum = 5;          // THe quantum for RR
    int QBurst = 0;           // Keep track of the quantum burst
    string Command;           // Used to store the next action of a process
    string InputFile;             // Used for reading input file
    bool CPUbusy = false;     // Flag for if CPU is in use
    bool Ibusy = false;       // Flag for if Input is in use
    bool Obusy = false;       // Flag for if Output is in use
    bool CPUIdleFlag = false; // Flag for if the CPU is idle
    bool CPUdone = false;     // Flag for if the CPU is done
    bool ONextTick = false;   // Used to stop process from going from CPU to I/O on the same tick
    bool INextTick = false;   // Used to stop process from going from CPU to I/O on the same tick
    process *p;               // Used to read from Ready queue
    process *cp;              // Used to point to the process on the CPU
    process *ip;              // Used to point to the process on the Input
    process *op;              // Used to point to the process on the Output
    process *p2;              // Used to move from entry to ready queue

    // Checks if File was entered on command line
    if (optind >= argc)
    {
        cout << "No File!" << endl;
        exit(-1); // exit
    }
    if (argv[2] != NULL) // Check if user enters quantum
    {
        if (atoi(argv[2]) >= 0) // checks if user enters a value that is >= 0
        {
            Quantum = atoi(argv[2]);
        }
    }
    cout << "Simulation of CPU Scheduling\n\n";
    ifstream myfile;
    myfile.open(argv[1]); // open file
    if (myfile.fail())    // file cant be opened
    {
        cerr << "Failed error: " << argv[1] << " cant be opened!\n";
        exit(-1);
    }

    myfile >> InputFile; // Read the name of process
    while (!myfile.eof())
    {
        // if process is named STOPHERE then break out of reading loop
        if (InputFile == "STOPHERE")
        {
            break;
        }
        process pin;              // Create class object
        pin.ProcessName = InputFile;  // Set process name
        Pid++;                    // increse PID
        pin.ProcessID = Pid;      // Set PID of process
        myfile >> Arrival;        // Get arrival time
        pin.Arrivetime = Arrival; // set arrival time
        myfile >> InputFile;
        // Loop instructions and store them into vector
        for (int i = 0; i < 10; i++)
        {
            myfile >> Time;
            pin.History.push_back(make_pair(InputFile, Time)); // set command and length of command
            myfile >> InputFile;
        }
        // if process is arrival time is greater than 0 && there are <= 5 process in the Ready queue
        if (Timer >= Arrival && Ready.size() <= IN_USE)
        {
            cout << "\nProcess " << pin.ProcessID << " moved from the Entry Queue into the Ready Queue at time " << Timer << endl
                 << endl;
            Ready.push_back(new process(pin)); // Create pointer to class and push onto queue
            ActInUse++;                        // The number of process in the Ready queue
        }
        else // If Entry queue is full and Arrival time is greater than 0
        {
            Entry.push_back(new process(pin));
        }
    }
    myfile.close(); // close file
    p = nullptr;
    ip = nullptr;
    op = nullptr;
    // Loop until timer is less than max time (500ms)
    while (Timer != MAX_TIME)
    {
        // if all queues are empty and CPU and I/O are not active
        if (Entry.empty() && Ready.empty() && Input.empty() && Output.empty() && p == nullptr && ip == nullptr && op == nullptr && cp == nullptr)
        {
            break;
        }
        // if the CPU ever idles of a tick
        if (Active == 0 && CPUIdleFlag == false && Ready.empty() && Entry.empty())
        {
            cout << "\nAt time " << Timer << ", Active is 0, so we have idle time for a while\n\n";
            CPUIdleFlag = true;
        }
        // if Ready queue is empty and nothing is running on the CPU
        if (Ready.empty() && CPUbusy == false)
        {
            // Check if empty queue is not empty
            if (!Entry.empty())
            {
                // Loop while active process is less then 6
                while (ActInUse < IN_USE)
                {
                    // once Entry is empty break
                    if (Entry.empty())
                    {
                        break;
                    }
                    p2 = Entry.front(); // Get front of entry queue
                    Entry.pop_front();  // Pop it
                    // If process arival time is less than timer
                    if (p2->Arrivetime <= Timer)
                    {
                        cout << "\nProcess " << p2->ProcessID << " moved from the Entry Queue into the Ready Queue at time " << Timer << endl
                             << endl;
                        ActInUse++;             // increase number active process count
                        p2->Arrivetime = Timer; // Timer is set to arrival time into the ready queue
                        Ready.push_back(p2);    // Move to ready queue
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
        // Timer is 0 or is % to 25 than print out queue status
        if (Timer == 0 || Timer % HOW_OFTEN == 0)
        {
            cout << "\nStatus at time " << Timer << endl;
            printStatus(Entry, Ready, Input, Output, Timer, Active, IActive, OActive);
            cout << endl;
        }
        // if Ready queue is not empty and CPU is not busy and P is nullpointer
        if (!Ready.empty() && p == nullptr && CPUbusy == false)
        {
            p = Ready.front(); // Get the process on the front of the ready queue
            Ready.pop_front(); // pop off front
        }
        // CP and P pointer are not null
        if (p != nullptr || cp != nullptr)
        {
            // If the CPU is not bust
            if (!CPUbusy)
            {
                cp = p;
                p = nullptr;
                CPUbusy = true;         // Set the CPU to busy
                CPUIdleFlag = false;    // Set the cpu not busy flag
                Active = cp->ProcessID; // Set PID to Active
            }
            // If the qburst timer is less than quatum AND CPUtimer does not match max time of the CPU burst
            if (QBurst <= Quantum && cp->CPUTimer != cp->History[cp->Sub].second)
            {
                cp->CPUTimer++;
                QBurst++;
            }
            // If CPUtimer is equal to Max burst
            if (cp->CPUTimer == cp->History[cp->Sub].second)
            {
                cp->CPUTotal += cp->History[cp->Sub].second; // increase Total CPU burst
                cp->Sub++;                                   // increase index
                Command = cp->History[cp->Sub].first;        // get next command
                cp->CPUTimer = 0;
                cp->CPUCount++;  // add to on CPU count
                CPUbusy = false; // set CPU to not busy
                Active = 0;
                QBurst = 0;
                CPUdone = true;
                // if command is Input
                if (Command == "I")
                {
                    // IF Input is empty and Input is not busy
                    if (Input.empty() && ip == nullptr)
                    {
                        // set cp pointer to ip
                        ip = cp;
                        IActive = ip->ProcessID; // Set Ip to active
                        INextTick = true;        // sets so Input cant tick on the same tick
                        Ibusy = true;
                        cp = nullptr;
                    }
                    else
                    {
                        Input.push_back(cp); // Else push onto Input queue
                        cp = nullptr;
                    }
                }
                // IF Output is empty and Output is not busy
                else if (Command == "O")
                {
                    if (Output.empty() && op == nullptr)
                    {
                        op = cp;
                        OActive = op->ProcessID; // set output to active
                        Obusy = true;            // set output to busy
                        ONextTick = true;
                        cp = nullptr;
                    }
                    else
                    {
                        Output.push_back(cp);
                        cp = nullptr;
                    }
                }
                else if (Command == "N")
                {
                    TotalProcesswait += printProcesssum(cp, Timer); // If N terminate the process
                    delete cp;                                      // delete pointer
                    cp = nullptr;
                    TotalProcesses++; // add to the total process count
                    ActInUse--;       // subtract from the process in use count
                }
            }
            else if (cp->CPUTimer % Quantum == 0) // if quantum is hit but the process still has more CPU time
            {
                if (Ready.size() == 0 && IActive == 0 && OActive == 0) // if Ready queue is empty
                {
                    QBurst = 0; // set to 0 and burst again
                }
                else
                {
                    Ready.push_back(cp); // else push onto Ready queue
                    CPUbusy = false;
                    cp = nullptr;
                    Active = 0;
                }

                QBurst = 0;
            }
        }

        if (!Input.empty() || ip != nullptr) // if the Input queue is not empty or input is active
        {
            if (Ibusy == false) // if input is not busy and input is set to NULL
            {
                if ((Input.size() > 1 || CPUdone == false) && INextTick == false) // if input is greater than 1 or CPU has not finished on this tick and INext is false
                {
                    ip = Input.front(); // set ip to the front of the queue
                    Ibusy = true;       // set input to bust
                    IActive = ip->ProcessID;
                    Input.pop_front();
                }
            }
            if (ip != nullptr && INextTick == false) // if Input is active and Inext is false
            {
                // if active input timer has not hit the its burst time
                if (ip->ITimer < ip->History[ip->Sub].second - 1)
                {
                    ip->ITimer++;
                }
                else
                {
                    ip->ITotal += ip->History[ip->Sub].second; // else add total
                    ip->Sub++;                                 // increase index
                    ip->ITimer = 0;                            // reset its timer
                    ip->ICount++;                              // Increase on Input count
                    IActive = 0;
                    Command = ip->History[ip->Sub].first; // get next command
                    Ibusy = false;
                    if (Command == "C") // If CPU
                    {
                        Ready.push_back(ip);
                    }
                    else if (Command == "O") // if Output
                    {
                        Output.push_back(ip);
                    }
                    else if (Command == "N") // if time to terminate
                    {
                        TotalProcesswait += printProcesssum(ip, Timer);
                        delete ip;
                        ip = nullptr;
                        TotalProcesses++;
                        ActInUse--;
                    }
                    ip = nullptr;
                }
            }
        }
        if (!Output.empty() || op != nullptr) // if the Output queue is not empty or output is active
        {
            if (Obusy == false) // Output is not busy
            {
                if ((Output.size() > 1 || CPUdone == false) && ONextTick == false) // if output is greater than 1 or CPU has not finished on this tick and ONext is false
                {
                    op = Output.front(); // set output to active
                    Obusy = true;
                    OActive = op->ProcessID;
                    Output.pop_front();
                }
            }
            if (op != nullptr && ONextTick == false) // if output is active and Onext is false
            {
                // if active ouput timer has not hit the its burst time
                if (op->OTimer < op->History[op->Sub].second - 1)
                {
                    op->OTimer++;
                }
                else
                {
                    op->OTotal += op->History[op->Sub].second; // add to output total
                    op->Sub++;                                 // increase index
                    op->OCount++;                              // increase output amount
                    op->OTimer = 0;
                    OActive = 0;
                    Command = op->History[op->Sub].first; // get next command
                    Obusy = false;
                    if (Command == "C") // If CPU
                    {
                        Ready.push_back(op);
                    }
                    else if (Command == "I") // If Input
                    {
                        Input.push_back(op);
                    }
                    else if (Command == "N") // If time to terminate
                    {
                        TotalProcesswait += printProcesssum(op, Timer);
                        delete op;
                        op = nullptr;
                        TotalProcesses++;
                        ActInUse--;
                    }
                    op = nullptr;
                }
            }
        }
        CPUdone = false;
        INextTick = false;
        ONextTick = false;
        if (CPUIdleFlag == true) // If the CPU is idle this tick add to the CPU idle count
        {
            CPUIdle++;
        }
        Timer++; // increase timer
    }

    TotalProcesswait = TotalProcesswait / TotalProcesses; // Get the average process wait time
    // print out final run time stats
    cout << "\nThe run has ended.\nThe final value of the timer was:  " << Timer << endl
         << "The amount of time spent idle was:  " << CPUIdle << endl
         << "Number of terminated processes = " << TotalProcesses << endl
         << "Average waiting time for all terminated processes = " << TotalProcesswait << endl;
    printStatus(Entry, Ready, Input, Output, Timer, Active, IActive, OActive); // print out all queue

    return 0; // exit
}
