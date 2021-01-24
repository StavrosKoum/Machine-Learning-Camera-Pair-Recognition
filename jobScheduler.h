#ifndef JOBSCHEDULER
#define JOBSCHEDULER

#include <pthread.h>
#include <stdio.h>
#include <stddef.h>
#include <stdbool.h>
#include "logisticRegression.h"

typedef void (*thread_funct)(void *args);

//struct keeping the arguments for each job
typedef struct args{

    //batch start
    int start;

    //batch finish
    int finish;

    int *activeThreads;

    int *J;
    
    //loop (for k;)
    int k;

    logistic_reg *classfier;

    pthread_mutex_t *lock;

}Arguments;



//job struct
typedef struct job{

    //pointer to the function to be executed
    thread_funct func;
    //pointer to the function to be executed
    void *function;
    //pointer to a struct containing arguments
    void *args;

}Job;

//queue node
//contains a job and a pointer to the next node
typedef struct queueNode{

    //pointer to job
    Job *job;
    //pointer to next
    struct queueNode *next;

}queueNode;

//the job Queue
typedef struct Queue{

    //pointer to head
    queueNode *head;

    //pointer to tail
    queueNode *tail;

    //number of elements inside
    int counter;

}Queue;

//job scheduler struct
typedef struct jobScheduler{

    //number of avaiable threads
    int execution_threads;
    
    //FIFO queue to hold the jobs
    Queue *q;

    //array with the ids of the avaiable threads
    pthread_t* tids;

    //mutex for all locking
    pthread_mutex_t  work_mutex;
    // signals that there is work to be processed
    pthread_cond_t   work_cond;
    // signals that there are no threads processing
    pthread_cond_t   working_cond;

    size_t alive_thread_count;
    size_t thread_count;
    //false by default
    bool stop;

}jobScheduler;



//--------------QUEUE FUNCTIONS--------------//

//constructor for a queue node
queueNode *createQueueNode(Job *);

//destructor for the node it DOESNT FREE THE JOB
void freeQueueNode(queueNode *);

//constuctor for the Queue
Queue *createQueue();

//functions that adds a job to the end of the queue
void queueInsert(jobScheduler *, Job *);

//function that pops and returns the job from the top of the queue
Job *queuePop(Queue *);

//function that initialises and returns job scheduler 
jobScheduler* initialise_jobScheduler(int );

//function that initialises and returnes a job
Job *create_job(thread_funct function, Arguments *arg_struct);

void JobSchedulerWait(jobScheduler *jb);

#endif