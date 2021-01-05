#include <stdio.h>
#include "jsonStruct.h"
#include <string.h>
#include <stdlib.h>
#include <dirent.h>
#include "hashTable.h"
#include "ctype.h"

//most xcommon english words
//if encountered they are skipped
char *stopwords[] = {"a","able","about","across","after","all","almost","also","am","among","an","and","any","are",
            "as","at","be","because","been","but","by","can","cannot","could","dear","did","do","does",
            "either","else","ever","every","for","from","get","got","had","has","have","he","her","hers",
            "him","his","how","however","i","if","in","into","is","it","its","just","least","let","like",
            "likely","may","me","might","most","must","my","neither","no","nor","not","of","off","often","on",
            "only","or","other","our","own","rather","said","say","says","she","should","since","so","some",
            "than","that","the","their","them","then","there","these","they","this","tis","to","too","twas",
            "us","wants","was","we","were","what","when","where","which","while","who","whom","why","will",
            "with","would","yet","you","your"};

int isStopWord(char *array[], char *key){

    //traverse the array
    for(int i = 0; i < 119; i++){
        //check if its stopword
        if(!strcmp(array[i],key)){
            return 1;
        }
    }
    //if not return 0
    return 0;
}



char* wordFormating(char *text){

    //word array pos
    int p = 0;
    //to store the new word
    char *newWord = strdup(text);
    char *newLine = NULL;

    //check for new line
    if((newLine = strstr(newWord, "\n"))!= NULL){
        //replace it with space
        newLine[0] = ' ';
    }
    newLine = NULL;

    //check for the appearence of \n inside the text
    if((newLine = strstr(newWord, "\\n"))!= NULL){
        //replace \ with space
        newLine[0] = ' ';
        //replace n with space
        newLine[1] = ' ';
    }
    newLine = NULL;

    //if its a utf character we dont need it
    //starts with u then a number follows
    if(strstr(newWord, "\\u")){
        
        free(newWord);
        return NULL;
       
    }

    //traverse all the letters
    for(int i = 0; i < strlen(newWord); i++){
        //if it's not a punct
        if(!ispunct(newWord[i])){
            //copy it as lowercase
            newWord[p] = tolower(newWord[i]);
            p++;
        //if it is, replace it with a spcace
        }else{
            //add space at the punct location
            newWord[p] = ' ';
            p++;
        }
    }

    //add end of word
    newWord[p] = '\0';

    //return the word
    return newWord;
}

//function that creates a list of words froma jsonFile
//removes stopwords, punctuation
void textFormating(jsonFile *file, word_ht *ht){

    //to traverse
    jsonNode *cur = NULL;
    //to store words
    char *text = NULL;
    char *temp = NULL;
    char *tempDesc = NULL;
    char *brokenDesc = NULL;
    char *toFree = NULL;
    char *toFreeText = NULL;
    //mode
    int mode = 0;
    //set cur to head
    cur = file->head;
    //wordNode
    wordNode *node = NULL;

    //traverse all the jsonNodes
    while(cur != NULL){

        //take the desc
        text = strdup(cur->desc);
        //set pointer for free
        toFreeText = text;
        //printf("Desc: %s\n", text);

        //traverse it word by word
        while((temp = strsep(&text, " "))!= NULL){
            if(strlen(temp) <= 1){
                continue;
            }
            //format
            tempDesc = wordFormating(temp);
            //set to free pointer
            toFree = tempDesc;
            //printf("word after format:  %s\n", tempDesc);
            if(toFree != NULL){

                //if its 2 or more words we must break it again
                while((brokenDesc = strsep(&tempDesc, " ")) != NULL){
                    //if its a single letter skip it
                    //or bigger than 15 letters
                    if(strlen(brokenDesc) > 1 && strlen(brokenDesc) < 15){

                        //if ot's not a stopword add it to the list
                        if(!isStopWord(stopwords, brokenDesc)){
                            
                            //printf("word: %s\n", brokenDesc);
                            //create a wordNode
                            node = createWordNode(brokenDesc);
                            //add it to the list
                            mode = insertWordNode(file->words, node);
                            //add it to the hashTable
                            insert_pair_hash_table(ht, brokenDesc);
                            //if the word want in the list 
                            //we have to add the counter for the current word
                            //it is located in the wordHash
                            if(mode == 1){
                                addWordHashFileCounter(ht, brokenDesc);
                            }
                        }
                    }
                    //set broken to null
                    brokenDesc = NULL;
                }
            }
            //free tempDesc
            if(toFree != NULL){
                free(toFree);
            }
            if(tempDesc != NULL){
                free(tempDesc);
            }
            tempDesc = NULL;
            toFree = NULL;

            //set temp to NULL
            temp = NULL;
        }

        //free txt
        free(toFreeText);
        if(text != NULL){
            free(text);
        }
        //set it to null
        text = NULL;
        toFreeText = NULL;

        //go to next file
        cur = cur->next;
    }  

}

//folder counter
// Datasets/2013_camera_specs
//counts # of subfolders inside a folder
int folderCounter(char *path, int mode){

    DIR *dr = opendir(path);
    struct dirent *de;

    // Open the folder
    if(dr == NULL){
        printf("Error Opening Directory\n");
        exit(-1);
    }
    //count all subfolders (-2 cause it counts 2 more)
    int counter = 0;
    while ((de = readdir(dr)) != NULL){
        //if its a folder
        if(de->d_type == mode){
            counter++;
        }
    }

    //close folder
    closedir(dr);
    free(de);

    //if we were counting folders we must remove 2
    //we dont want to count . , ..
    if(mode == 4){
        counter -= 2;
    }

    //printf("the items in the folder %s are %d\n",path, counter);
    
    //return results
    return counter;

}

int countFiles(char *path){

    //open folder at path
    DIR *dr;
    struct dirent *de;
    int counter = 0;
    char newpath[500];

    dr = opendir(path);
    if(dr == NULL){
        printf("Error Opening Directory\n");
        exit(-1);
    }

    //traverse the files inside
    while ((de = readdir(dr)) != NULL){
        //if its a folder
         if(strcmp(de->d_name,".") != 0 && strcmp(de->d_name,"..") != 0){

            //open it and count using the function
            //we need a new path
            snprintf(newpath, sizeof(newpath), "%s/%s", path, de->d_name);
            //call the function and save the number
            //the code for files is 8
            counter += folderCounter(newpath, 8);

            //empty the buffer
            memset(newpath, 0, sizeof(newpath));
        }
    }

    //close the dir
    closedir(dr);

    //return the number
    printf("There are %d total files in the Dataset.\n", counter);

    return counter;
}

//merge two stings together
char* concat(const char *s1, const char *s2){
    
    char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator

    strcpy(result, s1);
    strcat(result, s2);

    return result;
}

//handles the case where the category is ""
void noTitleCase(char *text, jsonFile *list, FILE *fp){

    char *temp = NULL;
    char *desc = NULL;
    char *key = NULL;
    char *segment = NULL;
    int i = 0;
    char buffer[500];
    int flag = 1;
    jsonNode *node = NULL;

    //case that desc is an array [ ... ]

    if(strstr(text, "[") != NULL){
        
        while (fgets(buffer, sizeof(buffer), fp) != NULL){

            key = NULL;

            //we are at the end of the array
            if(strstr(buffer, "],") != NULL){
                //printf("end \n");
                break;
            }
            //printf("3. %s\n", buffer);
            //first line goes in with strdup
            if(flag == 1){
                desc = strdup(buffer);
                flag = 0;
                //printf("4. %s\n", desc);
            }else{
                //the rest with strcat
                temp = concat(desc, buffer);
                //printf("5. %s", temp);
                free(desc);
                desc = strdup(temp);
                free(temp);
            }

            //empty thr buffer
            memset(buffer, 0, sizeof(buffer));

        }

        //printf("%s\n", desc);

    //here is the case that desc is ".........."
    }else{
        //get the category name
        segment = strtok(NULL, "\",");

        //traverse the line
        while(segment != NULL){

            //the first time strdup is required
            if(i == 0){
                desc = strdup(segment);
            //after that we use concat
            }else{
                //merge the segments
                temp = concat(desc, segment);
                //free previus version
                free(desc);
                //update description
                desc = strdup(temp);
                free(temp);

            }
            //get next segment
            segment = strtok(NULL, "\",");
            i++;

        }

    }

    //assign a title
    key = strdup("None");
    //printf("%s\n", desc);
    //create a node
    node = createJsonNode(key, desc);
    free(key);
    free(desc);
    //add it to the list
    insertJsonFile(list, node);

}

jsonFile* parcer(FILE *fp, char *site, char *jsonName, word_ht *ht){

    //line buffer
    char buffer[100000];
    char name[500];
    char *segment;
    char *key = NULL;
    char *desc = NULL;
    int flag;
    char *temp;
    jsonNode *node = NULL;
    jsonFile *list = NULL;

    //name must be like this: site//jsonFileName (without .json at the end)
    snprintf(name, sizeof(name), "%s//%s", site, jsonName);
    //remove .json (4 letters)
    name[strlen(name) - 5] = '\0';

    //create a list
    list = createJsonFile(name);

    //skip the first line
    fgets(buffer, sizeof(buffer), fp);
    memset(buffer, 0, sizeof(buffer));

    //traverse all the lines inside the file
    while (fgets(buffer, sizeof(buffer), fp) != NULL){

        key = NULL;
        desc = NULL;

        //unused
        segment = strtok(buffer, "\"");
        //skip first line
        if(strstr(segment, "}")){
            continue;
        }

        //get the field name
        segment = strtok(NULL, "\"");
        //check if there is a name
        if( (strstr(segment, ": ") != NULL && strlen(segment) <= 3) || (strstr(segment, ": [") != NULL && strlen(segment) <= 4)){
            //printf("SEGMENT |%s|\n", segment);

            //handle the special case
            noTitleCase(segment, list, fp);
            //go to next line
            continue;
        }

        //add it as a key
        key = strdup(segment);
 
        //get the desc
        segment = strtok(NULL, ":");
        
        //if its a single line
        if(strstr(segment, "\"") != NULL){
            
            desc = strdup(segment);

            segment = strtok(NULL, "\",");
            //check if there is rest of string
            while(segment != NULL){
                //merge strings
                temp = concat(desc, segment);
                //printf("5. %s", temp);
                free(desc);
                desc = strdup(temp);
                free(temp);

                segment = strtok(NULL, "\",");
            }
            //delete the first 2 characters
            desc += 2;
            char *toremove = strdup(desc);
            desc -= 2;
            free(desc);
            desc = strdup(toremove);
            free(toremove);

            //delete last characters
            if(strstr(desc, "\",") != NULL){
                desc[strlen(desc) -3] = '\0';
            }

            //printf("1. %s\n", desc);
        
        //if its [ we must add until we see ],
        }else{
            //printf("2. %s\n", buffer);
            //no need to add the first line
            //its only " [ "
            //empty the buffer
            memset(buffer, 0, sizeof(buffer));
            //read next line
            flag = 1;
            while (fgets(buffer, sizeof(buffer), fp) != NULL){

                //we are at the end of the array
                if(strstr(buffer, "],") != NULL){
                    //printf("end \n");
                    break;
                }
                //printf("3. %s\n", buffer);
                //first line goes in with strdup
                if(flag == 1){
                    desc = strdup(buffer);
                    flag = 0;
                    //printf("4. %s\n", desc);
                }else{
                    //the rest with strcat
                    temp = concat(desc, buffer);
                    //printf("5. %s", temp);
                    free(desc);
                    desc = strdup(temp);
                    free(temp);
                }

                //empty thr buffer
                memset(buffer, 0, sizeof(buffer));

            }

            //delete last 2 characters
            desc[strlen(desc) -3] = '\0';
        }

        //create a node
        node = createJsonNode(key, desc);

        if(desc != NULL)    free(desc);
        //add it to the list
        //if its page - title add is as head
        if(strstr(key, "page title") != NULL){
            insertJsonFileHead(list, node);
        }
        else{
            insertJsonFile(list, node);
        }
        //empty the buffer
        memset(buffer, 0, sizeof(buffer));
        //free the variables used
        if(key != NULL){
            free(key);
        }
        
    }

    textFormating(list, ht);
    //the nodes with the desc are no longer needed
    //we can now delete them
    freeJsonNodeList(list);
    return list;

}