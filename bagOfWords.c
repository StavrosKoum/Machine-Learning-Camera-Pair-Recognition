#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "bagOfWords.h"
#include "hashTable.h"
#include "jsonStruct.h"
#include <limits.h>

//constructor for jsonNode object
wordNode* createWordNode(char *key){

    wordNode *node = NULL;

    //create a node    
    node = malloc(sizeof(wordNode));
    //error check
    if(node == NULL){
        perror("malloc");
        exit(-3);
    }

    //assign key
    node->word = strdup(key);
    //set the next to null
    node->next = NULL;
    //set counter to one
    node->counter = 1;
    //set tf score to zero 
    node->tf = 0.0;
    //return the node
    return node;
}

//constructor for wordList object
wordList* createWordList(){

    wordList *list = NULL;

    //allocate space
    list = malloc(sizeof(wordList));

    //set pointers
    list->head = NULL;

    //init the counter
    list->counter = 0;

    //return it
    return list;

}
//inserts a wordNode to the head of wordList
int insertWordNode(wordList *list, wordNode *node){

    //search if the word already exists
    wordNode *tmp_node;
    tmp_node = list->head;
    while(tmp_node!=NULL)
    {
        //if the word already exists
        if(!strcmp(tmp_node->word,node->word))
        {
            //add one to counter
            tmp_node->counter++;

            //free the node we dont need
            freeWordNode(node);
            return 0;
        }
        tmp_node = tmp_node->next;
    }

    //make node point to head
    node->next = list->head;
    //change head pointer
    list->head = node;
    //update counter
    list->counter += 1;    
    return 1;
}

//delete a wordNode object
void freeWordNode(wordNode *node){

    //free the key
    free(node->word);
    //free the node
    free(node);

}

//delete the wordList
void freeWordList(wordList *list){

    //to free the nodes
    wordNode *cur = NULL;
    wordNode *prev = NULL;

    cur = list->head;
    prev = list->head;

    //traverse and free the nodes
    while(cur != NULL){

        //make prev cur
        prev = cur;
        //move cur
        cur = cur->next;
        //free prev
        freeWordNode(prev);
    }

    //free jsonFile
    free(list);

}

//takes a wordList and calculates the tf score for each word inside
void calculateTF(wordList *list){

    //pointer to wordNode
    wordNode *cur = NULL;

    //set pointer to head
    cur = list->head;

    //traverse the list
    while(cur != NULL){
        //calculate tf
        cur->tf = (double) cur->counter / list->counter;
        //go to the next node
        cur = cur->next;
    }

}

pair_node** create_word_array(int size)
{
    pair_node** ptr;
    ptr = malloc(size * sizeof(pair_node*));

    //init the array
    for(int i = 0; i < size; i++)
    {
        ptr[i] = NULL;
    }

    return ptr;
}

word_ht* create_word_ht(int size)
{
    word_ht* ht;
    ht = malloc(sizeof(word_ht));
    ht->array = create_word_array(size);
    ht->id_counter = 0;
    ht->size = size;
    return ht;
    
}

void insert_pair_hash_table(word_ht* pair_ht, char* word)
{
    int hash = hashing1(word,pair_ht->size);
    pair_node * ptr = NULL;
    pair_node *node = NULL;

    //get the cell the word must go in
    ptr = pair_ht->array[hash];
    //if its empty
    if( ptr==NULL)
    {
        //create node and insert word
        pair_ht->array[hash] = malloc(sizeof(pair_node));
        pair_ht->array[hash]->word = strdup(word);
        pair_ht->array[hash]->counter = 1;
        pair_ht->array[hash]->next = NULL;
        pair_ht->array[hash]->word_id = pair_ht->id_counter;
        pair_ht->array[hash]->fileCounter = 0;
        pair_ht->id_counter += 1;
        pair_ht->array[hash]->idf = 0.0;
        pair_ht->array[hash]->tfidf = 0.0;
        return;
    }
    //if not
    //add it to the end
    while(ptr !=NULL )
    {   
        //if you found this word return and ++ counter
        if(!strcmp(ptr->word,word))
        {
            //if(strlen(word) == 2)   printf("FOUND those two <<%s>>-------<<%s>>\n",ptr->word,word);
            ptr->counter += 1;
            return;
        }
        ptr = ptr->next;
    }
    //create node
    node = malloc(sizeof(pair_node));
    node->word = strdup(word);
    node->counter = 1;
    node->word_id = pair_ht->id_counter;
    node->fileCounter = 0;
    node->idf = 0.0;
    node->tfidf = 0.0;
    pair_ht->id_counter += 1;
    // pair_ht->array[hash]->idf = 0.0;
    // pair_ht->array[hash]->tfidf = 0.0;
    //add node to hash table
    node->next = pair_ht->array[hash];
    pair_ht->array[hash] = node;
}

//searches for key in the hashTable and returns the word_id
//  -1 -> not found
int searchWordHashTable(word_ht *ht, char *key){

    //position
    int pos = 0;
    //word node pointer
    pair_node *cur = NULL;
    //hash the key
    pos = hashing1(key, ht->size);
    //get the array cell
    cur = ht->array[pos];
    //search for the word
    while(cur != NULL){
        //if you found it
        if(!strcmp(key, cur->word)){
            //return its word_id
            return cur->word_id;
        }

        //go to the next node
        cur = cur->next;
    }

    //if we reach this point the word was not found
    return -1;
}

//search for a word and return its idf along with it's word_id
//if its not found, return null
double searchWordHashTablePtr(word_ht *ht, char *key, int *p){

    //position
    int pos = 0;
    //word node pointer
    pair_node *cur = NULL;
    //hash the key
    pos = hashing1(key, ht->size);
    //get the array cell
    cur = ht->array[pos];
    //search for the word
    while(cur != NULL){
        //if you found it
        if(!strcmp(key, cur->word)){
            //update p with word_id
            *p = cur->word_id;
            //return word idf
            return cur->idf;
        }

        //go to the next node
        cur = cur->next;
    }

    //if we reach this point the word was not found
    //return null
    return -1.0;
}

//function that re adds word_ids
//they range between 0 - # of words -1 
void reCalculateHashTableWordId(word_ht *ht){

    //to provide ids
    int id = 0;
    //cell pointer
    pair_node *cur = NULL;

    //traverse the hashTable
    for(int i = 0; i < ht->size; i++){

        //get cell
        cur = ht->array[i];

        //traverse its list
        while(cur != NULL){
            //fix the word id
            cur->word_id = id;
            //adjust id
            id += 1;

            //go to next cur
            cur = cur->next;
        }

        //set cur to null
        cur = NULL;
    }

    //set the number to the ht 
    ht->id_counter = id;

}

//find the word key and add 1 to the fileCounter
void addWordHashFileCounter(word_ht *ht, char * key){

    pair_node *cur = NULL;
    //hash value
    int pos = 0;

    //hash the key
    pos = hashing1(key, ht->size);
    //get the array cell
    cur = ht->array[pos];
    //search for the word
    while(cur != NULL){
        //if you find it
        if(!strcmp(key, cur->word)){
            //add 1 to the fileCounter
            cur->fileCounter += 1;
            //work done
            return;
        }

        //go to the next node
        cur = cur->next;
    }

    //if we reach this point the word was not found
    printf("elousa elousa elousa\n");
    return;

}

void freePair_node(pair_node *node){

    //free the word
    free(node->word);
    //free the node
    free(node);

}

void deleteWordsWithCount(word_ht *ht, double key){

    //cell pointer
    pair_node *cur = NULL;
     pair_node *del = NULL;
    //used to remove elements
    pair_node *prev = NULL;
    int flag = 0;
    int count = 0;
    

    //traverse the hashTable
    for(int i = 0; i < ht->size; i++){

        //get cell
        cur = ht->array[i];
        prev = cur;

        prev = ht->array[i];
        while(cur != NULL)
        {
            flag = 0;
            //if we need to remove it
            if(cur->tfidf < key)
            {
                if(cur == ht->array[i])
                {
                    count ++;
                    ht->array[i] = cur->next;
                    prev = cur;
                    freePair_node(prev);
                    cur = ht->array[i];
                    flag = 1; //so it will skip the cur = cur->next

                }
                else
                {
                    count ++;
                    prev->next = cur->next;
                    del = cur;
                    freePair_node(del);
                    cur = prev; //so it will go to new node later
                }
                
                
            }

            
                
            if (flag==0)
            {
                prev = cur;
                cur = cur->next;
            }
            
        }
       
    }

}



void findMO(word_ht *hashTable){

    double sum = 0.0;
    double max = -1.0;
    double min = 5000.0;
    int over100 = 0;
    pair_node *cur = NULL;
    pair_node *temp = NULL;

    //traverse the ht
    for(int i = 0; i < hashTable->size; i++){
        cur = hashTable->array[i];

        while(cur != NULL){
            sum += cur->idf;
            
            if(cur->idf > max){
                max = cur->idf;
                temp = cur;
            }      

            if(cur->idf < min){
                min = cur->idf;
            }      

            if(cur->idf == 1){     
                over100++;
            }

            cur = cur->next;
        }
    }

    printf("MO: %f\n", sum / (hashTable->id_counter + 1));
    printf("MIN: %f     MAX: %f\n", min, max);
    printf("MAX word: %s\n", temp->word);
    printf("SUM = %f\n", sum);
    printf("Under 1: %d\n", over100);
}

void wordHashPrint(word_ht *hashTable){

    pair_node *cur = NULL;

    //for each bucket
    for(int i = 0; i < hashTable->size; i++){
        cur = hashTable->array[i];

        while(cur != NULL){

            printf("%d WORD: %s, COUNT: %d, TFIDF %10.8f\n", cur->word_id, cur->word, cur->counter, cur->tfidf);

            cur = cur->next;
        }
    }

}


// Calculate IDF value
void CalculateIDF(recordList *reclist, word_ht *wordHash, int size){

    pair_node *cur = NULL;
    int totalRecords = reclist->counter;
    double temp = 0.0;

    //test max tfidf
    double max = -1.0;

    for(int i = 0; i < wordHash->size; i++){
        cur = wordHash->array[i];

        while(cur != NULL){

            temp = (double) totalRecords / cur->fileCounter;
            // printf("TEMP %f  CUR->FILECOUNTER %d   TOTAL RECORDS %d\n",temp,cur->fileCounter,totalRecords);

            cur->idf = log10(temp);
            //printf("!!! IDF %f\n",cur->idf);

            cur = cur->next;
        }
    }

    wordNode* tmp = NULL;
    recordNode *curRec = NULL;
    pair_node *nodecur = NULL;
    
    curRec = reclist->head;
    while(curRec != NULL){
        tmp = curRec->file->words->head;
        while (tmp!=NULL){
            // printf("------------%s TF -> %6.3f\n",tmp->word,tmp->tf);
            // printf("TOTAL %d, WORD %d\n", curRec->file->words->counter, tmp->counter);


            int pos = 0;
            //word node pointer
            nodecur = NULL;
            //hash the key
            pos = hashing1(tmp->word, wordHash->size);
            //get the array cell
            nodecur = wordHash->array[pos];
            //search for the word
            while(nodecur != NULL){
                //if you found it
                if(!strcmp(tmp->word, nodecur->word)){
                    
                    //printf("TF %10.8f   IDF %10.8f  TFIDF %10.8f    %s  TOTAL %10.8f\n", tmp->tf, nodecur->idf, (tmp->tf * nodecur->idf / size), nodecur->word, nodecur->tfidf);

                    if((tmp->tf * nodecur->idf / size) > max){
                        max = (tmp->tf * nodecur->idf / size);
                    }
                    
                    nodecur->tfidf = nodecur->tfidf + (tmp->tf * nodecur->idf / size);

                    //printf("TD/IDF is %f\n",nodecur->tfidf);
                    break;
                }

                //go to the next node
                nodecur = nodecur->next;
            }



            tmp = tmp->next;
        }

        curRec = curRec->next;
    }

    return;
}


void freePairNodeList(pair_node *node){

    //used in free
    pair_node *cur = NULL;
    pair_node *prev = NULL;

    cur = node;
    prev = node;

    //traverse the nodes
    while(cur != NULL){

        //make prev cur
        prev = cur;
        //move cur
        cur = cur->next;
        //free prev word
        free(prev->word);
        //free prev
        free(prev);
    }
}

void freeWordHT(word_ht *ht){

    //traverse the array
    for(int i = 0; i < ht->size; i++){

        //free each pair_node list
        freePairNodeList(ht->array[i]);

    }

    //free the array
    free(ht->array);
    //free the HT
    free(ht);

}