#ifndef JSONSTRUCT
#define JSONSTRUCT

#include<stdio.h>
#include "bagOfWords.h"

typedef struct wordList wordList;
typedef struct word_ht word_ht;

//this struct contains a json category
typedef struct jsonNode
{
    //key
    char *key;
    //key's description
    char *desc;
    //pointer to the next node
    struct jsonNode *next;

}jsonNode;

//functions
jsonNode* createJsonNode(char *key, char *desc);
void freeJsonNode(jsonNode *node);

//this struct contains all the info of a json file
typedef struct jsonFile
{
    
    //pointer to head
    //head key is the json title
    jsonNode *head;

    //site
    char *site; 
    
    //node counter
    int counter;

    //pointer to tail
    jsonNode* tail;

    //pointer to next jsonFILE
    struct jsonFile *next;

    //flag to see if i have already print it 
    int print_flag; 

    //list with all the words in the jsonFile
    wordList *words;

    //Array with word count
    double *JsonWordCount;

}jsonFile;


jsonFile* createJsonFile(char *site);
jsonFile* createRedusedJsonFile(char *site, double *wordCount);
int insertJsonFile(jsonFile *list, jsonNode *node);
int insertJsonFileHead(jsonFile *list, jsonNode *node);
jsonFile* createAndInsertJsonNode(jsonFile *list, char *key, char *desc);
void jsonFilePrint(jsonFile *list);
void freeJsonFile(jsonFile *list);
void freeJsonNodeList(jsonFile *list);
void freeJsonWordCountArray(jsonFile *json);
void textFormating(jsonFile *file, word_ht *ht);
void CreateJsonListWordCountArray(jsonFile *list, int arraySize);
void FillJsonWordCountArray(jsonFile *list, int arraySize,word_ht *wordHash );
void FreeJsonListWordCountArray(jsonFile *list, int arraySize);

//recordList node
typedef struct recordNode{

    //pointer to jsonFile
    jsonFile *file;

    //pointer to next
    struct recordNode *next;

}recordNode;

//all the json files are stored here
typedef struct recordList{

    //node counter
    int counter;

    //head pointer
    recordNode *head;
    //tail pointer
    recordNode *tail;


}recordList;

recordNode* createRecordNode(jsonFile *file);
recordList* createRecordList();
void freeRecordNode(recordNode * node);
void freeRecordList(recordList *list);

int insertRecordList(recordList * list, recordNode *node);

//parcer function
jsonFile* parcer(FILE *fp, char *site, char *temp, word_ht *ht);
//sub-folder counter
int folderCounter(char *path, int mode);
int countFiles(char *path);
//TF calculator for all jsnoFiles
void calculateRecordListTF(recordList *list);
//printing function
void recordListPrint(recordList *recList);

#endif

