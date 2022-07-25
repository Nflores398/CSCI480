/*
Noah Flores
z1861588
CSCI 480-1
I certify that this is my own work and where appropriate an extension
of the starter code provided for the assignment.
*/

/***************************************************************
This program will have one readers and x amount of writers
the writer will signal the reader and reader will signal writer
untill the string is empty. Then program will delete threads and
exit when string is empty. The program should go writer than
reader until the string empty.
***************************************************************/
#include <iostream>
#include <string>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

using namespace std;
// string that will be removed by the writers.
string Mystring = "All work and no play makes Jack a dull boy.";
// The amound of readers
pthread_t tid[50];
pthread_t tid2[50];
sem_t s1;
sem_t s2;
int writercount = 0;

/**
 * Reader will wait for s2 to signal
 *
 * Reader will wait for s2 to signal then reader will read the string and signal s1
 * so writer can go. When string is ending the Reader will signal the rest of the
 * writers.
 *
 * @param param passes the id of the thread.
 *
 ********************************************************************************/
void *Readers(void *param)
{
    int RID = *(int *)param;
    while (!Mystring.empty() || Mystring == "")
    {
        sem_wait(&s2);        // wait on s2
        if (Mystring.empty()) // break if string is empty
        {
            break;
        }

        printf("reader %i is reading ... content : %s \n", RID, Mystring.c_str());
        fflush(stdout);
        sem_post(&s1); // post s1
    }
    printf("reader %i is Exiting \n", RID);
    sem_post(&s1); // post s1
    printf("There are still %i writers waiting on the semaphore... \n", writercount - 1);
    for (int i = 0; i < writercount - 1; i++) // loop to free the other writer threads waiting
    {
        sem_post(&s1);
    }
    pthread_exit(0);
}
/**
 * Writer will go first and signal s1 and wait on s2
 *
 * A writer will wait s1 and pop the string and then signal s2. writer will
 * wait on s1 to be free which is done by reader after it finishes reading.
 *
 * @param param passes the id of the thread.
 *
 ********************************************************************************/
void *Writers(void *param)
{
    writercount++; // add to the total writer count
    int WID = *(int *)param;
    while (!Mystring.empty() || Mystring == "") // loop untill string is empty
    {
        sem_wait(&s1);        // wait on s1
        if (Mystring.empty()) // break if string is empty
        {
            break;
        }

        printf("writer %i is writing...\n", WID); // print that the writer is going
        fflush(stdout);
        Mystring.pop_back(); // pop off end of string

        sem_post(&s2); // post s2
        if (Mystring.empty())
        {
            break;
        }
        sleep(1); // sleep for 1
    }
    printf("Writer %i is Exiting \n", WID); // when writer is exiting
    pthread_exit(0);                        // writer exited
}

int main(int argc, char **argv)
{
    int numReader = 0; // number readers
    int numWriter = 0; // number writers
    int RID[32];       // array ints for Reader ids
    int WID[32];       // array ints for writers ids
    // if user passes in two argv
    if (argv[1] > 0 && argv[2] > 0 && atoi(argv[1]) > 0)
    {
        numReader = 1;             // set readers to 1
        numWriter = atoi(argv[2]); // set num of writers based of user input
    }
    else // else display usage and print.
    {
        cout << "Please Provide Number of Readers and Writers\n./z1861588_project5 #Readers #Writers\n";
        exit(-1);
    }
    cout << "*** Reader-Writer Problem Simulation ***\nNumber of reader threads: " << numReader << "\nNumber of writer threads: " << numWriter << endl;
    pthread_attr_t attr;         // create threads attributes
    pthread_attr_init(&attr);    // inti attributes
    if (sem_init(&s1, 0, 1) < 0) // error check semaphore
    {
        perror("sem failed to init!");
        exit(-1);
    }
    if (sem_init(&s2, 0, 0) < 0) // error check semaphore
    {
        perror("sem failed to init!");
        exit(-1);
    }
    // Create reader threads
    for (int i = 0; i < numReader; i++)
    {
        RID[i] = i;
        if (pthread_create(&tid[i], &attr, Readers, &RID[i]) != 0)
        {
            perror("Thread Creation error!");
            exit(-1);
        }
    }
    sleep(.1); // sleep for 1ms
    // create writer threads
    for (int i = 0; i < numWriter; i++)
    {
        WID[i] = i;
        if (pthread_create(&tid2[i], &attr, Writers, &WID[i]) != 0)
        {
            perror("Thread Creation error!");
            exit(-1);
        }
    }
    // join reader threads
    for (int i = 0; i < numReader; i++)
    {
        if (pthread_join(tid[i], NULL) != 0)
        {
            perror("Thread joining error!");
            exit(-1);
        }
    }
    // join writers threads.
    for (int i = 0; i < numWriter; i++)
    {
        if (pthread_join(tid2[i], NULL) != 0)
        {
            perror("Thread joining error!");
            exit(-1);
        }
    }
    // destroy semaphores
    sem_destroy(&s1);
    sem_destroy(&s2);
    cout << "All threads are done.\nResources cleaned up.\n";
    return 0;
}