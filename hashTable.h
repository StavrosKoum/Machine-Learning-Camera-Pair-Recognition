#ifndef HASHTABLES
#define HASHTABLES

#include <stdio.h>
#include "jsonStruct.h"
#include "logisticRegression.h"
#include "graph.h"

typedef struct logistic_reg logistic_reg;

typedef struct bucketNode
{
    char* name;
    Graph* graph;

}bucketNode;


typedef struct bucket{

    int size;
    int spaces;
    struct bucket* next;
    bucketNode** array;


}Bucket;

//struct to keep all the info regarding a train pair for the model
typedef struct transitivityPair{

    jsonFile *leftJson;
    jsonFile *rightJson;
    int result;
    double prediction;
    double *array;

}transitivityPair;

transitivityPair *createTransitivityPair(jsonFile *file1, jsonFile *file2, double p, double *array, int result);
void deletePair(transitivityPair *p);


//functions
Bucket** initHashTables(int maxB, int bSize);
int hashing(char* word, int limit);
int hashing1(char* str, int limit);
Bucket* createBucket(int size);
Bucket* addToHashTable(Bucket* ht, char* key, int position, jsonFile* file);
int searchArray(bucketNode** array ,char* key, int limit);
int bucketArrayPrint(bucketNode** array, int limit,FILE* fp);
void bucketPrint(Bucket* b);
void freeBucketNode(bucketNode* n);
void freeBuckets(Bucket** ht, int stop);
int getElementCount(Bucket** ht, int limit);
int isFull(bucketNode** array, int limit);
Bucket* searchBuckets(Bucket* bucket, char* key);
Clique* findEntry(Bucket** ht, int hash, char *key);
void print_cliques(Bucket** ht, int stop);
void setCliquePointersToNull(Bucket **ht, int stop, recordList *list, int hashSize);

void parcerCSV(char *path, char *csv, Bucket **hashtable, int hashSize,int opt);
void printNegativeCliques(Bucket **ht, int stop);
int check_neg_cliques(Bucket** ht, char* word1, char* word2,int limit);
void NegativeQliqueTest(char *path, char *csv, Bucket **hashtable, int hashSize);
void fixLists(Bucket **hashtable, int hashSize, Clique *toStay, Clique *toMove);
bucketNode* findEntryArrayPointer(Bucket** ht, int hash, char *key);

logistic_reg* CreateTrainAndTest(char *path,char *csv,Bucket** ht,int hashSize, word_ht *wordHash, int size80,int batch_size);
jsonFile* findJsonFromCsv(Bucket** ht,int hash, char *key);
int GetCSVlineCount(char *path,char *csv);
double* arrayConcat(double *startArray, double *endArray, int size);


#endif