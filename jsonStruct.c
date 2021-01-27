#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "jsonStruct.h"

//constructor for jsonNode object
jsonNode* createJsonNode(char *key, char *desc){

    jsonNode *node = NULL;

    //create a node    
    node = malloc(sizeof(jsonNode));
    //error check
    if(node == NULL){
        perror("malloc");
        exit(-3);
    }

    //assign key
    node->key = strdup(key);
    //assign desc
    node->desc = strdup(desc);
    //set the next to null
    node->next = NULL;

    //return the node
    return node;
}

//constructor for jsonFile object
jsonFile* createJsonFile(char *site){

    jsonFile *list = NULL;

    //allocate space
    list = malloc(sizeof(jsonFile));

    //set pointers
    list->head = NULL;
    list->tail = NULL;

    //set the site
    list->site = strdup(site);

    //init the counter    int arraySize = 0;
    list->counter = 0;

    //mark next pointer as null
    list->next = NULL;

    //init flag as 0
    list->print_flag = 0;

    //initialize the wordList
    list->words = createWordList();

    list->JsonWordCount = NULL;

    //return it
    return list;

}

jsonFile *createRedusedJsonFile(char *site, double *wordCount){

    jsonFile *list = NULL;

    //allocate space
    list = malloc(sizeof(jsonFile));

    //set pointers
    list->head = NULL;
    list->tail = NULL;

    //set the site
    list->site = strdup(site);

    //init the counter    int arraySize = 0;
    list->counter = 0;

    //mark next pointer as null
    list->next = NULL;

    //init flag as 0
    list->print_flag = 0;
    
    list->JsonWordCount = wordCount;

    return list;
}

//insert function for jsonFile
//inserts a jsonNode
//1 -> sucess | 0 -> failure
int insertJsonFile(jsonFile *list, jsonNode *node){

    //if the list is empty add it as head
    if(list->head == NULL){
        list->head = node;
        //set tail 
        list->tail = node;
        //update counter
        list->counter += 1;
        return 1;
    }

    //add it after the tail
    list->tail->next = node;
    //update tail
    list->tail = node;
    //update counter
    list->counter += 1;

    return 1;
}

//inserts a jsonNode as head
int insertJsonFileHead(jsonFile *list, jsonNode *node){

    //make node point to head
    node->next = list->head;
    //change head pointer
    list->head = node;
    //update counter
    list->counter += 1;    

    //if its the first element update the tail
    if(list->counter == 1){
        list->tail = node;
    }

    return 1;
}

jsonFile* createAndInsertJsonNode(jsonFile *list, char *key, char *desc){

    jsonNode *node = NULL;

    //create the node
    node = createJsonNode(key, desc);
    //add it to the list
    insertJsonFile(list, node);

    //return the list
    return list;

}

//list print function
//prints only the keys
void jsonFilePrint(jsonFile *list){

    jsonNode *cur = NULL;

    //set to head
    cur = list->head;

    //traverse and print
    while(cur != NULL){

        printf("%s\n%s\n", cur->key, cur->desc);

        //move current
        cur = cur->next;

    }

}

//Constructor of recordNode
recordNode* createRecordNode(jsonFile *file){

    recordNode *node = NULL;

    //create a node    
    node = malloc(sizeof(recordNode));
    //error check
    if(node == NULL){
        perror("malloc");
        exit(-3);
    }

    //assign the file
    node->file = file;
    //set next as null
    node->next = NULL;
    //return node
    return node;

}

//Constructor of recordList
recordList* createRecordList(){

    recordList *list = NULL;

    //allocate space
    list = malloc(sizeof(recordList));

    //set pointers
    list->head = NULL;
    list->tail = NULL;

    //init the counter
    list->counter = 0;

    //return it
    return list;

}

//Insert function for recordList
//1 -> sucess
//0 -> failure
int insertRecordList(recordList * list, recordNode *node){

    //if the list is empty add it as head
    if(list->head == NULL){
        list->head = node;
        //set tail 
        list->tail = node;
        //update counter
        list->counter += 1;
        return 1;
    }

    //add it after the tail
    list->tail->next = node;
    //update tail
    list->tail = node;
    //update counter
    list->counter += 1;

    return 1;
}

//adds the node at the end of the jsonFileList
void jsonFileListAppend(jsonFile *list, jsonFile *node){


    jsonFile *cur = list;

    //go to the last node   
    while(cur->next != NULL){
        
        //go to next node
        cur = cur->next;
    }

    //add the node as next of cur
    cur->next = node;
    //no need to mess with node->next as it might have other nodes attached to it
}

//takes a recordList and calculates the TF score for each word inside
void calculateRecordListTF(recordList *list){

    //world list head pointer
    wordList *curWords = NULL;
    //record list head pointer
    recordNode *curNode = NULL;

    //set pointer to head of record list
    curNode = list->head;

    //traverse the list
    while(curNode != NULL){

        //get the wordList from the jsonFile
        curWords = curNode->file->words;
        //calculate TF for all words inside the list
        calculateTF(curWords);

        //go to the next recordNode
        curNode = curNode->next;
    }

}

//print function for the recordList
void recordListPrint(recordList *recList){

    wordNode* tmp = NULL;
    recordNode *cur = NULL;

    cur = recList->head;
    while(cur != NULL){
        tmp = cur->file->words->head;
        printf("FILE %s\n", cur->file->site);
        while (tmp!=NULL)
        {
            printf("------------%s TF -> %6.3f\n",tmp->word,tmp->tf);
            printf("TOTAL %d, WORD %d\n", cur->file->words->counter, tmp->counter);
            tmp = tmp->next;
        }

        cur = cur->next;
    }

}

//DELETE FUNCTIONS

//delete a jsonNode object
void freeJsonNode(jsonNode *node){

    //free the key
    free(node->key);
    //free the desc
    free(node->desc);
    //free the node
    free(node);

}

void freeJsonWordCountArray(jsonFile *json){

    //free the array
    free(json->JsonWordCount);
    //set it as null
    json->JsonWordCount = NULL;
}

void freeJsonFile(jsonFile *list){

    //to free the nodes
    jsonNode *cur = NULL;
    jsonNode *prev = NULL;

    //free the site
    free(list->site);

    //free the wordList
    freeWordList(list->words);
    
    cur = list->head;
    prev = list->head;

    //traverse and free the nodes
    while(cur != NULL){

        //make prev cur
        prev = cur;
        //move cur
        cur = cur->next;
        //free prev
        freeJsonNode(prev);
    }

    //free array
    freeJsonWordCountArray(list);
    //free jsonFile
    free(list);

}

void freeJsonNodeList(jsonFile *list){

    //to free the nodes
    jsonNode *cur = NULL;
    jsonNode *prev = NULL;

    cur = list->head;
    prev = list->head;

    //traverse and free the nodes
    while(cur != NULL){

        //make prev cur
        prev = cur;
        //move cur
        cur = cur->next;
        //free prev
        freeJsonNode(prev);
    }

    //set list pointer as null
    list->head = NULL;
    list->tail = NULL;

}

//free the record node and the jsonFile inside it
void freeRecordNode(recordNode * node){

    //free the file
    freeJsonFile(node->file);
    //free the node
    free(node);

}

//free all the nodes in the list, and then the list itself
void freeRecordList(recordList *list){

    //to free the nodes
    recordNode *cur = list->head; 
    recordNode *prev;   

    //traverse and free the nodes
    while(cur != NULL){

         //make prev cur
        prev = cur;
        //move cur
        cur = cur->next;
        //free prev
        freeRecordNode(prev);
    }

    //free the list
    free(list);
}


void CreateJsonListWordCountArray(jsonFile *list, int arraySize){

    //set to head
    // cur = list->words;

    // //traverse and count
    // while(cur != NULL){
    //     //count each word 
    //     wordCounter++;

    //     //move current
    //     cur = cur->next;

    // }

    //malloc
    list->JsonWordCount = malloc(arraySize * sizeof(double));
    // printf("Word counter array created\n");

    //init the array with 0
    for(int i=0;i<arraySize;i++){
        list->JsonWordCount[i] = 0.0;
    }

    return;
}

//fill the TF-IDF array of a jsonFilewith the tfidf value of each word
void FillJsonWordCountArray(jsonFile *list, int arraySize,word_ht *wordHash ){
    
    wordNode *cur;
    cur = list->words->head;
    int wordPos = 0;
    double idf = 0.0;
    // Search every word in the jsonFile in the hashtable
    // get the words poss in the hash
    while(cur != NULL){
        // get the word
        idf = searchWordHashTablePtr(wordHash,cur->word, &wordPos);        
        // error check
        if(idf != -1.0){
            //printf("word found %s %d TF: %10.8f\n",cur->word,wordPos, cur->tf / idf);
            if(idf == 0.0)    printf("eeeeeeeeeeeeee\n");
            // list->JsonWordCount[wordPos] = (double) cur->tf / idf;
            list->JsonWordCount[wordPos] = (double) cur->counter;
        }
        else{
            // Word not in hashtable
            // printf("Word not found\n");
        }

        cur = cur->next;
    }

    return;
}


void FreeJsonListWordCountArray(jsonFile *list, int arraySize){

    //free the array
    free(list->JsonWordCount);
    
    return;
}


int InsertWordCountArray(jsonFile *list, int arraySize, word_ht *hashtable, char *word){

    //check if the word is on the hashtable
    int flag = 0;

    flag = searchWordHashTable(hashtable,word);

    //if the word isnt on the hashtable
    if (flag == 0){
        // no need to do anything 
        // return -1 for word not found
        return -1;
    }
    else{
        // increase the word count of the given word 
        // return 1 for success
        list->JsonWordCount[flag]++;
        return 1;
    }

}