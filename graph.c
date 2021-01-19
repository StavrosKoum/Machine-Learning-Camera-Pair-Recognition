#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "graph.h"

//Clique Constructor
Clique* createClique(jsonFile *file){

    Clique *temp = NULL;
    //allocate
    temp = (Clique *) malloc(sizeof(Clique));
    //error check
    if(temp == NULL){
        perror("malloc");
        exit(-3);
    }

    //set name
    temp->name = NULL;
    temp->name = strdup(file->site);

    temp->neg_node_list = NULL;
    //set clique sum
    temp->cliqueSum = 1;
    //set file pointer
    temp->file = file;
    //set printed
    temp->printed = 0;
  
    //return it
    return temp;
}

void free_negNode_list(Negative_node* ptr)
{
    Negative_node* free_ptr;
    free_ptr = ptr;

    while (ptr!=NULL)
    {
        free_ptr = ptr;
        ptr = ptr->next_ptr;
        free(free_ptr);
    }
    
}

//Clique Destructor
void cliqueFree(Clique *c){

    //if it's null do nothing
    if(c == NULL){
        return;
    }

    //free the name
    free(c->name);
    //free the list of negative nodes
    //freeNegativeNodeList(c->neg_node_list);
    //free the clique
    free(c);

}

//Graph Constructor
Graph* createGraph(){

    Graph *gr = NULL;
    //allocate
    gr = malloc(sizeof(Graph));
    //error check
    if(gr == NULL){
        perror("malloc");
        exit(-3);
    }

    //init
    gr->head = NULL;
    //return
    return gr;

}

//insert a Clique to the Graph
void graphInsert(Graph *g,jsonFile *key){

    //create a clique
    Clique *c = createClique(key);

    //always insert at head

    //if its empty
    if(g->head == NULL){
        g->head = c;
        return;
    }

    
}




void graphFree(Graph *graph){

    free(graph);
}


Negative_node*  create_negative_node(Clique * neg_clique)
{
    Negative_node *ptr = malloc(sizeof(Negative_node));
    ptr->neg_clique_ptr = neg_clique;
    ptr->printed = false; 
    ptr->pair = 0;
    ptr->next_ptr = NULL;
    return ptr;
}


int search_for_neg_clique(Negative_node *neg_list,Clique* neg_clique)
{
    Negative_node* cur = neg_list;

    while(cur!=NULL)
    {
        if(!strcmp(cur->neg_clique_ptr->name,neg_clique->name))
        {
            return 1;
        }
        cur = cur->next_ptr;
    }
    return 0;
}


int insert_neg_clique(Clique * neg_clique,Clique* this_clique)
{
    //check if there is no list
    if(this_clique->neg_node_list==NULL)
    {
        //create new neg node and insert
        this_clique->neg_node_list = create_negative_node(neg_clique);
        //return 1 when we make a new neg connection
        return 1;
    }

    //search if neg clique exists 
    int i = search_for_neg_clique(this_clique->neg_node_list,neg_clique);

    if(i == 1)//if it exists already
    {
        return 0;
    }
    Negative_node* temp = create_negative_node(neg_clique);
    temp->next_ptr = this_clique->neg_node_list;
    this_clique->neg_node_list = temp;
    return 1;

}


//searches the negCliqueList for a Clique and updates the printed variable
void updateCliquePrinted(Clique *clique, char *key, int value){

    //negNode pointer
    Negative_node *cur = NULL;

    //set cur to head of the list
    cur = clique->neg_node_list;

    //traverse the list
    while(cur != NULL){

        //if its the clique we want
        if(!strcmp(cur->neg_clique_ptr->name, key)){
            //update bool with value
            cur->neg_clique_ptr->printed = value;
            return;
        }
        cur = cur->next_ptr;
    }
}

//free for negative nodes

void freeNegativeNodeList(Negative_node *head){

    //pointers used in free
    Negative_node *cur = NULL;
    Negative_node *prev = NULL;

    //set pointers to head
    cur = head;
    prev = head;

    //traverse the list
    while(cur != NULL){

        //move cur to the next node
        cur = cur->next_ptr;
        //free the previus
        free(prev);
        //make prev equal to cur
        prev = cur;

    }

}

//search the clique for the site and return the json
jsonFile *searchClique(Clique *clique, char *site){

    jsonFile *cur = clique->file;

    //search all the files
    while (cur != NULL){
        
        //if its the one we want
        if(!strcmp(cur->site, site)){
            return cur;
        }

        //go to the nex
        cur = cur->next;
    }
    
    //not found
    return NULL;
}