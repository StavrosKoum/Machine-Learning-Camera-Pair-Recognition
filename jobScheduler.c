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
void queueInsert(jobScheduler *JS, Job *j){

    queueNode *node = NULL;

    //create a node for the job
    node = createQueueNode(j);

    //lock while insertion
    //its critical section
    pthread_mutex_lock(&(JS->work_mutex));

    //if the queue is empty
    if(JS->q->counter == 0){
        //update tail and head
        JS->q->head = node;
        JS->q->tail = node;
        //adjust counter
        JS->q->counter = 1;

    //add the node to the end of the queue
    }else{

        JS->q->tail->next = node;
        JS->q->tail = JS->q->tail->next; 
        //adjust counter
        JS->q->counter += 1; 

    }
    
    //notify the threads
    pthread_cond_broadcast(&(JS->work_cond));
    //end of critical section
    pthread_mutex_unlock(&(JS->work_mutex));

}

//function that pops and returns the job from the top of the queue
Job *queuePop(Queue *q){

    queueNode *node = NULL;
    Job *j = NULL;

    //if the list is empty its en error
    if(q->counter == 0){
        return NULL;
    }

    //adjust the counter
    q->counter -= 1;

    //keep the node
    node = q->head;
    //keep the job
    j = node->job;

    //move head to next node
    q->head = q->head->next;

    //free the node
    //freeQueueNode(node);

    //return the job
    return j;
    
}

//constructor for job

Job *create_job(thread_funct function, Arguments *arg_struct)
{

    // error check
    if(function == NULL){
        printf("Null function\n");
        return NULL;
    }

    Job* ptr = malloc(sizeof(Job));
    ptr->func = function;
    ptr->args = arg_struct;

    return ptr;

}


void* thread_Job_function(void* jobSch)
{

    jobScheduler *JS = (jobScheduler *) jobSch;
    Job *work;

    while(1){
        // lock mutex to secure the jobScheduler numbers
        pthread_mutex_lock(&(JS->work_mutex));
        //check if there is more work to be done
        //wait
        while(JS->q->counter == 0 && !JS->stop){
            //lock work cond and unlock work mutex
            //printf("Thread going to sleep\n");
            pthread_cond_wait(&(JS->work_cond), &(JS->work_mutex));
        }

        if(JS->stop){
            break;
        }

        work = queuePop(JS->q);
        JS->alive_thread_count++;
        pthread_mutex_unlock(&(JS->work_mutex));

        if(work != NULL){
            work->func(work->args);
            //destroy job
        }

        pthread_mutex_lock(&(JS->work_mutex));
        JS->alive_thread_count--;
        
        if(!JS->stop && JS->alive_thread_count == 0 && JS->q->counter == 0){
            pthread_cond_signal(&(JS->working_cond));
        }
        pthread_mutex_unlock(&(JS->work_mutex));
    }

    JS->thread_count--;
    pthread_cond_signal(&(JS->working_cond));
    pthread_mutex_unlock(&(JS->work_mutex));
    printf("end\n");

    return NULL;

    // Job* job = queuePop(queue);
    // if(job==NULL)
    // {
    //     return 0;
    // }
    // printf("thread id %ld and num is %d\n",pthread_self(),job->args->argv[0]);
    // return 0;
}

//jobScheduler constructor
jobScheduler* initialise_jobScheduler(int num_threads)
{

    jobScheduler* ptr = NULL;
    ptr = malloc(sizeof(jobScheduler));
    if(ptr == NULL){
        printf("tafo");
        exit(-3);
    }
    ptr->q = createQueue();
    ptr->execution_threads = num_threads;
    ptr->alive_thread_count = 0;
    ptr->thread_count = num_threads;
    ptr->stop = 0;
    // void* temp;

    // Arguments* args = malloc(sizeof(Arguments));
    // args->argv = malloc(sizeof(int));
    // int i = 555;
    // args->argv[0] = i;
    // Job* job = create_job(NULL,args);
    // queueInsert(ptr->q,job);
   
    //make n numThreads in array
    ptr->tids = malloc(sizeof(pthread_t)*num_threads);


    // init mutex
    pthread_mutex_init(&(ptr->work_mutex),NULL);
    pthread_cond_init(&(ptr->work_cond),NULL);
    pthread_cond_init(&(ptr->working_cond),NULL);


    for(int i =0; i <num_threads;i++)
    {
        //make thread
        pthread_create(&ptr->tids[i], NULL,thread_Job_function, ptr);
        pthread_detach(ptr->tids[i]);
    }

    // for(int i =0; i <num_threads;i++)
    // {
    //     pthread_join(ptr->tids[i],&temp);

    // }



    return ptr;
}


void JobSchedulerWait(jobScheduler *jb){

    if(jb == NULL){
        return;
    }
    pthread_mutex_lock(&(jb->work_mutex));

    while(1){
        if((!jb->stop && jb->alive_thread_count !=0 ) || (jb->stop && jb->thread_count != 0)){
            pthread_cond_wait(&(jb->working_cond), &(jb->work_mutex));
        }
        else{
            break;
        }
        
    }
    pthread_mutex_unlock(&(jb->work_mutex));
}




