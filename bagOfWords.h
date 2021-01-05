#ifndef BAGOFWORDS
#define BAGOFWORDS

#include <stdio.h>
#include "jsonStruct.h"

typedef struct recordList recordList;

//node containing
typedef struct wordNode{
    //word
    char *word;
    //pointer to next
    struct wordNode *next;
    //word count in the jsonFile
    int counter;
    //tf score of the word
    double tf;

}wordNode;


//list containing all the words in a jsonFile
typedef struct wordList{

    //head pointer
    wordNode *head;
    //total word count
    int counter;
}wordList;


typedef struct pair_node
{

    //word
    char* word;
    //word counter
    int counter;
    //word_id
    int word_id;
    //number of files containing word
    int fileCounter;
    //tfidf score of word
    double tfidf;
    //idf score
    double idf;
    //next
    struct pair_node * next;
    

}pair_node;

typedef struct word_ht
{
    //id_counter -1 is the total word count(without same words)
    int id_counter;
    pair_node **array;
    int size;
}word_ht;

//FUNCTIONS
wordNode* createWordNode(char *key);
wordList* createWordList();
int insertWordNode(wordList *list, wordNode *node);
void freeWordNode(wordNode *node);
void freeWordList(wordList *list);
void insert_pair_hash_table(word_ht* pair_ht, char* word);
void calculateTF(wordList *list);
pair_node** create_word_array(int size);
word_ht* create_word_ht(int size);
int searchWordHashTable(word_ht *ht, char *key);
double searchWordHashTablePtr(word_ht *ht, char *key, int *p);
void reCalculateHashTableWordId(word_ht *ht);
void addWordHashFileCounter(word_ht *ht, char * key);
void deleteWordsWithCount(word_ht *ht, double key);
void findMO(word_ht *);
void wordHashPrint(word_ht *hashTable);
void freeWordHT(word_ht *ht);

void CalculateIDF(recordList *list, word_ht *wordHash, int size);

#endif