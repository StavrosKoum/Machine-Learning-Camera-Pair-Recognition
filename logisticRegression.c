#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "logisticRegression.h"
#include "metrics.h"
#include "time.h"

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
            for(int k = 0; k < 10; k++){

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
logistic_reg* logisticRegretionAlgorithm(logistic_reg *cls, int limit, Bucket **ht, int HTsize, word_ht *wordHash){

    double threshold = 0.05;
    double step = 0.10;
    //do the following steps until the threshold
    while(threshold < 0.5){
            
        //calculate the new weights
        cls->weights = gradient_descend(cls);

        predictHashTable(cls, ht, HTsize, threshold, wordHash);
        exit(-5);
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
    srand(time(NULL));
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
double ** predictHashTable(logistic_reg *cls, Bucket ** ht, int HTsize, double threshold, word_ht *wordHash){

    Bucket *cur = NULL;
    int limit = 0;
    Clique *clique = NULL;
    jsonFile *curFile = NULL;
    jsonFile *ptr = NULL;
    double *curFileArray = NULL;
    double *X = NULL;
    double z = 0.0;

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

                    //first, traverse all the positive files
                    curFile = clique->file;

                    printf("%s          %d\n", clique->name, clique->cliqueSum);

                    while(curFile != NULL && clique->cliqueSum != 1){

                        //get tf-idf representation of curFile
                        CreateJsonListWordCountArray(curFile,wordHash->id_counter);
                        FillJsonWordCountArray(curFile,wordHash->id_counter,wordHash);
                        //init ptr, X
                        ptr = NULL;
                        X = NULL;
                        //traverse all the nodes in the list
                        for(int j = 0; j<clique->cliqueSum -1; j++)
                        {
                            ptr = curFile->next;
                            //traverse the nodes after the current
                            for(int u = j; u < clique->cliqueSum -1; u++)
                            {
                                //get the tf-idf representation of each file
                                CreateJsonListWordCountArray(ptr,wordHash->id_counter);
                                FillJsonWordCountArray(ptr,wordHash->id_counter,wordHash);
                                
                                //concat the two arrays
                                X = arrayConcat(curFile->JsonWordCount, ptr->JsonWordCount, wordHash->id_counter);

                                //get the model predection
                                z = calculateZ(X, cls);
                                printf("PREDICTION %f\n", z);
                                ptr = ptr->next;
                            }

                        }

                        //free arrays
                        free(curFile->JsonWordCount);
                        if(ptr != NULL){
                            free(ptr->JsonWordCount);
                        }
                        if(X != NULL){
                            free(X);
                        }

                    }

                }

            }

            cur = cur->next;
        }

    }    

}