#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "hashTable.h"
#include "graph.h"
#include "jsonStruct.h"
#include "logisticRegression.h"
#include "metrics.h"

int hashing(char* word, int limit){

    char* ptr = word;
    int count = 0;
    int sum = 0;
    int temp;

    for(ptr = word; *ptr != '\0'; ptr++){
        count++;
        temp = *ptr;
        sum += temp;
    }
    sum = sum >> count;

    return sum % limit; 

}

int hashing1(char *str, int limit){

    unsigned long hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash % limit;
}


Bucket** initHashTables(int maxB, int bSize){

    Bucket **d;

    //allocate the array
    d = malloc(maxB * sizeof(Bucket *));
    if(d == NULL){
        perror("Error");
        return NULL;
    }

    //allocate a bucket for each spot
    for (int i = 0; i < maxB; i++)
    {
        d[i] = malloc(sizeof(Bucket));
        d[i]->size = bSize;
        d[i]->next = NULL;
        //see how many spaces you have
        d[i]->spaces = (bSize - sizeof(Bucket)) / sizeof(bucketNode);
        //allocate the remaining space for the array
        d[i]->array = malloc(bSize - sizeof(Bucket));
        //set each cell to NULL
        for (int j = 0; j < d[i]->spaces; j++)
        {
            d[i]->array[j] = NULL;
        }
        //printf("SPACES %d, RAW SPACE %ld SIZEOF-bucketNode %ld\n", d[0]->spaces, bSize - sizeof(Bucket), sizeof(bucketNode));

    }
    return d;
}


Bucket* createBucket(int size){

    Bucket* temp = NULL;
    
    temp = malloc(sizeof(Bucket));
    if(temp == NULL){
        perror("Malloc");
        exit(-1);
    }

    //set pointer
    temp->next = NULL;
    //set size
    temp->size = size;
    //set space
    temp->spaces = (size - sizeof(Bucket)) / sizeof(bucketNode);
    //allocate an array of pointers
    temp->array = malloc(size - sizeof(Bucket));
    //set nulls
    for (int i = 0; i < temp->spaces; i++)
    {
        temp->array[i] = NULL;
    }
    
    return temp;

}

transitivityPair *createTransitivityPair(jsonFile *file1, jsonFile *file2, double p, double *array, int result){

    transitivityPair *pair = NULL;
    //allocate space
    pair = malloc(sizeof(transitivityPair));
    if(pair == NULL){
        perror("pair malloc");
        exit(-3);
    }

    //set data
    pair->leftJson = file1;
    pair->rightJson = file2;
    pair->result = result;
    pair->prediction = p;
    pair->array = array;

    //return it
    return pair;
}   

//searches array for the key. -1 -> not found
int searchArray(bucketNode** array ,char* key, int limit){

    //search the bucket array
    for(int i = 0; i < limit; i++){  
        //if it exists
        if(array[i] != NULL){
            //found it
            if(strcmp(array[i]->name, key) == 0){
                return i;
            }
        }
    }
    //else return -1
    return -1;

}

bucketNode* createBucketNode(jsonFile* entry, char* key){

    bucketNode* temp = NULL;
    
    temp = (bucketNode* ) malloc(sizeof(bucketNode));
    if(temp == NULL){
        perror("Malloc");
        exit(-1);
    }

    temp->name = strdup(key);
    temp->graph = createGraph();

    return temp;

}


Bucket* searchBuckets(Bucket* bucket, char* key){

    Bucket* start = bucket;
    int res = 0;
    //travesrse all the buckets
    while(start != NULL){
        //it is in the bucket
        res = searchArray(start->array, key, start->spaces);
        //printf("RESULT %d\n", res);
        if(res != -1){
            return start;
        }
        //go to next bucket
        start = start->next;
    }

    return NULL;
}


//check an array if it's full or not. -1 -> not full
int isFull(bucketNode** array, int limit){

    for(int i = 0; i < limit; i++){
        if(array[i] == NULL)
            return i;
    }

    return -1;
}

Bucket* addToHashTable(Bucket* b, char* key, int position, jsonFile* entry){

    Bucket* bucketPtr = NULL;
    Bucket* bucketToGo = NULL;
    Bucket* head = b;

    int index;
    //add it to the correct bucket
    bucketPtr = b;

    //search buckets for key
    bucketToGo = searchBuckets(bucketPtr, key);

    //it isn't in any bucket
    if(bucketToGo == NULL){

        //there is avaiable space
        //add it to the first bucket
        index = isFull(head->array, bucketPtr->spaces);
        if(index != -1){
            head->array[index] = createBucketNode(entry, key);
            graphInsert(head->array[index]->graph, entry);
            return head;
        //we have to go to a new bucket
        }else{
            //create the bucket
            bucketToGo = createBucket(head->size);
            //link it
            bucketToGo->next = head;
            //get the space in the array
            index = isFull(bucketToGo->array, bucketToGo->spaces);
            //create a bucketNode
            bucketToGo->array[index] = createBucketNode(entry, key);
            //insert it and keep the clique to add it to the list
            graphInsert(head->array[index]->graph, entry);
            return bucketToGo;
        }
    //it exists in a bucket
    }else{
        //find the exact space
        index = searchArray(bucketToGo->array, key, bucketToGo->spaces);
        //add it to the tree
        //printf("Already exists. Adding to tree %s\n", bucketToGo->array[index]->name);
        graphInsert(head->array[index]->graph, entry);
        return head;
    }   
           
    return head;
}


void freeBucketNode(bucketNode* n){

    //free name
    free(n->name);
    //free the node
    free(n);

}

void freeBuckets(Bucket** ht, int stop){

    Bucket* cur = NULL;
    Bucket* toFree = NULL;
    int limit;

    //for each bucket
    for(int i = 0; i<stop; i++){

        //set current
        cur  = ht[i];
        toFree = cur;
        //search cur and it's links
        while(cur != NULL){

            //fix limit
            limit = cur->spaces;

            //for each space in the array
            for(int k = 0; k < limit; k++){
                
                //if there is a tree
                if(cur->array[k] != NULL){

                    //free bucketNode
                    freeBucketNode(cur->array[k]);

                }
            }

            //update toFree
            toFree= cur;
            //move cur
            cur = cur->next;
            //free the array
            free(toFree->array);
            //free the bucket
            free(toFree);
        }
    }

    //all buckets have  been freed
    //free the bucketarray

    free(ht);
}

//search for an entry and return the bucket and the clique
Clique* findEntry(Bucket** ht, int hash, char *key){

    //go to the correct bucket
    Bucket* temp = NULL; 
    //to find the correct jsonFile
    temp = ht[hash];

    //find the spot
    Bucket* bucketToGo = NULL;
    bucketToGo = searchBuckets(temp, key);

    if(bucketToGo == NULL){
        return NULL;
    }

    //get the array position
    for(int i =0; i< bucketToGo->spaces; i++){
        //if you find it 
        if(bucketToGo->array[i] != NULL){
            //tf("%s ->", bucketToGo->array[i]->name);
            if(!strcmp(key, bucketToGo->array[i]->name)){
                //printf("FOUND %s\n", key);
                //return the clique
                if(bucketToGo->array[i]->graph != NULL){
                    if(bucketToGo->array[i]->graph->head != NULL){
                        return bucketToGo->array[i]->graph->head;
                    }else{
                        return NULL;
                    }
                }else{
                    return NULL;
                }
            }
        }
    }
    //not found, return NULL
    //printf("NOT FOUND\n");
    return NULL;

}


jsonFile* findJsonFromCsv(Bucket** ht,int hash, char *key){
    //go to the correct bucket
    Bucket* temp = NULL; 
    //to find the correct jsonFile
    temp = ht[hash];

    //find the spot
    Bucket* bucketToGo = NULL;
    bucketToGo = searchBuckets(temp, key);

    if(bucketToGo == NULL){
        return NULL;
    }

    for(int i =0; i< bucketToGo->spaces; i++){
        //if you find it 
        if(bucketToGo->array[i] != NULL){
            //printf("%s ->", bucketToGo->array[i]->name);
            if(!strcmp(key, bucketToGo->array[i]->name)){
                //printf("FOUND %s\n", key);
                //return the clique
                if(bucketToGo->array[i]->graph != NULL){
                    if(bucketToGo->array[i]->graph->head != NULL){
                        return bucketToGo->array[i]->graph->head->file;
                    }else{
                        return NULL;
                    }
                }else{
                    return NULL;
                }
            }
        }
    }

    return NULL;

}

//search and return an hastable array pointer
bucketNode* findEntryArrayPointer(Bucket** ht, int hash, char *key){

    //go to the correct bucket
    Bucket* temp = NULL; 
    //to find the correct jsonFile
    temp = ht[hash];

    //find the spot
    Bucket* bucketToGo = NULL;
    bucketToGo = searchBuckets(temp, key);

    if(bucketToGo == NULL){
        return NULL;
    }

    //get the array position
    for(int i =0; i< bucketToGo->spaces; i++){
        //if you find it 
        if(bucketToGo->array[i] != NULL){
            //printf("%s ->", bucketToGo->array[i]->name);
            if(!strcmp(key, bucketToGo->array[i]->name)){
                //printf("FOUND %s\n", key);
                //return the clique
                return bucketToGo->array[i];
            }
        }
    }
    //not found, return NULL
    //printf("NOT FOUND\n");
    return NULL;

}

//search the hashTable for a clique and set all the pointers to it as null
void setCliquePointersToNull(Bucket **ht, int stop, recordList *list, int hashSize){

    recordNode *cur = NULL;
    Clique *curClique = NULL;
    bucketNode *node = NULL;
    jsonFile *curFile = NULL;
    int hash = 0;
    //point to start of the list
    cur = list->head;
    //traverse the list
    while(cur != NULL){
        //search for the jsonFile clique
        hash = hashing1(cur->file->site, hashSize);
        //get the clique
        curClique = findEntry(ht, hash, cur->file->site);

        if(curClique == NULL){
            cur = cur->next;
            continue;
        }
        
        //if we havent already free it
        if(curClique != NULL){

            //set pointer to start
            curFile = curClique->file;
            //traverse the file list
            while(curFile != NULL){

                //get the bucketNode pointer
                hash = hashing1(curFile->site, hashSize);
                node = findEntryArrayPointer(ht, hash, curFile->site);

                //free the graph
                graphFree(node->graph);

                //set node pointer to null
                node->graph = NULL;

                curFile = curFile->next;
            }

            //free the negative node list fist
            freeNegativeNodeList(curClique->neg_node_list);
            //free the Clique
            cliqueFree(curClique);
        }

        //go to next file
        cur = cur->next;
    }
}


void case_zero(Clique *node1,Clique *node2)
{
    // when the label is 0 -> update the non matching list
    // for each clique
    insert_neg_clique(node1,node2);
    insert_neg_clique(node2,node1);

}


void fixLists(Bucket **hashtable, int hashSize, Clique *toStay, Clique *toMove){

    //init
    Negative_node *cur;
    Clique *curClique;
    Negative_node *tempcur;

    //Merge toMove list with toStay (toStay -> toMove)
    // Go to the end of toStay
    cur = toStay->neg_node_list;

    if(cur == NULL && toMove->neg_node_list == NULL){
        return;
    }

    if(cur != NULL){
        while(cur->next_ptr != NULL){
            cur = cur->next_ptr;
        }

        cur->next_ptr = toMove->neg_node_list;
    }
    else{

        // if cur != null
        // Traverse toMove list and update pointers
        toStay->neg_node_list = toMove->neg_node_list;
    }

    cur = toMove->neg_node_list;
    while(cur != NULL){

        //Get toMove negative clique
        curClique = cur->neg_clique_ptr;

        //Traverse the negative clique
        //Search for toMove clique
        tempcur = curClique->neg_node_list;
        while(tempcur != NULL){
            if (strcmp(tempcur->neg_clique_ptr->name,toMove->name) == 0){
                //Set the pointer to point to toStay
                tempcur->neg_clique_ptr = toStay;
                break;
            }
            tempcur = tempcur->next_ptr;
        } 

        cur = cur->next_ptr;
    }

}


void parcerCSV(char *path, char *csv, Bucket **hashtable, int hashSize,int opt){

    char CSVpath[100];
    char buffer[512];
    int hash = 0;
    jsonFile *cur = NULL;
    jsonFile *curCopy = NULL;
    Clique *node1 = NULL;
    Clique *node2 = NULL;
    Clique *toStay = NULL;
    Clique *toMove = NULL;
    Clique *foundNode = NULL;
    bucketNode *foundNodeArray = NULL;
    int f2 = 0;
    char *segment1,*segment2,*label;


    if(opt == 1){
        snprintf(CSVpath, sizeof(CSVpath), "./%s/%s",path,csv);
    }
    else{
        snprintf(CSVpath, sizeof(CSVpath), "%s",csv);
    }

    //printf("%s\n",CSVpath);

    //open csv file 
    FILE *csvfp;
    csvfp = fopen(CSVpath,"r");
    if(csvfp == NULL){
        perror("fopen");
        exit(-2);
    }

    memset(buffer, 0, sizeof(buffer));
    //read each line
    //skip first line
    fgets(buffer, sizeof(buffer), csvfp);
    while (fgets(buffer, sizeof(buffer), csvfp) != NULL){

        //get the first column
        segment1 = strtok(buffer,",");
        if(segment1 == NULL){
            perror("strtok");
            exit(-4);
        }
        //get the second column
        segment2 = strtok(NULL,",");
        if(segment2 == NULL){
            perror("strtok");
            exit(-4);
        }

        

        //get th third column
        label = strtok(NULL,",");
        if(label == NULL){
            perror("strtok");
            exit(-4);
        }
        


        //check if the entries is in the hashTable
        //first hash the name to narrow down the options
        hash = hashing1(segment1, hashSize);
        node1 = findEntry(hashtable, hash, segment1);
        
        if(node1 == NULL){
            printf("elouse1 %s\n",segment1);
            exit(-1);
        }

        //do the same for the second name
        hash = hashing1(segment2, hashSize);
        node2 = findEntry(hashtable, hash, segment2);

        if(node2 == NULL){
            printf("elouse2\n");
            exit(-1);
        }

        //if they are in the same clique, no work needed
        if(!strcmp(node1->name, node2->name)){
            //go to the next pair
            continue;
        }

        //if there is a negative connection
        if(atoi(label) == 0) 
        {
            case_zero(node1,node2);
            continue;
        }
        //TIME TO CREATE THE CLIQUES
        //if there is the same number of jsonFiles: node1 -> node 2
        //if node1 has more jsonFiles than node2: node1 -> node 2
        //if node2 has more jsonFiles than node1: node2 ->node1

        //case with the οne element in the list
        if(node1->cliqueSum == -1 && node2->cliqueSum == 1){

            //make node2 list of files go to the end of node1 list

            //cur points to the start of node1 list
            cur = node1->file;
            //go to the last jsonFile of node1 list
            while(cur->next != NULL){

                //go to next jsonFile
                cur = cur->next;
            }

            //merge node1 list with node 2 list
            cur->next = node2->file;

            //make node2 point to node1 list
            node2->file = node1->file;

            //fix counters
            //total jsonFiles for booth nodes are the sum
            node1->cliqueSum += node2->cliqueSum;
            //we calculated the sum so add it to node2
            node2->cliqueSum = node1->cliqueSum;

            printf("oxi\n");

        //case with multiple elements
        }else{

            //see which list has the most files
            //the node with the fewer files will be moved
            if(node1->cliqueSum >= node2->cliqueSum){
                //assign nodes to pointers
                toMove = node2;
                toStay = node1;
            }else{
                //assign nodes to pointers
                toMove = node1;
                toStay = node2;
            }

            //printf("toStay %p\n", toStay);
            //printf("toMove %p\n", toMove);
            int total = toMove->cliqueSum + toStay->cliqueSum;
            
            
            //check if either clique contains booth items
            cur = toStay->file;
            f2 = 0;
            while(cur != NULL){
                
                //both items found
                if(!strcmp(cur->site, toMove->name)){
                        f2 = 1;
                        break;
                }
                        

                cur = cur->next;
            }
            //no need to do anything
            //go to next line
            if(f2){
                //printf("edw \n");
                continue;
            }

            //we will now be working with toStay , toMove
            //the toMove list of jsonFIles will be moved at the end of toStay
            //after thst toMove will point at the first element of toStay list of jsonFiles

            //go to the end toStay list
            //cur points to the start of toStay list
            cur = toStay->file;
            //go to the last jsonFile of toStay list
            while(cur->next != NULL){

                //go to next jsonFile
                cur = cur->next;
            }

            //merge the two lists
            cur->next = toMove->file;

            //now traverse the toMove list and update all the node pointers to toStay list

            //cur now points to the first of the second list
            curCopy = toMove->file;

            //traverse the list and search the hashtable for the treeNodes
            while(curCopy != NULL)
            {

                //search the hasttable for the node with the same name
                hash = hashing1(curCopy->site, hashSize);
                //get the node
                foundNode = findEntry(hashtable, hash, curCopy->site);
                //error check
                if(foundNode == NULL){
                    printf("ELOUSA\n");
                    exit(-4);
                }

                //get the array pointer of the node
                foundNodeArray = findEntryArrayPointer(hashtable, hash, curCopy->site);

                //make the array pointer to point to toStay clique
                foundNodeArray->graph->head = toStay; 
                
                //go to next jsonFile from toMove list
                curCopy = curCopy->next;

                
            }

            fixLists(hashtable,hashSize,toStay,toMove);

            if(foundNode != NULL){

                cliqueFree(foundNode);
            }


            //fix the toStay counter
            //toStay->cliqueSum = toMove->cliqueSum;
            toStay->cliqueSum = total;

            curCopy = toStay->file;
            

            //set pointers to null
            toMove = NULL;
            toStay = NULL;
            foundNode = NULL;
            foundNodeArray = NULL;
            cur = NULL;

        }

        //clean buffer
        memset(buffer, 0, sizeof(buffer));
        //set the nodes to null
        node1 = NULL;
        node2 = NULL;
        //set cur to null
        cur = NULL;
    }

    //close file
    fclose(csvfp);

}

//writes the cliques in the results.csv file
void datasetResultsCreation(jsonFile *node,int len,FILE* fp)
{
    jsonFile* cur = node;
    jsonFile* ptr;
    char buf[200];

    //traverse all the nodes in the list
    for(int i=0; i<len-1; i++)
    {
        ptr = cur->next;
        //traverse the nodes after the current
        for(int k = i; k < len-1; k++)
        {
            //write them to the file in the specified format
            snprintf(buf,sizeof(buf),"%s, %s",cur->site,ptr->site);
            fprintf(fp,"%s\n",buf);
            ptr = ptr->next;
            
        }
        cur = cur->next;
    }
    
}

int bucketArrayPrint(bucketNode** array, int limit,FILE* fp)
{

    int counter = 0;
    Clique* tmpRoot = NULL;
    jsonFile* cur;

    for(int i = 0; i < limit; i++)
    {
        cur = NULL;
        if(array[i] != NULL){
            tmpRoot = array[i]->graph->head;
            cur = tmpRoot->file;  //ALERT ALERT WATCH OUT MAYBE I FUCK THEM ALL
            if(cur->print_flag == 1 || cur->next == NULL)
            {
                
                continue;
            }
            
            printf("_________________________________________\n");
            datasetResultsCreation(cur,tmpRoot->cliqueSum,fp);
            while(cur!=NULL)
            {

                cur->print_flag = 1;
                printf("%s\n",cur->site);
                cur = cur->next;
                counter++;
            }
            printf("_________________________________________\n");
        }
    }
    return counter;
}


void print_cliques(Bucket** ht, int stop)
{

    Bucket* cur = NULL;
    int limit = 0;
    int counter = 0;
    FILE *fp = NULL;

    //create a new file with the results
    fp = fopen("results.csv", "w");
    //error check
    if(fp == NULL){
        perror("fopen");
        exit(-2);
    }

    //write the first line of the file
    //all the lines follow this format
    fprintf(fp, "left_spec_id, right_spec_id\n");

    //for each bucket
    for(int i = 0; i<stop; i++){

        //set current
        cur  = ht[i];
        //search cur and it's links
        while(cur != NULL){

            //fix limit
            limit = cur->spaces;

            //call funct for printing
            counter = counter + bucketArrayPrint(cur->array,limit,fp);
            
            cur = cur->next;
        }
    }
    printf("counter is--------------> %d\n",counter);
    //close it
    fclose(fp);
}


//takes a clique and writes to a file all its negative jsonFiles
void printNegative(FILE *fp, Clique *clique){

    Negative_node *cur = NULL;

    //set cur to head of negative list
    cur = clique->neg_node_list;
    //traverse the negative list
    while((cur != NULL)){
        //printf("%s\n", cur->neg_clique_ptr->name);
        //check if the clique is printed
        if(cur->printed == 0){
            //write them to the file
            fprintf(fp, "%s, %s\n", clique->name, cur->neg_clique_ptr->name);
            //update printed bools
            cur->printed = 1;
            updateCliquePrinted(cur->neg_clique_ptr, clique->name, 1);
        }
        //go to next node
        cur = cur->next_ptr;
    }

}

//creates a csv and prints all the negative jsonFiles
void printNegativeCliques(Bucket **ht, int stop){

    Bucket* cur = NULL;
    int limit = 0;
    FILE *fp = NULL;

    //create a new file with the results
    fp = fopen("negativeResults.csv", "w");
    //error check
    if(fp == NULL){
        perror("fopen");
        exit(-2);
    }

    //write the first line of the file
    //all the lines follow this format
    fprintf(fp, "left_spec_id, right_spec_id\n");

    //for each bucket
    for(int i = 0; i<stop; i++){

        //set current
        cur  = ht[i];
        //search cur and it's links
        while(cur != NULL){
            //fix limit
            limit = cur->spaces;

            //for each space in the array
            for(int j = 0; j < limit; j++){
                //if there is a clique
                if(cur->array[j] != NULL){
                    //call the printing function
                    printNegative(fp, cur->array[j]->graph->head);
                }
            }
            //go to next bucket
            cur = cur->next;
        }
    }

    //close it
    fclose(fp);

}


int check_neg_cliques(Bucket** ht, char* word1, char* word2,int limit)//limit for hash
{
    //take hash num
    int hash1 = hashing1(word1,limit);
    int hash2 = hashing1(word2,limit);
    int flag1 = 0; //we found the 1st word
    int flag2 = 0; //we found the 2nd word


    //find their clique
    Clique *clique1,*clique2;
    clique1 = findEntry(ht,hash1,word1);
    clique2 = findEntry(ht,hash2,word2);

    //search for the word in each others clique
    //for word1 in clique2

    
    Negative_node *tmp_neg;
    Clique * tmp_cliq;
    jsonFile* tmp_file;

    
    //for each neg_clique at clique
    tmp_neg = clique2->neg_node_list;
    while(tmp_neg!=NULL)
    {
        //take the negative connected clique
        tmp_cliq = tmp_neg->neg_clique_ptr;

        //chech every file at clique file list
        tmp_file = tmp_cliq->file;
        while(tmp_file!=NULL)
        {
            //cmp the word1 with te site
            if(!strcmp(word1,tmp_file->site))
            {
                flag1 = 1;//means we found word1
                break;
            }
            tmp_file = tmp_file->next;
        }

        //if we found the word, dont search the othes cliques
        if(flag1 == 1)
        {
            break;
        }

        tmp_neg = tmp_neg->next_ptr;
    }


    //FOR WORD 2
    //for each neg_clique at clique
    tmp_neg = clique1->neg_node_list;
    while(tmp_neg!=NULL)
    {
        //take the negative connected clique
        tmp_cliq = tmp_neg->neg_clique_ptr;

        //chech every file at clique file list
        tmp_file = tmp_cliq->file;
        while(tmp_file!=NULL)
        {
            //cmp the word2 with te site
            if(!strcmp(word2,tmp_file->site))
            {
                flag2 = 1;//means we found word2
                break;
            }
            tmp_file = tmp_file->next;
        }

        //if we found the word, dont search the othes cliques
        if(flag2 == 1)
        {
            break;
        }

        tmp_neg = tmp_neg->next_ptr;
    }


    if(flag1 == 1 && flag2 == 1)
    {
        //all ok 
        return 1;
    }

    if(flag1 == 0 && flag2 == 0)
    {
        //we did not find any of the two words
        return 0;
    }

    if(flag1 == 1 || flag2 == 1)
    {
        //we only found the word in one clique.MAJOR PROBLEM 
        return 2;
    }

    //this command fixes a warning
    //program does not reach this line
    return 0;

}

void NegativeQliqueTest(char *path, char *csv, Bucket **hashtable, int hashSize){

    char CSVpath[100];
    char buffer[512];
    char *segment1,*segment2,*label;
    int numLabel = 1;


    //Put together the path
    snprintf(CSVpath, sizeof(CSVpath), "./%s/%s",path,csv);

    //open csv file 
    FILE *csvfp;
    csvfp = fopen(CSVpath,"r");
    if(csvfp == NULL){
        perror("fopen");
        exit(-2);
    }
    
    memset(buffer, 0, sizeof(buffer));
    //read each line
    //skip first line
    fgets(buffer, sizeof(buffer), csvfp);
    while (fgets(buffer, sizeof(buffer), csvfp) != NULL){
       // printf("%s\n",buffer);

        //get the first column
        segment1 = strtok(buffer,",");
        if(segment1 == NULL){
            perror("strtok");
            exit(-4);
        }
        //get the second column
        segment2 = strtok(NULL,",");
        if(segment2 == NULL){
            perror("strtok");
            exit(-4);
        }

        

        //get th third column
        label = strtok(NULL,",");
        if(label == NULL){
            perror("strtok");
            exit(-4);
        }

        numLabel = atoi(label);
        //check if the third column is 0
        if(numLabel == 0){
            //Search word in hashtable
            int result = 5;
            result = check_neg_cliques(hashtable,segment1,segment2,hashSize);
            //printf("Checking %s, %s     : %d\n", segment1, segment2, result);
            if(result != 1){    
                printf("!!!!!! %d\n",result);
            }
        }

    }

    //close the file
    fclose(csvfp);

    return;
}

//merge two given arrays into one
//order: start - end and return the array
double* arrayConcat(double *startArray, double *endArray, int size){

    double *array = NULL;

    //create a new array with size
    array = malloc(size * 2 *sizeof(double));
    //error check
    if(array == NULL){
        perror("malloc");
        exit(-3);
    }

    //traverse the arrays
    for(int i = 0; i < size; i++){
        
        //get euclidian distance
        //array[i] = sqrt(pow((startArray[i] - endArray[i]), 2) +  pow((endArray[i] - startArray[i]), 2) );
        //array[i] = startArray[i] + endArray[i];
        array[i] = startArray[i];
        
    }

    for(int i = size; i < (size*2); i++)
    {
         array[i] = endArray[i-size];
    }

    return array;
}


int GetCSVlineCount(char *path,char *csv){
    char buffer[512];
    char CSVpath[100];
    int lines = 0;

    snprintf(CSVpath, sizeof(CSVpath), "./%s/%s",path,csv);

    //open csv file 
    FILE *csvfp;

    csvfp = fopen(CSVpath,"r");
    if(csvfp == NULL){
        perror("fopen");
        exit(-2);
    }

    memset(buffer, 0, sizeof(buffer));
    //read each line
    //skip first line
    fgets(buffer, sizeof(buffer), csvfp);
    while (fgets(buffer, sizeof(buffer), csvfp) != NULL){

        lines++;

        memset(buffer, 0, sizeof(buffer));
    }

    //printf("The CSV has %d lines\n",lines);

    //close the file
    fclose(csvfp);
    
    return lines;
} 


logistic_reg* CreateTrainAndTest(char *path,char *csv,Bucket** ht,int hashSize, word_ht *wordHash, int size,int batch_size){

    char buffer[512];
    char CSVpath[100];
    char *segment1,*segment2,*label;
    //hashing result
    int hash;
    //logistic regression
    logistic_reg *classifier = NULL;
    //Xtrain
    double *X = NULL;
    //array index
    int i = 0;
    //file array
    double **file = NULL;
    //file results array
    int *fileResults = NULL;
    //file names array
    char **fileNameLeft = NULL;
    char **fileNameRight = NULL;
    //open csv file 
    FILE *csvfp;
    //file with the predictions
    FILE *predFp = NULL;
    jsonFile *FirstFile,*SecondFile;
    Clique *temp;

    int remaining;
    double** x_train;
    int* y_train;


    //init metrics
    positiveMetrics *P_metrics;
    negativeMetrics *N_metrics;

    P_metrics = initPositiveMetrics("possitive");
    N_metrics = initNegativeMetrics("negative");

    //LOGISTIC REGRESION MODEL

    //initialize the classifier
    //size is the concatenated size of 2 jsonFiles represented as TF-IDF matrices
    classifier = create_logistic_reg(2 * (wordHash->id_counter));

    //initialize the file array
    //same size as the number of lines inside
    file = malloc(size * sizeof(double *));
    //initialize the fileResults
    //same size
    fileResults = malloc(size * sizeof(int));
    //initialize name arrays
    fileNameLeft = malloc(size * sizeof(char *));
    fileNameRight = malloc(size * sizeof(char *));
    

    //fix the file path
    snprintf(CSVpath, sizeof(CSVpath), "./%s/%s",path,csv);
    //open the file
    csvfp = fopen(CSVpath,"r");
    if(csvfp == NULL){
        perror("fopen");
        exit(-2);
    }

    //open the prediction file
    predFp = fopen("predictions.txt","w");
    if(predFp == NULL){
        perror("fopen");
        exit(-2);
    }

    //traverse the file line by line
    //add each line to the 2d file array
    //initialize the buffer
    memset(buffer, 0, sizeof(buffer));
    //read each line
    //skip first line
    fgets(buffer, sizeof(buffer), csvfp);
    while (fgets(buffer, sizeof(buffer), csvfp) != NULL){

        //get the first column
        segment1 = strtok(buffer,",");
        if(segment1 == NULL){
            perror("strtok");
            exit(-4);
        }
        //  printf("SEGMENT 1%s\n",segment1);
    
        //Hash the key
        hash = hashing1(segment1,hashSize);
        //get the json file
        FirstFile = findJsonFromCsv(ht,hash,segment1);
        temp = findEntry(ht,hash,segment1);
        FirstFile = temp->file;
        while(FirstFile != NULL){
            if(strcmp(FirstFile->site,segment1) == 0){
                // printf("FOUND %s\n",FirstFile->site);
                break;
            }
            FirstFile = FirstFile->next;
        }


        //get the second column
        segment2 = strtok(NULL,",");
        if(segment2 == NULL){
            perror("strtok");
            exit(-4);
        }
        //printf("%s\n",segment2);

        hash = hashing1(segment2,hashSize);
        //get the json file
        SecondFile = findJsonFromCsv(ht,hash,segment2);
        temp = findEntry(ht,hash,segment2);
        SecondFile = temp->file;
        while(SecondFile != NULL){
            if(strcmp(SecondFile->site,segment2) == 0){
                //printf("FOUND %s\n",SecondFile->site);
                break;
            }
            SecondFile = SecondFile->next;
        }

        //get th third column
        label = strtok(NULL,",");
        if(label == NULL){
            perror("strtok");
            exit(-4);
        }
        // printf("%s\n",label);

        //create TF-IDF arrays for both files
        //first file
        CreateJsonListWordCountArray(FirstFile,wordHash->id_counter);
        FillJsonWordCountArray(FirstFile,wordHash->id_counter,wordHash);

        //second file
        CreateJsonListWordCountArray(SecondFile,wordHash->id_counter);
        FillJsonWordCountArray(SecondFile,wordHash->id_counter,wordHash);

        //merge the two arrays into one
        X = arrayConcat(FirstFile->JsonWordCount, SecondFile->JsonWordCount, wordHash->id_counter);
        
        //store them in the first null spot of the file array
        //its indicated by the index
        file[i] = X;
        fileResults[i] = atoi(label);

        //store the names of the files above
        fileNameLeft[i] = FirstFile->site;
        fileNameRight[i] = SecondFile->site;
        
        //we can now free the individual arrays inside the jsonFiles
        freeJsonWordCountArray(FirstFile);
        freeJsonWordCountArray(SecondFile);

        //fix index
        i++;        

        //empty the buffer
        memset(buffer, 0, sizeof(buffer));

    }

    //close the dataset file
    fclose(csvfp);

    //at this point all the file is inside the arrays
    //time to shuffle it    
    file = shuffleArray(file, fileResults, fileNameLeft, fileNameRight, size);

    //format of the predictions file
    fprintf(predFp, "Prediction, Actual Result\n");

    remaining = size * 60 / 100;

    //create a new hashtable for transitivity
    Bucket** transitivityHashtable = NULL;
    int hashresult,thashSize;
    jsonFile *tempJson,*secondTempJson;

    // testFile
    FILE *testFile;
    testFile = fopen("TransitivityFile.csv","w");

    //set the size of the hashtable with bucket size 120
    thashSize = size * 80 / 100;
    transitivityHashtable = initHashTables(thashSize,120);

    
    fprintf(testFile,"left_spec_id,right_spec_id,label\n");

    for(int j=0;j<remaining;j++){
        // keep the file lef
        tempJson = malloc(sizeof(jsonFile));
        tempJson->site = strdup(fileNameLeft[j]);
        // tempJson->JsonWordCount = file[j];
        tempJson->next = NULL;

        //keep the file right
        secondTempJson = malloc(sizeof(jsonFile));
        secondTempJson->site = strdup(fileNameRight[j]);
        // secondTempJson->JsonWordCount = file[j];
        secondTempJson->next = NULL;

        // add json in the hash table
        hashresult = hashing1(tempJson->site,thashSize);
        transitivityHashtable[hashresult] = addToHashTable(transitivityHashtable[hashresult], tempJson->site, hashresult, tempJson);

        // add the second record in the hash
        hashresult = hashing1(secondTempJson->site,thashSize);
        transitivityHashtable[hashresult] = addToHashTable(transitivityHashtable[hashresult], secondTempJson->site, hashresult, secondTempJson);

        fprintf(testFile, "%s,%s,%d\n",tempJson->site,secondTempJson->site,fileResults[j]);

    }

    fclose(testFile);

    parcerCSV("a","TransitivityFile.csv",transitivityHashtable,thashSize,5);

    int current = 0;
    printf("\nTraining the Logistic Regression Classifier...\n");
    
    // while(remaining!=0)
    // {
    //     if(remaining < batch_size)
    //     {
    //         batch_size = remaining;
    //     }
        
    //     //allocate the arrays
    //     x_train  = malloc(sizeof(double*) * batch_size);
    //     y_train = malloc(sizeof(int) * batch_size);

    //     for(int i=0; i < batch_size; i++)
    //     {
    //         x_train[i]= file[current];
    //         y_train[i] = fileResults[current];
    //         current+=1;
    //     }

    //     //fit data
    //     fit(classifier, x_train, y_train,2 * (wordHash->id_counter),batch_size);   
    //     //train with the current file 10 times
    //     printf("Current Cost: %f\n", cost_function(classifier));
    //     free(y_train);
    //     free(x_train);
    //     remaining -= batch_size;
    // }

    classifier = logisticRegretionAlgorithm(classifier, 1, ht, hashSize, wordHash,file,fileResults,size,batch_size, transitivityHashtable, thashSize);

    current = size * 60 / 100;
    printf("\nTraining Completed.\n");
    printf("\nGenerating test results...\n");
    printf("start: %d       finish: %d", current, (size * 80 / 100));
    for(int i = current;i < (size * 80 / 100); i ++)
    {
        //test
        logisticRegrationTest(classifier, file[i], fileNameLeft[i], fileNameRight[i], fileResults[i], predFp, P_metrics, N_metrics);
    }




    // //traverse the array and feed it to the classifier
    // for(int i = 0; i < size; i++){

    //     //if its the first 80%
    //     //go to train
    //     if(i < (size * 60 / 100)){
            
    //         //message
    //         if(i == 0){
    //             printf("\nTraining the Logistic Regression Classifier...\n");
    //         }

    //         //fit data
    //         fit(classifier, file[i], fileResults[i], wordHash->id_counter);   
    //         //train with the current file 10 times
    //         classifier = logisticRegretionAlgorithm(classifier, 10);

    //         if(i % 1000 == 0){
    //             printf("Current Cost: %f\n", cost_function(classifier));
    //         }

    //     //else start testing
    //     }else{
            
    //         if(i == (size * 80 / 100 + 1)){
    //             printf("\nTraining Completed.\n");
    //             printf("\nGenerating test results...\n");
    //         }

    //         //test
    //         logisticRegrationTest(classifier, file[i], fileNameLeft[i], fileNameRight[i], fileResults[i], predFp, P_metrics, N_metrics);

    //     }

    // }

    printf("\nTesting Completed.\n");

    //get metrics
    P_metrics = evaluatePositiveMetrics(P_metrics);
    N_metrics = evaluateNegativeMetrics(N_metrics);

    printPositiveMetrics(P_metrics);
    printNegativeMetrics(N_metrics);



    //free results array
    free(fileResults);
    //free file array
    for(int i = 0; i < size; i++){

        free(file[i]);

    }
    free(file);
    //free left names
    free(fileNameLeft);
    //same for right
    free(fileNameRight);
    //close the prediction file
    fclose(predFp);

    freeBuckets(transitivityHashtable, thashSize);

    //free metrics 
    // freePositiveMetrics(&P_metrics);
    // freeNegativeMetrics(&N_metrics);


    //return the classifier
    return classifier;

}