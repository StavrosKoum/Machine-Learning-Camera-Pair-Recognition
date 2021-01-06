#include <stdio.h>
#include <stdlib.h>
#include "jobScheduler.h"

//--------------QUEUE FUNCTIONS--------------//

//constructor for a queue node
queueNode *createQueueNode(Job *j){

    queueNode *node = NULL;
    
    //allocate space
    node = malloc(sizeof(queueNode));
    //error check
    if(node == NULL){
        perror("malloc");
        exit(-3);
    }

    //add the job
    node->job = j;

    //return the object
    return node;
}

//destructor for the node it DOESNT FREE THE JOB
void freeQueueNode(queueNode *n){

    //free
    free(n);

}

//constuctor for the Queue
Queue *createQueue(){

    Queue *q = NULL;

    //allocate space
    q = malloc(sizeof(Queue));
    //error check
    if(q == NULL){
        perror("malloc");
        exit(-3);
    }

    //set pointers to null
    q->head = NULL;
    q->tail = NULL;
    //set counter to 0
    q->counter = 0;

    //return the object
    return q;
}

//functions that adds a job to the end of the queue
void queueInsert(Queue *q, Job *j){

    queueNode *node = NULL;

    //create a node for the job
    node = createQueueNode(j);

    //if the queue is empty
    if(q->counter == 0){
        //update tail and head
        q->head = node;
        q->tail = node;
        //adjust counter
        q->counter = 1;

    //add the node to the end of the queue
    }else{

        q->tail->next = node;
        //adjust counter
        q->counter += 1; 

    }
    
}

//function that pops and returns the job from the top of the queue
Job *queuePop(Queue *q){

    queueNode *node = NULL;
    Job *j = NULL;

    //if the list is empty its en error
    if(q->counter == 0){
        printf("Queue is empty!\n");
        return NULL;
    }

    //adjust the counter
    q->counter -= 1;

    //keep the node
    node = q->head;
    //keep the job
    j = node->job;

    //free the node
    freeQueueNode(node);

    //move head to next node
    q->head = q->head->next;

    //return the job
    return j;
    
}

//constructor for job

Job *create_job(void * function, Arguments *arg_struct)
{
    Job* ptr = malloc(sizeof(Job));
    ptr->function = function;
    ptr->args = arg_struct;

    return ptr;

}

//jobScheduler constructor
jobScheduler* initialise_jobScheduler(int num_threads,void* thread_function)
{
    jobScheduler* ptr = malloc(sizeof(jobScheduler));
    ptr->q = createQueue();
    ptr->execution_threads = num_threads;

    void* temp;

    Arguments* args = malloc(sizeof(Arguments));
    args->argv = malloc(sizeof(int));
    int i = 555;
    args->argv[0] = i;
    Job* job = create_job(NULL,args);
    queueInsert(ptr->q,job);
   
    //make n numThreads in array
    ptr->tids = malloc(sizeof(pthread_t)*num_threads);

    for(int i =0; i <num_threads;i++)
    {
        //make thread
        pthread_create(&ptr->tids[i], NULL,thread_function,(void *)ptr->q);

    }

    for(int i =0; i <num_threads;i++)
    {
        pthread_join(ptr->tids[i],&temp);

    }


    




    return ptr;
}



