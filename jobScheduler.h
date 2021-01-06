#ifndef JOBSCHEDULER
#define JOBSCHEDULER

#include <pthread.h>
#include <stdio.h>

//struct keeping the arguments for each job
typedef struct args{

    //num of args
    int argc;

    //array with arguments
    void **argv;

}Arguments;   



//job struct
typedef struct job{


    //pointer to the function to be executed
    void *function;
    //pointer to a struct containing arguments
    Arguments *args;

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

}jobScheduler;

//--------------QUEUE FUNCTIONS--------------//

//constructor for a queue node
queueNode *createQueueNode(Job *);

//destructor for the node it DOESNT FREE THE JOB
void freeQueueNode(queueNode *);

//constuctor for the Queue
Queue *createQueue();

//functions that adds a job to the end of the queue
void queueInsert(Queue *, Job *);

//function that pops and returns the job from the top of the queue
Job *queuePop(Queue *);

jobScheduler* initialise_jobScheduler(int ,void *);

Job *create_job(void * function, Arguments *arg_struct);

#endif