#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logisticRegression.h"
#include "metrics.h"
#include "time.h"
#include "red-black.h"


logistic_reg * create_logistic_reg(int lineSize)
{

    logistic_reg *ptr;

    ptr = malloc(sizeof(logistic_reg));

    ptr->bias = 0.0;
    ptr->learning = 0.01;
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

void fit(logistic_reg * cur,double **x,int *y, int lineSize,int array_size)
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


double calculateZ(double *x_line, logistic_reg *cls)
{
    double z = 0.0; //cls->bias;

    for(int i =0; i < cls->lineSize; i++)
    {
        z += (cls->weights[i]*x_line[i]);
    }

    return logistic_function(z);
}
double cost_function(logistic_reg *cls){

    double error=0.0; 
    double z=0.0;
    double* line = NULL;
    double error_sum = 0.0;


    for(int i =0; i < cls->arraySize;i++ )
    {
        line = cls->x[i];
        z = calculateZ(line, cls);

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

    
    double J = error_sum / cls->arraySize;
    return J;
} 

double cost_function_derivative(logistic_reg *cls, int j)
{
    double linear_score = 0.0;
    double error = 0.0;
    double J;
    double* line = NULL;
    double error_sum = 0.0;

    for(int i =0; i < cls->arraySize;i++ )
    {
        line = cls->x[i];

        linear_score = calculateZ(line,cls);
        error = (linear_score - cls->y[i]) * line[j];
        error_sum +=error;

        if(cls->y[i] == 1){
            //if its 1 add more repetitions
            for(int k = 0; k < 3; k++){

                line = cls->x[i];

                linear_score = calculateZ(line,cls);
                error = (linear_score - cls->y[i]) * line[j];
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
logistic_reg* logisticRegretionAlgorithm(logistic_reg *cls, int limit, Bucket **ht, int HTsize, word_ht *wordHash,double **x, int *y,int x_size,int batchSize, Bucket **trHash, int trSize){

    double threshold = 0.1;
    double step = 0.10;
    int current;
    int remaining = x_size * 60 / 100;
    double **x_train;
    int *y_train;

    //do the following steps until the threshold
    while(threshold < 0.5){

        current = 0;
        remaining = x_size * 60 / 100;
            
        while(remaining != 0){

            if(remaining < batchSize){
                batchSize = remaining;
            }

            //allocate the arrays
            x_train  = malloc(sizeof(double*) * batchSize);
            y_train = malloc(sizeof(int) * batchSize);

            for(int i=0; i < batchSize; i++){
                x_train[i]= x[current];
                y_train[i] = y[current];
                current+=1;
            }

            fit(cls, x_train, y_train,2 * (wordHash->id_counter),batchSize);   

            //calculate the new weights
            cls->weights = gradient_descend(cls);

            printf("Remaining = %d and batchSize = %d\n",remaining,batchSize);
            printf("Current Cost: %f\n", cost_function(cls));
            free(y_train);
            free(x_train);
            remaining -= batchSize;
        }

        

        predictHashTable(cls, ht, HTsize, threshold, wordHash, trHash, trSize);
        threshold +=1;
    }
    //return the new weights
    return cls;
}

double logisticRegrationTest(logistic_reg *cls, double *data, char *left, char *right, int y, FILE *fp,positiveMetrics *P_metrics, negativeMetrics *N_metrics){    
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
double** shuffleArray(double** array, int *array2, char **array3, char ** array4, int size){

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

//function that traverses all the pairs and selects the results with the 
//higher accuracy depening on the threshold
double ** predictHashTable(logistic_reg *cls, Bucket ** ht, int HTsize, double threshold, word_ht *wordHash, Bucket **trHash, int trSize){

    Bucket *cur = NULL;
    int limit = 0;
    Clique *clique = NULL;
    jsonFile *curFile = NULL;
    jsonFile *ptr = NULL;
    jsonFile *neg_file1 = NULL;
    jsonFile *neg_file2 = NULL;
    double *curFileArray = NULL;
    double *X = NULL;
    double z = 0.0;
    int pCounter = 0;
    int nCounter = 0;

    //create tree for pairs
    rbTree* tree_ptr;

    tree_ptr = createTree();



    //to store the results that pass the requirements
    //transitivityPair pairs[HTsize];
    transitivityPair *filePair = NULL;
    jsonFile *leftJson = NULL;
    jsonFile *rightJson = NULL;
    printf("PAME\n");

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

                    if(clique->cliqueSum != 1 && clique->printed!= 1){

                        
                      
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

                                //get the model predection
                                z = calculateZ(X, cls);
                                //printf("PREDICTION %f %s %s\n", z,ptr->site,curFile->site);

                                //check if the prediction is in the range we want
                                if(z > (1 - threshold)){

                                    //printf("adding positive %f\n", z);

                                    //count the pair
                                    pCounter++;

                                    //create 2 jsonFiles with the data
                                    leftJson = createRedusedJsonFile(curFile->site, curFile->JsonWordCount);
                                    rightJson = createRedusedJsonFile(ptr->site, ptr->JsonWordCount);
                                    //create the pair
                                    filePair = createTransitivityPair(leftJson, rightJson, z, X, 1);
                                    
                                    //printf("->>>>>>>>>>>>>>>>>>> %s______%s \n",filePair->leftJson->site,filePair->rightJson->site);
                                    //add the pair to the tree
                                    insertTree(&tree_ptr->root,filePair);
                                }

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
                        

                        
                        clique->printed = 1;

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
                                temp_neg_node->pair = 1;
                            }
                            temp_neg_node = temp_neg_node->next_ptr;
                        }

                        if(cur_neg->pair == 0)
                        {
                            //mark as visited
                            cur_neg->pair = 1;

                            //update cur_neg list with pairs
                            temp_neg_node = clique->neg_node_list;
                            while(temp_neg_node != NULL){
                                if(!strcmp(temp_neg_node->neg_clique_ptr->name, cur_neg->neg_clique_ptr->name)){
                                    temp_neg_node->pair = 1;
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

                                    //get the model predection
                                    z = calculateZ(X, cls);
                                    //printf("NEGATIVE PREDICTION %f %s %s\n", z,neg_file2->site,neg_file1->site);
                                    
                                    //if its in the range we want
                                    if(z < threshold){

                                        //create 2 jsonFiles with the data
                                        leftJson = createRedusedJsonFile(neg_file1->site, neg_file1->JsonWordCount);
                                        rightJson = createRedusedJsonFile(neg_file2->site, neg_file2->JsonWordCount);
                                        //create the pair
                                        filePair = createTransitivityPair(leftJson, rightJson, z, X, 0);
                                        
                                        printf("->>>>>>>>>>>>>>>>>>> %s______%s \n",filePair->leftJson->site,filePair->rightJson->site);
                                        //add the pair to the tree
                                        insertTree(&tree_ptr->root,filePair);

                                        //count it
                                        nCounter++;

                                    }
                                    
                                    
                                    
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
    printTree(tree_ptr->root);
    //Print results
    printf("Positive pairs: %d\n", pCounter);
    printf("Negative pairs: %d\n", nCounter);


}