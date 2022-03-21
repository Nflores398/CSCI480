/*
Noah Flores
z1861588
CSCI 480-1
I certify that this is my own work and where appropriate an extension
of the starter code provided for the assignment.
*/

/***************************************************************
This program will simulate a shell. User will input a command
and the parnet will fork() and create a child to exec the command.
User can pull up a command history, rerun command with !!, and
write to a file with > and read in a file with <.
***************************************************************/

#include <iostream>
#include "unistd.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <string>
#include <cstring>
#include <vector>
#include <fcntl.h>

using namespace std;

#define MAX_LINE 1024 // sets the max amount of input

int main(int argc, char **argv)
{
    char *args[1024];       // used for exec command
    char input[1024];       // user input
    char *token;            // The input array broken down
    int status;             // Used to get status of waitpid
    string temp;            // turns input array into a string
    vector<string> history; // vector used to store command history
    int i = 1;              // Used keep count of amt of elements in the command
    pid_t pid;              // store pid
    int fd1;                // used to store file descripter

    cout << "mysh>";

    while (fgets(input, MAX_LINE, stdin) != NULL) // loop until user enters "quit" or "q"
    {
        input[strlen(input) - 1] = 0; // remove null term of the input
        if (strcmp(input, "") == 0)   // if user does not enter input
        {
            cout << "mysh>"; // prompt user
            continue;        // Loop again
        }
        temp = input; // turn input char into string

        // store the all commands into the history vector if not !! or qm
        if (strcmp(input, "q") != 0 && strcmp(input, "!!") != 0)
            history.push_back(temp);

        // exec last command
        if (strcmp(input, "!!") == 0)
        {
            if (history.empty()) // history is empty
            {
                cout << "No commands in history!\n";
                cout << "mysh>";
                continue; // return to the top of the loop
            }
            temp = history.back();       // get the last elem of vector
            history.push_back(temp);     // push command onto vector so it can be in the history
            strcpy(input, temp.c_str()); // sent command as input
        }
        // q or quit then exit program
        if (strcmp(input, "q") == 0 || strcmp(input, "quit") == 0)
        {
            exit(0);
        }
        // if input is history then print history vector
        else if (strcmp(input, "history") == 0)
        {
            for (size_t i = 0; i < history.size(); i++)
            {
                cout << i << " " << history.at(i) << "\n";
            }
        }

        if (strcmp(input, "q") != 0 && strcmp(input, "history") != 0 && strcmp(input, "!!") != 0)
        {
            if ((pid = fork()) < 0) // fork() if error then exit
            {
                cerr << "Fork error\n";
                exit(-1);
            }
            else if (pid == 0)
            {
                token = strtok(input, " "); // split input by space
                if (token != NULL)
                {
                    args[0] = token; // place command into 0
                }
                while (token != NULL)
                {
                    token = strtok(NULL, " ");
                    args[i] = token; // place the other parts into args[i]
                    i++;
                }
                if (i >= 3) // if i greater than or equal to 3
                {
                    if (strcmp(args[i - 3], "<") == 0) // if < then its input
                    {
                        fd1 = open(args[i - 2], O_RDWR, 0644); // open file for input
                        if (dup2(fd1, STDIN_FILENO) == -1)
                        {
                            cerr << "error dup2!" << endl;
                            exit(-1);
                        }
                        args[i - 3] = NULL; // remove the <
                        args[i - 2] = NULL; // remove file name
                    }
                    else if (strcmp(args[i - 3], ">") == 0) //  if > then its output
                    {
                        fd1 = open(args[i - 2], O_CREAT | O_WRONLY | O_TRUNC, 0644); // open or create for output
                        if (dup2(fd1, STDOUT_FILENO) == -1)
                        {
                            cerr << "error dup2!" << endl;
                            exit(-1);
                        }
                        args[i - 3] = NULL; // remove the >
                        args[i - 2] = NULL; // remove file name
                    }
                }

                if (execvp(args[0], args) == -1) // exec command
                {
                    cout << "Couldn't execute the command " << temp << endl; // if command fails
                    exit(-1);
                }
            }
            if (waitpid(pid, &status, 0) == -1) // wait for child procress to finish
            {
                cerr << "waitpid error" << endl;
            }
        }

        cout << "mysh>"; // print and wait for user input again
    }

    return 0;
}