/*
Noah Flores
z1861588
CSCI 480-1
I certify that this is my own work and where appropriate an extension
of the starter code provided for the assignment.
*/

/***************************************************************
This program will create x amount of readers and writer threads
based of user input. The reader thread will read the string and
then the writers will go afer the readers are done. The program
will delete all threads exit when the string empty.
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

string Mystring = "All work and no play makes Jack a dull boy.";
int readcount = 0;  // The count of the readers threads that are reading.
pthread_t tid[50];  // array of reader threads
pthread_t tid2[50]; // array of writer threads
sem_t rw_sem;       // reading and writing semaphore
sem_t cs_sym;       // reader critical section semaphore

/**
 * Reader will print out the string
 *
 * Reader will wait the cs_sym and then increase readcount then post it.
 * the Reader than will print the string and then wait the cs_sym again but
 * decrease the readcount.
 *
 * @param param passes the id of the thread.
 *
 ********************************************************************************/
void *Readers(void *param)
{
    int RID = *(int *)param; // ID of reader thread
    // loop until string is empty
    while (!Mystring.empty() || Mystring == "")
    {
        if (Mystring.empty()) // if empty signal writers so they can also exit
        {
            sem_post(&rw_sem);
            break;
        }
        sem_wait(&cs_sym); // wait for critical section
        readcount++;       // increase active reader count
        printf("read_count increments to: %i \n", readcount);
        fflush(stdout);
        if (readcount == 1) // if reader count is = 1 wait rw_sem
        {
            sem_wait(&rw_sem);
        }
        sem_post(&cs_sym);                                                         // post critical section
        printf("reader %i is reading ... content : %s \n", RID, Mystring.c_str()); // print out string
        fflush(stdout);
        sem_wait(&cs_sym); // wait for critical section
        readcount--;       // decrease active reader count
        printf("read_count decreased to: %i \n", readcount);
        fflush(stdout);
        if (readcount == 0)
        {
            sem_post(&rw_sem);
        }
        sem_post(&cs_sym); // post critical section
        sleep(1);          // sleep thread for 1 sec
    }
    sem_post(&cs_sym); // post cs_sym to free up any waiting readers
    sem_post(&rw_sem); // post rw_sem to free up any waiting Writers/Readers
    printf("reader %i is Exiting \n", RID);
    pthread_exit(0); // exit reader thread
}
/**
 * Writers will pop off of the string
 *
 * Writer waits on the rw_sem and will pop off the back of the string and will
 * post rw_sem when its done. The writer will exit once the string is empty.
 *
 * @param param passes the id of the thread.
 *
 ********************************************************************************/
void *Writers(void *param)
{
    int WID = *(int *)param; // writer ID
                             // loop until string is empty
    while (!Mystring.empty() || Mystring == "")
    {
        sem_wait(&rw_sem);    // wait on rw_sem
        if (Mystring.empty()) // if string is empty
        {
            sem_post(&rw_sem);
            break;
        }

        printf("writer %i is writing...\n", WID);
        fflush(stdout);
        Mystring.pop_back(); // pop back of string

        sem_post(&rw_sem); // post rw_sem
        sleep(1);
    }
    printf("Writer %i is Exiting \n", WID);
    pthread_exit(0); // writer thread exits
}

int main(int argc, char **argv)
{
    int numReader = 0;                                   // number of reader threads to be created
    int numWriter = 0;                                   // number of writer threads to be created
    int RID[32];                                         // Int array for reader ID
    int WID[32];                                         // Int array for writer ID
    if (argv[1] > 0 && argv[2] > 0 && atoi(argv[1]) > 0) // checks for argv
    {
        numReader = atoi(argv[1]); // sets number of reader threads to create
        numWriter = atoi(argv[2]); // sets number of writers threads to create
    }
    else // Prints usage statment if argv isnt provided
    {
        cout << "Please Provide Number of Readers and Writers\n./z1861588_project5 #Readers #Writers\n";
        exit(-1); // exit
    }
    cout << "*** Reader-Writer Problem Simulation ***\nNumber of reader threads: " << numReader << "\nNumber of writer threads: " << numWriter << endl;
    pthread_attr_t attr;      // create attributes
    pthread_attr_init(&attr); // init attributes

    if (sem_init(&rw_sem, 0, 1) != 0) // init semaphores error check
    {
        perror("sem failed to init!");
        exit(-1);
    }
    if (sem_init(&cs_sym, 0, 1) != 0)
    {
        perror("sem failed to init!");
        exit(-1);
    }
    // create reader threads
    for (int i = 0; i < numReader; i++)
    {
        RID[i] = i;
        if (pthread_create(&tid[i], &attr, Readers, &RID[i]) != 0)
        {
            perror("Thread Creation error!");
            exit(-1);
        }
    }
    sleep(.1); // sleep main process for 1ms
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
    // join reader and writer threads
    for (int i = 0; i < numReader; i++)
    {
        if (pthread_join(tid[i], NULL) != 0)
        {
            perror("Thread joining error!");
            exit(-1);
        }
    }
    for (int i = 0; i < numWriter; i++)
    {
        if (pthread_join(tid2[i], NULL) != 0)
        {
            perror("Thread joining error!");
            exit(-1);
        }
    }
    sem_destroy(&cs_sym); // destroy semaphore at the end of the program
    sem_destroy(&rw_sem);
    cout << "All threads are done.\nResources cleaned up.\n";
    return 0;
}