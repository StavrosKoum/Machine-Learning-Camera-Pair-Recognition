#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logisticRegression.h"
#include "graph.h"
#include "metrics.h"
#include "time.h"
#include "red-black.h"
#include "sparce.h"
#include "jobScheduler.h"
#include <signal.h>
#include <unistd.h>

//init mutex
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;;
pthread_mutex_t lock_weight = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond_weight = PTHREAD_COND_INITIALIZER;

logistic_reg * create_logistic_reg(int lineSize)
{

    logistic_reg *ptr;

    ptr = malloc(sizeof(logistic_reg));

    ptr->bias = 0.0;
    ptr->learning = 0.05;
    ptr->x = NULL;
    ptr->lineSize = lineSize;
    ptr->arraySize = 0;

    ptr->weights = malloc(lineSize * sizeof(double));

    //init values
    for(int i= 0; i <lineSize;i++)
    {
        ptr->weights[i] = 0.0;
    }
    return ptr;

}

void fit(logistic_reg * cur,sparceMatrix **x,int *y, int lineSize,int array_size)
{
    cur->x = x;
    cur->y = y;
    cur->lineSize = lineSize;
    cur->arraySize = array_size;
}


double logistic_function(double z)
{
    double temp = 1.0 / (1.0 + exp(-z));

    return temp;
}


double calculateZ(sparceMatrix *x_line, logistic_reg *cls)
{
    double z = 0.0; //cls->bias;
    sparceNode* cur = x_line->head;

    while(cur!=NULL)
    {
        z += (cls->weights[cur->index] * cur->value);
        cur = cur->next;
    }


    // for(int i =0; i < cls->lineSize; i++)
    // {
    //     z += (cls->weights[i]*x_line[i]);
    // }
    //printf("%f \n", z);

    return logistic_function(z);
}
double cost_function(logistic_reg *cls){

    double error=0.0; 
    double z=0.0;
    sparceMatrix* line = NULL;
    double error_sum = 0.0;


    for(int i =0; i < cls->arraySize;i++ )
    {
        line = cls->x[i];
        z = calculateZ(line, cls);
        // printf("Z               %f\n", z);
        if(cls->y[i]==1)
        {              

            error = -log10(z);
        }
        else
        {

            error = -log10(1 - z);
        }
        error_sum +=error;
    }

    // printf("%f      %d\n", error_sum, cls->arraySize);

    double J = error_sum / cls->arraySize;
    return J;
} 

double cost_function_derivative(logistic_reg *cls, int j)
{
    double linear_score = 0.0;
    double error = 0.0;
    double J;
    sparceMatrix* line = NULL;
    double error_sum = 0.0;
    double line_j;

    for(int i =0; i < cls->arraySize;i++ )
    {
        line = cls->x[i];
        line_j = sparce_search(line,j);

        //if line[j] is 0
        //we know that the error will also be 0
        if(line_j == 0){
            continue;
        }

        linear_score = calculateZ(line,cls);
        error = (linear_score - cls->y[i]) * line_j;
        error_sum +=error;

        if(cls->y[i] == 1){
            //if its 1 add more repetitions
            for(int k = 0; k < 12; k++){

                line = cls->x[i];

                linear_score = calculateZ(line,cls);
                error = (linear_score - cls->y[i]) * line_j;
                error_sum +=error;

            }
        }
    }


    //printf("%d %f\n", i, x[i][0]);
    //linear_score = calculateZ(cls->x,cls);
    // printf("linear score is %6.4f",linear_score);

    //error = (linear_score - cls->y) * cls->x[j];

    J = (cls->learning) * error_sum;

    //printf("J is ----> %6.4f",J);
    return J;
}

void cost_function_derivative2(void *arg)
{

    Arguments *args = arg;
    logistic_reg *cls = args->classfier;

    double linear_score = 0.0;
    double error = 0.0;
    // double J;
    sparceMatrix* line = NULL;
    double error_sum = 0.0;
    double line_j;

    //get start end
    pthread_mutex_lock(&lock);
    int start = args->start;
    int finish = args->finish;
    pthread_mutex_unlock(&lock);

    int j = args->k;

    for(int i = start; i < finish;i++ )
    {
        line = cls->x[i];
        line_j = sparce_search(line,j);

        //if line[j] is 0
        //we know that the error will also be 0
        if(line_j == 0){
            continue;
        }

        linear_score = calculateZ(line,cls);
        error = (linear_score - cls->y[i]) * line_j;
        error_sum +=error;

        if(cls->y[i] == 1){
            //if its 1 add more repetitions
            for(int k = 0; k < 8; k++){

                line = cls->x[i];

                linear_score = calculateZ(line,cls);
                error = (linear_score - cls->y[i]) * line_j;
                error_sum +=error;

            }
        }
    }


    //printf("%d %f\n", i, x[i][0]);
    //linear_score = calculateZ(cls->x,cls);
    // printf("linear score is %6.4f",linear_score);

    //error = (linear_score - cls->y) * cls->x[j];

    //critical section here
    pthread_mutex_lock(&lock);
    //update alive threads counter
    // printf("NUM: %d    Thread %ld\n", *(args->activeThreads), pthread_self());
    *(args->activeThreads) -= 1;
    //update the z
    *(args->J) =+ (cls->learning) * error_sum;

    //if its the last thread signal the parent to continue
    if(*(args->activeThreads) == 0){
        // printf("SIGNAL %ld", pthread_self());
        pthread_mutex_lock(&lock_weight);
        pthread_cond_signal(&cond_weight);
        pthread_mutex_unlock(&lock_weight);

    }

    //free arguments
    free(args);

    pthread_mutex_unlock(&lock);
    // printf("job done %ld\n", pthread_self());
}

double* gradient_descend(logistic_reg *cls)
{
    double derivative = 0.0;

    for(int i =0; i < cls->lineSize;i++ )
    {
        derivative = cost_function_derivative(cls,i);
    
        cls->weights[i] -= derivative;

    }
    
    return cls->weights;
}



//for each step until limit the algorithm calculates gradients which change the weights
//each time closer to the ideal weights
logistic_reg* logisticRegretionAlgorithm(logistic_reg *cls, int limit, Bucket **ht, int HTsize, word_ht *wordHash,sparceMatrix **x, int *y,int x_size,int batchSize, Bucket **trHash, int trSize,int thread_num){

    double threshold = 0.10;
    double step = 0.05;
    int current;
    int remaining;
    sparceMatrix **x_train;
    trainData * trainSet = NULL;
    int *y_train;
    int i = 1;
    int bSize = batchSize;
    int newRemaining = x_size * 60 / 100;

    //create the struct to hold the train set
    trainSet = createTrainData(x, y);
    
    //Init job scheduler
    jobScheduler *jb;
    Arguments *args;
    Job *job;

    double J;
    int aliveThreads;
    int result;
    int numfiles[thread_num];
    int start[thread_num];
    int end[thread_num];
    int pos = 0;

    jb = initialise_jobScheduler(thread_num);
    printf("threads created\n");

    // // init mutex
    if (pthread_mutex_init(&lock, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return NULL;
    }
    if (pthread_mutex_init(&lock_weight, NULL) != 0)
    {
        printf("\n mutex init failed\n");
        return NULL;
    }

    //do the following steps until the threshold
    while(threshold < 0.5){

        x_train = NULL;
        y_train = NULL;

        current = 0;
        remaining = newRemaining;
        batchSize = bSize;
            
        while(remaining != 0){

            // counter to help divide batch for threads
            pos = 0;

            if(remaining < batchSize){
                batchSize = remaining;
            }

            //allocate the arrays
            x_train  = malloc(sizeof(sparceMatrix*) * batchSize);
            y_train = malloc(sizeof(int) * batchSize);

            for(int i=0; i < batchSize; i++){
                x_train[i]= trainSet->x[current];
                y_train[i] = trainSet->y[current];
                current+=1;
            }

            // seperate batch
            result = batchSize / thread_num;
            for(int i = 0; i < thread_num; i++){
                numfiles[i] = result;
            }
            int mod = batchSize % thread_num;
            while(mod != 0){
                for(int i = 0; i < thread_num; i++){
                    if(mod > 0){
                        numfiles[i] += 1;
                        mod--;
                    }
                }
            }

            for(int i=0;i<thread_num;i++){
                
                // printf("%d  %d\n",i,numfiles[i]);
                start[i] = pos;
                end[i] = start[i]+numfiles[i];
                pos = end[i];
            }

            // for(int i=0;i<thread_num;i++){
            //     args = malloc(sizeof(Arguments));
            //     args->start = start[i];
            //     args->finish = end[i];

            //     printf("thread num %d  start %d end %d\n",i,start[i],end[i]);
            // }

            fit(cls, x_train, y_train, wordHash->id_counter, batchSize);
            //calculate the new weights


            // cls->weights = gradient_descend(cls);

            // double derivative = 0.0;

            for(int k =0; k < cls->lineSize;k++ )
            {
                J = 0.0;
                aliveThreads = thread_num;

                pthread_mutex_lock(&lock_weight);
                for(int i=0;i<thread_num;i++){

                    args = malloc(sizeof(Arguments));
                    args->start = start[i];
                    args->finish = end[i];
                    args->J = &J;
                    args->activeThreads = &aliveThreads;
                    // args->lock = &lock;
                    args->classfier = cls;
                    args->k = k;
                    // args->lock_weight = &lock_weight;
                    // args->cond_weight = &cond_weight;
                    job = create_job(cost_function_derivative2,args);

                    queueInsert(jb,job);
                }

                // derivative = cost_function_derivative(cls,k);
                // wait threads to finish working
                // printf("waiting threads\n");
                pthread_cond_wait(&cond_weight, &lock_weight);
                pthread_mutex_unlock(&lock_weight);

            
                cls->weights[k] -= J;

            }

            printf("Remaining = %d and batchSize = %d\n",remaining,batchSize);
            // printf("Current Cost: %f\n", cost_function(cls));
            free(y_train);
            free(x_train);

            remaining -= batchSize;
        }

        logisticValidationTest(cls,x,y,x_size);
        if(i == 3)
        {
            JobSchedulerWait(jb);
            break;
        }      
        //enlarge the data set
        trainSet = predictHashTable(cls, ht, HTsize, threshold, wordHash, trHash, trSize, i, trainSet, &newRemaining);
        
        // printf("AFTER New Remaining %d\n",newRemaining);
        // for(int j=0;j<newRemaining;j++){
        //     printf("INDEX %d        %d\n",j, trainSet->y[j]);
        // }

        
        threshold += step;
        i++;
        
         
    }

    //free unneeded memory
    free(trainSet->y);
    for(int i = x_size * 60 / 100; i < newRemaining; i++){
        deleteSparceMatrix(trainSet->x[i]);
    }
    free(trainSet);

    //return the new weights
    return cls;
}

double logisticRegrationTest(logistic_reg *cls, sparceMatrix *data, char *left, char *right, int y, FILE *fp,positiveMetrics *P_metrics, negativeMetrics *N_metrics){    
    //sum
    double z = 0.0;
    //model prediction
    int roundZ;

    //calculate the z for the data
    z = calculateZ(data, cls);
    roundZ = round(z);

    

    //print metrics
    P_metrics = updatePositiveMetrics(P_metrics,roundZ,y);
    N_metrics = updateNegativeMetrics(N_metrics,roundZ,y);

    //printf("%d\n %d\n %d\n ---------- \n", P_metrics->truePositive,P_metrics->falsePositive,P_metrics->flaseNegative);
    //printf("%d\n %d\n %d\n ---------- \n", N_metrics->truePositive,N_metrics->falsePositive,N_metrics->flaseNegative);

    //print the result
    fprintf(fp, "%s, %s, prediction: %f, result %d\n", left, right, z, y);

    
    //return it
    return z;
}

void logisticValidationTest(logistic_reg *cls, sparceMatrix **data,  int * y,int size)
{    
    //sum
    double z = 0.0;
    //model prediction
    int roundZ;

    //init metrics
    positiveMetrics *P_metrics;
    negativeMetrics *N_metrics;

    P_metrics = initPositiveMetrics("possitive");
    N_metrics = initNegativeMetrics("negative");


    for(int i= (80/100) * size ; i< size; i++)
    {
        //calculate the z for the data
        z = calculateZ(data[i], cls);
        roundZ = round(z);

        //print metrics
        P_metrics = updatePositiveMetrics(P_metrics,roundZ,y[i]);
        N_metrics = updateNegativeMetrics(N_metrics,roundZ,y[i]);


    }
    
    //get metrics
    P_metrics = evaluatePositiveMetrics(P_metrics);
    N_metrics = evaluateNegativeMetrics(N_metrics);

    printf("-----------------\n-----------------\n");
    printf("Model results after training: \n");
    printPositiveMetrics(P_metrics);
    //printf("-----------------\n-----------------\n");
    printNegativeMetrics(N_metrics);
    printf("-----------------\n-----------------\n");
    //free metrics 
    freePositiveMetrics(&P_metrics);
    freeNegativeMetrics(&N_metrics);
    
}


void printClassifier(logistic_reg *cls){

    //file pointer
    FILE *fp = NULL;
    
    //open the file for writing
    fp = fopen("classifier.txt", "w");
    //error check
    if(fp == NULL){
        perror("fopen");
        exit(-2);
    }

    //write the learning rate
    fprintf(fp, "Learning: %f", cls->learning);

    //write the weights
    fprintf(fp, "Weights:\n");
    for(int i = 0; i < cls->lineSize; i++){

        fprintf(fp, "%f\n", cls->weights[i]);
    }

    //close file
    fclose(fp);
}

//destructor for the logistic regressor
void freeLogisticRegressor(logistic_reg *cls){

    //the x array is freed in another place
    //we dont have to do anything for it

    //free the weights 
    free(cls->weights);
    //free the classifier
    free(cls);
}


//shuffle a 2d array of doubles and return it
double** shuffleArray(double** array, int *array2, int size, char** array3, char** array4){

    //line to be swapped with current
    int line = 0;
    //temp line
    double *temp = NULL;
    int tempRes = 0;
    char *tempName = NULL;

    //initialize generator
    //srand(time(NULL));
    //traverse the array
    for(int i = 0; i < size; i++){
        //get a random number between i -- rand_max
        line = rand() % size;
        //checkif we are within limits
        if(line >= size){
            line = line % size;
        }

        //swap the lines
        temp = array[i];
        array[i] = array[line];
        array[line] = temp; 

        //do the same for the results
        tempRes = array2[i];
        array2[i] = array2[line];
        array2[line] = tempRes; 

        //flip name array
        tempName = array3[i];
        array3[i] = array3[line];
        array3[line] = tempName;

        //flip name array
        tempName = array4[i];
        array4[i] = array4[line];
        array4[line] = tempName;
        
    }   

    //returnt he completed array
    return array;
}

//IF DECISION BOUNDARY CHANGES --> CHANGE LINES:

//transitivity issues are resolved using the following algorithm
//entries are extracted from the tree sorted by the prediction distance from 0 or 1
//if it's possible they are added to the hashtable
//after the procedure above is completed the new train set (X and Y) is created
void resolveTransitivity(treeNode *root, Bucket **hashTable, int trSize, sparceMatrix **X, int *Y, SparceList *spList){

    int hash;
    int found_left = 0;
    int found_right= 0;
    Negative_node* tmp_neg;
    Negative_node *neg = NULL;
    jsonFile* tmp_json = NULL;
    Clique *foundNode = NULL;
    bucketNode *foundNodeArray = NULL;
    // Clique *temp;
    int found = 0;
    int total = 0;


    if(root == NULL)
        return;

    //go to left sub-tree
    resolveTransitivity(root->right, hashTable, trSize, X, Y,spList);

    //get the pair
    transitivityPair *pair = NULL;

    //check root and its links
    treeNode *cur;
    cur = root;
    while(cur != NULL){

        //get the pair
        pair = root->pair;

        //check if the jsonFiles are in the hashTable
        //left
        hash = hashing1(pair->leftJson->site, trSize);
        Clique *left = findEntry(hashTable, hash, pair->leftJson->site);
        //right
        hash = hashing1(pair->rightJson->site, trSize);
        Clique *right = findEntry(hashTable, hash, pair->rightJson->site);

        if(left != NULL)    
        {
            //printf("%s is in the hashTable LEFT\n", pair->leftJson->site);
            found_left = 1;
        }
        //else                printf("%s not found LEFT\n", pair->leftJson->site);

        if(right != NULL)
        {
            //printf("%s is in the hashTable RIGHT\n", pair->rightJson->site);
            found_right = 1;

        }    
        //else                printf("%s not found RIGHT\n", pair->rightJson->site);

        //left found while right not
        if(found_left ==1 && found_right == 0)
        {
            //printf("Left found, Right not found\n");
            //IF THE RESULT IS 0
            if(pair->result == 0)
            {
                //ADD right JSON TO HASHTABLE
                //add the json to the correct eshop bucket
                hash = hashing1(pair->rightJson->site, trSize);
                //printf("HASH %d\n", res);
                addToHashTable(hashTable[hash], pair->rightJson->site, hash, pair->rightJson);
                //take his clique
                right = findEntry(hashTable, hash, pair->rightJson->site);

                //create negative connection between right and elft cliques
                right->neg_node_list = create_negative_node(left);
                
                //create and insert negative connection between left and right cliques
                tmp_neg = left->neg_node_list;
                left->neg_node_list = create_negative_node(right);
                left->neg_node_list->next_ptr = tmp_neg;

                //Enlarge train
                InsertSparceListNode(spList,pair->array,pair->result);

            }
            else if(pair->result == 1)
            {
                pair->rightJson->next = left->file;
                left->file = pair->rightJson;

                //Enlarge train
                InsertSparceListNode(spList,pair->array,pair->result);
            }
        }
        if(found_left ==0 && found_right == 1)
        {
            //printf("Right found, Left not found\n");
            //IF THE RESULT IS 0
            if(pair->result == 0)
            {
                //ADD left JSON TO HASHTABLE
                //add the json to the correct eshop bucket
                hash = hashing1(pair->leftJson->site, trSize);
                //printf("HASH %d\n", res);
                addToHashTable(hashTable[hash], pair->leftJson->site, hash, pair->leftJson);
                //take his clique
                left = findEntry(hashTable, hash, pair->leftJson->site);

                //create negative connection between right and elft cliques
                left->neg_node_list = create_negative_node(right);
                
                //create and insert negative connection between right and right cliques
                tmp_neg = right->neg_node_list;
                right->neg_node_list = create_negative_node(left);
                right->neg_node_list->next_ptr = tmp_neg;

                //Enlarge train
                InsertSparceListNode(spList,pair->array,pair->result);

            }
            else if(pair->result == 1)
            {
                pair->leftJson->next = right->file;
                right->file = pair->leftJson;

                //Enlarge train
                InsertSparceListNode(spList,pair->array,pair->result);
            }
        }

        //if none of the jsonFiles were found
        if(found_left ==0 && found_right == 0)
        {
            //printf("None found\n");
            if(pair->result ==1)
            {
                //create and add ONE clique because they are the same (randomly the left)
                hash = hashing1(pair->leftJson->site, trSize);
                //printf("HASH %d\n", res);
                addToHashTable(hashTable[hash], pair->leftJson->site, hash, pair->leftJson);
                //take his clique
                left = findEntry(hashTable, hash, pair->rightJson->site);

                //also add right
                left->file->next = pair->rightJson;

                //Enlarge train
                InsertSparceListNode(spList,pair->array,pair->result);
            }

            //now if they are negative connected
            if(pair->result == 0 )
            {
                //FOR LEFT
                hash = hashing1(pair->leftJson->site, trSize);
                //printf("HASH %d\n", res);
                addToHashTable(hashTable[hash], pair->leftJson->site, hash, pair->leftJson);
                //take his clique
                left = findEntry(hashTable, hash, pair->rightJson->site);

                //FOR RIGHT
                hash = hashing1(pair->rightJson->site, trSize);
                //printf("HASH %d\n", res);
                addToHashTable(hashTable[hash], pair->rightJson->site, hash, pair->rightJson);
                //take his clique
                right = findEntry(hashTable, hash, pair->rightJson->site);

                //create negative conectio
                left->neg_node_list = create_negative_node(right);
                right->neg_node_list = create_negative_node(left);

                //Enlarge train
                InsertSparceListNode(spList,pair->array,pair->result);
            }

        }
        //if they both exist to hashtable
        if(found_left == 1 && found_right == 1)
        {
            //Enlarge train
            InsertSparceListNode(spList,pair->array,pair->result);
            
            //printf("Both found\n");

            //init found
            found = 0;
            //files in booth cliques
            total = left->cliqueSum + right->cliqueSum;
            //check if there is a negative connection between the files
            //search left negatives for right
            neg = left->neg_node_list;
            while(neg != NULL && found == 0)
            {
                //search negs files
                if(searchClique(neg->neg_clique_ptr, pair->rightJson->site) != NULL)
                {
                    //no work needed
                    //printf("found negative\n");
                    found = 1;
                }
                neg = neg->next_ptr;
            }

            int found_positive = 0 ;
            //search at left for right POSITIVE
            if(searchClique(left, pair->rightJson->site) != NULL)
            {
                found_positive = 1;
                //printf("found positive\n");
            }
            if(found == 0 && found_positive == 0)
            {
                //if the result is negative
                if(pair->result == 0)
                {
                    //printf("Negative case\n");
                    //insert negative node to left
                    tmp_neg = left->neg_node_list;
                    left->neg_node_list = create_negative_node(right);
                    left->neg_node_list->next_ptr = tmp_neg;

                    //insert negative node to right
                    tmp_neg = right->neg_node_list;
                    right->neg_node_list = create_negative_node(left);
                    right->neg_node_list->next_ptr = tmp_neg;

                    // //Enlarge train
                    // InsertSparceListNode(spList,pair->array,pair->result);
                }
                //if the result is positive
                else
                {
                    //printf("Positive case\n");
                   //append right to left clique
                   tmp_json = left->file;

                    //go to last file
                    while(tmp_json->next != NULL)
                    {
                        tmp_json = tmp_json->next;
                    }

                    //insert right list to left list of jsons
                    tmp_json->next = right->file;

                    //update array pointers of right files
                    tmp_json = right->file;

                    while(tmp_json != NULL){
                        //get the array pointer to that file
                        //search the hasttable for the node with the same name
                        hash = hashing1(tmp_json->site, trSize);
                        //get the node
                        foundNode = findEntry(hashTable, hash, tmp_json->site);
                        //error check
                        if(foundNode == NULL){
                            printf("ELOUSA\n");
                            exit(-4);
                        }

                        //get the array pointer of the node
                        foundNodeArray = findEntryArrayPointer(hashTable, hash, tmp_json->site);

                        //make the array pointer to point to left clique
                        foundNodeArray->graph->head = left; 
                        
                        //go to next jsonFile from right list
                        tmp_json = tmp_json->next;
                    }
                    //merge negative lists
                    fixLists(hashTable,trSize,left,right);
                    //remove if needed
                    if(foundNode != NULL){

                        cliqueFree(foundNode);
                    }
                    //update file counter
                    left->cliqueSum = total;
                }
                
            }

        }

        //printf("%f\n", cur->priority);            

        //go to next node
        cur = cur->next;
    }

    //go to right sub-tree
    resolveTransitivity(root->left, hashTable, trSize, X, Y,spList);


}

//function that traverses all the pairs and selects the results with the 
//higher accuracy depening on the threshold
trainData* predictHashTable(logistic_reg *cls, Bucket ** ht, int HTsize, double threshold, word_ht *wordHash, Bucket **trHash, int trSize, int lim, trainData *trainSet, int *size_x){

    Bucket *cur = NULL;
    int limit = 0;
    Clique *clique = NULL;
    jsonFile *curFile = NULL;
    jsonFile *ptr = NULL;
    jsonFile *neg_file1 = NULL;
    jsonFile *neg_file2 = NULL;
    double *X = NULL;
    double z = 0.0;
    int pCounter = 0;
    int nCounter = 0;
    sparceMatrix * sparce_matrix;
    sparceMatrix * sparce_matrix2;

    //to store the results that pass the requirements
    //transitivityPair pairs[HTsize];
    transitivityPair *filePair = NULL;
    jsonFile *leftJson = NULL;
    jsonFile *rightJson = NULL;
    rbTree* tree_ptr;
    SparceList *spList;
    
    printf("Resolving Transitivity\n");
    
    //create tree for pairs
    tree_ptr = createTree();
    
    //traverse the hashTable
    for(int i = 0; i < HTsize; i++){

        //set current
        cur  = ht[i];
        //search cur and it's links
        while(cur != NULL){

            //fix limit
            limit = cur->spaces;

            //for each space in the array
            for(int k = 0; k < limit; k++){
                
                //if there is a clique
                if(cur->array[k] != NULL){

                    //get it
                    clique = cur->array[k]->graph->head;
                    //printf("_______________ %d, %s",k,clique->name);


                    //first, traverse all the positive files
                    curFile = clique->file;

                    //printf("%s          %d\n", clique->name, clique->cliqueSum);

                    if(clique->cliqueSum != 1 && clique->printed < lim){

                        
                      
                        //traverse all the nodes in the POSITIVE list
                        for(int j = 0; j<clique->cliqueSum -1; j++)
                        {
                            //init ptr, X
                            ptr = NULL;
                            X = NULL;
                            if(curFile->JsonWordCount==NULL)
                            {
                                //get tf-idf representation of curFile
                                CreateJsonListWordCountArray(curFile,wordHash->id_counter);
                                FillJsonWordCountArray(curFile,wordHash->id_counter,wordHash);
                            }
                            

                            ptr = curFile->next;
                            //traverse the nodes after the current
                            for(int u = j; u < clique->cliqueSum -1; u++)
                            {
                                if(ptr->JsonWordCount ==NULL)
                                {
                                    //get the tf-idf representation of each file
                                    CreateJsonListWordCountArray(ptr,wordHash->id_counter);
                                    FillJsonWordCountArray(ptr,wordHash->id_counter,wordHash);
                                }
                                
                                
                                //concat the two arrays
                                X = arrayConcat(curFile->JsonWordCount, ptr->JsonWordCount, wordHash->id_counter);
                                //create sparce matrix with X array
                                sparce_matrix = create_sparce_matrix(X,wordHash->id_counter);


                                //get the model predection
                                z = calculateZ(sparce_matrix, cls);
                                //printf("PREDICTION %f %s %s\n", z,ptr->site,curFile->site);

                                //check if the prediction is in the range we want
                                if(z > (1.0 - threshold) || z < threshold){
                                // if(0){

                                    //printf("adding positive %f\n", z);

                                    //count the pair
                                    pCounter++;

                                    //create 2 jsonFiles with the data
                                    leftJson = createRedusedJsonFile(curFile->site, curFile->JsonWordCount);
                                    rightJson = createRedusedJsonFile(ptr->site, ptr->JsonWordCount);
                                    //create the pair
                                    filePair = createTransitivityPair(leftJson, rightJson, z, sparce_matrix, 1);
                                    
                                    // printf("->>>>>>>>>>>>>>>>>>> %s______%s \n",filePair->leftJson->site,filePair->rightJson->site);
                                    // printf("%f\n", filePair->prediction);
                                    //add the pair to the tree
                                    insertTree3(filePair, &tree_ptr->root);
                                    // printf("after\n");
                                    //update counter
                                    tree_ptr->counter += 1;

                                }else{
                                        deleteSparceMatrix(sparce_matrix);
                                }

                                free(X);

                                

                                // if(X != NULL)
                                // {
                                //     free(X);
                                // }
                                ptr = ptr->next;

                            }
                            //free arrays
                            //free(curFile->JsonWordCount);
                            curFile = curFile->next;
                            
                        }
                        

                        
                        clique->printed += 1;

                    }

                    Clique* tmp = NULL;
                    Negative_node* cur_neg;
                    cur_neg = clique->neg_node_list;
                    //traverse all the NEGATIVE nodes 
                    while(cur_neg!=NULL)
                    {   
                        //printf("CLIQUE %p\nNEGATIVE %p\nNEXT %p\n", clique, cur_neg->neg_clique_ptr, cur_neg->next_ptr->neg_clique_ptr);
                       
                        //search for neg clique
                        tmp = cur_neg->neg_clique_ptr;

                        Negative_node* temp_neg_node;
                        temp_neg_node = tmp->neg_node_list;
                        while (temp_neg_node!=NULL)
                        {

                            if(!strcmp(temp_neg_node->neg_clique_ptr->name,clique->name))
                            {
                                //printf("edw %d\n", temp_neg_node->pair);
                                if(temp_neg_node->pair < lim){
                                    temp_neg_node->pair += 1;
                                }
                            }
                            temp_neg_node = temp_neg_node->next_ptr;
                        }

                        if(cur_neg->pair == (lim - 1))
                        {

                            //update cur_neg list with pairs
                            temp_neg_node = clique->neg_node_list;
                            while(temp_neg_node != NULL){
                                if(!strcmp(temp_neg_node->neg_clique_ptr->name, cur_neg->neg_clique_ptr->name)){
                                    if(temp_neg_node->pair < lim){
                                        temp_neg_node->pair += 1;
                                    }
                                }
                                temp_neg_node = temp_neg_node->next_ptr;
                            }
                            
                            neg_file1 = clique->file;

                            
                            while(neg_file1!=NULL)
                            {
                                
                                if(neg_file1->JsonWordCount == NULL)
                                {
                                    //get the tf-idf representation of each file
                                    CreateJsonListWordCountArray(neg_file1,wordHash->id_counter);
                                    FillJsonWordCountArray(neg_file1,wordHash->id_counter,wordHash);
                                }
                                
                                neg_file2 = tmp->file;

                                while (neg_file2!= NULL)
                                {

                                    if(neg_file2->JsonWordCount == NULL)
                                    {
                                        //get the tf-idf representation of each file
                                        CreateJsonListWordCountArray(neg_file2,wordHash->id_counter);
                                        FillJsonWordCountArray(neg_file2,wordHash->id_counter,wordHash);
                                    }
                                    
                                
                                    //concat the two arrays
                                    X = arrayConcat(neg_file1->JsonWordCount, neg_file2->JsonWordCount, wordHash->id_counter);
                                    //create sparce matrix with X array
                                    sparce_matrix2 = create_sparce_matrix(X,wordHash->id_counter);

                                    //get the model predection
                                    z = calculateZ(sparce_matrix2, cls);
                                    //printf("NEGATIVE PREDICTION %f %s %s\n", z,neg_file2->site,neg_file1->site);
                                    
                                    //if its in the range we want
                                    if(z < threshold || (z > (1.0 - threshold))){
                                    // if(0){

                                        //create 2 jsonFiles with the data
                                        leftJson = createRedusedJsonFile(neg_file1->site, neg_file1->JsonWordCount);
                                        rightJson = createRedusedJsonFile(neg_file2->site, neg_file2->JsonWordCount);
                                        //create the pair
                                        filePair = createTransitivityPair(leftJson, rightJson, z, sparce_matrix2, 0);
                                        
                                        //printf("->>>>>>>>>>>>>>>>>>> %s______%s \n",filePair->leftJson->site,filePair->rightJson->site);
                                        //add the pair to the tree
                                        insertTree3(filePair, &tree_ptr->root);
                                        //update counter

                                        //count it
                                        nCounter++;

                                    }else{
                                        deleteSparceMatrix(sparce_matrix2);
                                    }

                                    free(X);
                                    
                                    
                                    
                                    
                                    //free(neg_file2->JsonWordCount);
                                    //free(X);                  
                                     
                                    neg_file2 = neg_file2->next;
                                }


                                //free(neg_file1->JsonWordCount);
                                neg_file1 = neg_file1->next;
                            }

                            
                        }
                        cur_neg = cur_neg->next_ptr;
                    }

                }

            }

            cur = cur->next;
        }

    }    

    //print tree
    //printTree(tree_ptr->root);
    //Print results
    printf("Positive pairs: %d\n", pCounter);
    printf("Negative pairs: %d\n", nCounter);

    //create a list for sparce matrix
    spList = createSparceList();
    //time to resolve transitivity
    resolveTransitivity(tree_ptr->root, trHash, trSize, trainSet->x, trainSet->y,spList);
    // printTree(tree_ptr->root);
    printf("insertions %d\n",spList->counter);

    int newSize = spList->counter + *size_x;

    //Increase the size of train array
    sparceMatrix **newTrain;
    int *new_y;
    newTrain = malloc(sizeof(sparceMatrix*)*newSize);
    new_y = malloc(sizeof(int)*newSize);

    //fill the new array
    for(int j=0;j<*size_x;j++){
        newTrain[j] = trainSet->x[j];
        new_y[j] = trainSet->y[j];
    }

    // free(trainSet->x);
    // free(trainSet->y);
    // free(trainSet);

    // trainSet = malloc(sizeof(trainSet));

    // x = NULL;
    // y = NULL;

    SparceListNode *temp;
    SparceListNode *toFreeNode = NULL;
    temp = spList->head;
    for(int i = *size_x; i < newSize; i++){

        toFreeNode = temp;

        newTrain[i] = temp->matrix;
        new_y[i] = temp->result;

        temp = temp->next;
        free(toFreeNode);
    }
    free(spList);

    //assign values to struct
    trainSet->x = newTrain;
    trainSet->y = new_y;

    //free the tree
    freeTree(tree_ptr);

    //update size
    *size_x = newSize;
    //return the new set
    return trainSet;
}