#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <dirent.h>
#include <unistd.h>
#include "jsonStruct.h"
#include "hashTable.h"
#include "bagOfWords.h"
#include "sparce.h"
#include "logisticRegression.h"
#include "jobScheduler.h"
#include <pthread.h>
//#include <threads.h>


#include <math.h>

#define BUCKETSIZE 120
#define SIZE 5000

int my_run()
{
    return rand();
}


void worker(void *arg)
{
    // int *val = arg;
    // int  old = *val;

    Arguments *temp = arg;
    sparceMatrix *val = temp->argv[0];
    sparceNode* cur = val->head;
    int num = *(int*)temp->argv[1];
    int counter = 0;

    while(cur!=NULL)
    {
        printf("%d--%ld\n",cur->index,pthread_self());
        counter += cur->index;
        if(cur->index == 5)
        sleep(1);
        cur = cur->next;
    }
    printf("-------------->%d\n",num);
    // static __thread int old = 5;

    // *val += 1000;
    // printf("tid=%ld, old=%d, val=%d\n", pthread_self(), old, *val);
    //printf("value = %d , %ld\n",old,pthread_self());
    //old+=10;
    //temp->argv[0] = &old;
    //printf("New value = %d, %ld \n",old,pthread_self());

}



int main(int argc,char *argv[]){

    sparceMatrix * matrix = createSparceMatrix();
    for(int i = 0; i < 10; i++)
    {
        insertMatrixNode(matrix,i,(i*0.1));
    }

    jobScheduler *jb;
    Job *job;
    Arguments *args;

    jb = initialise_jobScheduler(2);

    printf("threads\n");
    int j = 0;
    for (int i = 0; i<5; i++) {

        args = malloc(sizeof(Arguments));
        args->argc = 1;
        args->argv = malloc(sizeof(void*)*2);
        args->argv[0] = matrix;
        args->argv[1] = &i;
        printf("edw %d\n", i);
        
        job = create_job(worker,args);
        

        // vals[i] = i
        // sleep(1);
        queueInsert(jb,job);
        //printf("after %d\n",*(int *)args->argv[0]);
    }
    
    sleep(1);
    JobSchedulerWait(jb);

    printf("After wait\n");

    return 0;
    
    //arguments
    char *datasetX,*datasetCSV;
    //paths
    char path[300];
    char shopPath[300];
    char jsonPath[300];
    //folder manipulation
    struct dirent *de,*dx,*fe;
    DIR *subdir = NULL;
    DIR *shopDIR = NULL;
    //file manipulation
    FILE *fp = NULL;
    //STRUCTS
    //list with all the jsonFiles
    recordList *recList = NULL;
    //hashTables
    Bucket** eShopHash = NULL;
    word_ht* wordHash = NULL;
    //jsonFile pointer
    jsonFile *jsonPtr = NULL;
    //recordNode pointer
    recordNode *recNode = NULL;
    //machine learnign model
    logistic_reg *classifier = NULL;
    //counters
    int fileCounter = 0;
    int count1 = 0;
    //csv file lines
    int lines = 0;
    //size
    int eShopHashSize = 0;
    //hashing
    int hashResult = 0;

    //fix the args to the correct values
    int index = 1;
    while (index < argc){
        if(!strcmp(argv[index], "-x"))
            datasetX = strdup(argv[index + 1]);
        else if(!strcmp(argv[index], "-c"))
            datasetCSV = strdup(argv[index + 1]);
        index+= 2;
    }

    //Create the list for the records
    recList = createRecordList();

    //create the eshop hashTable
    //bucket size is 512 bytes
    //the size of the hashTable is 70% more from the folders inside

    //get number of folders (4 is the code for folders)
    count1 = folderCounter("Datasets/2013_camera_specs", 4);
    //get the number of jsonFiles (8 is the code for files)
    count1 = countFiles("Datasets/2013_camera_specs");
    //get hash table size
    eShopHashSize = count1 / 2 + count1;
    //printf("HashTable Size: %d\n", eShopHashSize);
    //create the hash table
    eShopHash = initHashTables(eShopHashSize, BUCKETSIZE);
    //create single word hash table
    wordHash = create_word_ht(SIZE);

    //fix the path
    snprintf(path, 20, "./%s", datasetX);
    //printf("path %s\n",path);

    // Open the folder Dataset
    DIR *dr = opendir(path);
    if(dr == NULL){
        printf("Error Opening Directory\n");
        exit(1);
    }
    //count all subfolders (-2 cause it counts 2 more)
    int dirnumber = -2;
    while ((de = readdir(dr)) != NULL)
        dirnumber++;
    //printf("the items in the folder are %d\n",dirnumber);
    rewinddir(dr);

    //Clean the Path
    memset(path,0,sizeof(path));
    printf("Traversing Dataset folders.\n");
    //open the folder DatasetX
    int count = 0;
    while ((de = readdir(dr)) != NULL && count == 0){
        if(strcmp(de->d_name,".") != 0 && strcmp(de->d_name,"..") != 0){
            //fix new path
            snprintf(path, 300, "./%s/%s", datasetX, de->d_name);

            //if it's not a folder skip it
            if(de->d_type != 4){
                continue;
            }

            //traverse the files inside
            subdir = opendir(path);
            if(subdir == NULL){
                perror("Dir open");
                exit (-1);
            }
            //Read all the shops
            while ((dx = readdir(subdir)) != NULL ){
                if(strcmp(dx->d_name,".") != 0 && strcmp(dx->d_name,"..") != 0){
                    // printf("%s\n",dx->d_name);
                    snprintf(shopPath,sizeof(shopPath),"%s/%s",path,dx->d_name);

                    //Read the json files
                    shopDIR = opendir(shopPath);
                    if(shopDIR == NULL){
                        perror("Dir open");
                        exit (-1);
                    }
                    printf("Opening folder: %s.\n", dx->d_name);
                    //traverse the files
                    while ((fe = readdir(shopDIR)) != NULL ){
                        if(strcmp(fe->d_name,".") != 0 && strcmp(fe->d_name,"..") != 0){
                            
                            //fix the path for fopen
                            snprintf(jsonPath, sizeof(jsonPath), "%s/%s", shopPath, fe->d_name);
                            //printf("%s\n", jsonPath);

                            fp = fopen(jsonPath, "r");
                            if(fp == NULL){
                                perror("fopen");
                                exit(-2);
                            }
                            
                            //function to read json and create a struct
                            jsonPtr = parcer(fp, dx->d_name, fe->d_name, wordHash);

                            //create a recordNode
                            recNode = createRecordNode(jsonPtr);

                            //add the recorNode to the recordList
                            insertRecordList(recList, recNode);

                            //add the json to the correct eshop bucket
                            hashResult = hashing1(jsonPtr->site, eShopHashSize);
                            //printf("HASH %d\n", res);
                            eShopHash[hashResult] = addToHashTable(eShopHash[hashResult], jsonPtr->site, hashResult, jsonPtr);
                            
                            //return 0;
                            fileCounter++;

                            //close the file after work
                            fclose(fp);

                        }

                        
                    }
                    
                    //close the current shopDir
                    closedir(shopDIR);
                }
            }

            closedir(subdir);
        }
    }

    closedir(dr);   

    //Read the csv file
    parcerCSV(datasetX,datasetCSV, eShopHash, eShopHashSize,1);
    // print_cliques(eShopHash,eShopHashSize);
    
    //Get the number of lines in the file 
    lines = GetCSVlineCount(datasetX,datasetCSV);

    //negative clique check
    NegativeQliqueTest(datasetX, datasetCSV, eShopHash, eShopHashSize);

    //print_cliques(eShopHash, eShopHashSize);
    //printf("Total: %d\n", fileCounter);
    printf("\nThere are %d unique .json files in the Dataset.\n", recList->counter);
    printf("\nThere are %d unique words in all the files.\n", wordHash->id_counter + 1);

    //export the negative clique file
    //printNegativeCliques(eShopHash, eShopHashSize);
    
    //calculate TF score for each word in the jsonFiles
    calculateRecordListTF(recList);
    
    //calculate the IDF for each word in the hashTable
    CalculateIDF(recList,wordHash, recList->counter);
    //calculate TF-IDF for each word in the hashTable
    //its a MO of each TFIDF from each jsonFile

    //remove words with tf-idf under 0.002
    //0.00034
    deleteWordsWithCount(wordHash, 0.002);
    //re-apply the wordIds after the delete
    reCalculateHashTableWordId(wordHash);
    //wordHashPrint(wordHash);
    //findMO(wordHash);
    printf("\nTF-IDF arrays feeded to the classifier have %d spaces.\n", wordHash->id_counter);
    
    //split the data from the csv file as following
    //60 % train data
    //20 % test data
    //20 % validation data
    //initialize the classifier
    //fit the train data and train
    //return the classifier struct
    classifier = CreateTrainAndTest(datasetX,datasetCSV,eShopHash,eShopHashSize, wordHash, lines, 512);
    //get metrics for the program
    
    //print the classifier data to a .txt file
    printf("\nPrinting the classifier info to a file.\n");
    printClassifier(classifier);

    printf("\nEnd of the program.\n");
    printf("\nFreeing used memory.\n");
    //work is done
    //time to free memory
    setCliquePointersToNull(eShopHash, eShopHashSize, recList, eShopHashSize);
    //free the list of jsonFiles
    freeRecordList(recList);
    //delete the hashTable
    freeBuckets(eShopHash, eShopHashSize);
    freeWordHT(wordHash);
    //free variables
    free(datasetCSV);
    free(datasetX);
    //free the classifier
    freeLogisticRegressor(classifier);

    return 0;
}
