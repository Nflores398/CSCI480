/*
Noah Flores
z1861588
CSCI 480-1
I certify that this is my own work and where appropriate an extension
of the starter code provided for the assignment.
*/

/***************************************************************
This program will fork() 4 times creating 4 different procress.
Parent procress will print out ps for the 4 running procress.
Then wait for all other children procress to finish first.
***************************************************************/
#include <iostream>
#include "unistd.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <string>
#include <cstring>

int main()
{
    pid_t cpid1, // stores pid of child
        cpid2,   // stores pid of intermediate parent
        gcpid;   // stores pid of grand child

    std::string cmd = "";

    cpid1 = fork(); // fork and create child procress
    if (cpid1 < 0)  // if an error has occured when forking
    {
        perror("forking error");
        exit(-1);
    }
    if (cpid1 != 0) // prevents child procress from forking
    {
        cpid2 = fork(); // parent procress will fork again creating IP
        if (cpid2 < 0)  // if an error has occured when forking
        {
            perror("forking error");
            exit(-1);
        }
    }

    if (cpid1 == 0) // the child procress will display the following
    {
        std::cout << "CHILD: My PID is " << getpid() << ", my parent's PID is " << getppid() << ".\n";
        sleep(3); // child will sleep for 3 secs
        std::cout << "CHILD: " << getpid() << " is awake.\n";
    }
    else if (cpid2 == 0) // the child procress will display the following
    {
        gcpid = fork(); // IP fork again to create GChild. Store gcpid for IP to use
        if (gcpid < 0)  // if an error has occured when forking
        {
            perror("forking error");
            exit(-1);
        }
        if (gcpid == 0) // GC displays the following
        {
            std::cout << "GRAND CHILD: My PID is " << getpid() << ", my parent's PID is " << getppid() << ".\n";
            sleep(3); // GC sleeps for 3 secs
            std::cout << "GRAND CHILD: " << getpid() << " is awake.\n";
        }
        else // IP will display the following
        {
            std::cout << "INTERMEDIATE PARENT: My PID is " << getpid() << ", my parent's PID is " << getppid() << ", my child is " << gcpid << ".\n";
            sleep(3); // IP will sleep for 3 sec
            std::cout << "INTERMEDIATE PARENT: " << getpid() << " is awake.\n";
            wait(0); // IP will wait for its child(GC) to finish first
        }
    }
    else
    {
        std::cout << "PARENT: My PID is " << getpid() << ", my parent's PID is " << getppid() << " my children are " << cpid1 << ", " << cpid2 << ".\n";
        cmd = "/bin/ps -f --ppid ";      // store system command in cmd string
        cmd += std::to_string(getpid()); // add parent pid
        cmd += ",";
        cmd += std::to_string(getppid()); // add parent's parents pid
        cmd += ",";
        cmd += std::to_string(cpid1); // add Child pid
        cmd += ",";
        cmd += std::to_string(cpid2); // add IP pid
        std::cout << "PARENT: Issuing command: " << cmd << std::endl;
        system(cmd.c_str());                                       // run system command to show all the procress
        waitpid(cpid1, 0, 0);                                      // wait for child to finish
        waitpid(cpid2, 0, 0);                                      // wait for IP to finish
        std::cout << "PARENT: Children processes are finished.\n"; // parent program finishes
    }

    return 0;
}