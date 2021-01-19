#ifndef GRAPH
#define GRAPH

#include "jsonStruct.h"
#include <stdbool.h>



struct negative_node;



//the clique stucture
typedef struct clique{

    //primary key
    char *name;
    jsonFile* file;
    int printed;
    int cliqueSum;
    
    //ptr to negative clique list head
    struct negative_node *neg_node_list;

}Clique;


//node for list with negative cliques
typedef struct negative_node
{
   
    //pointer to negative connected clique
    struct clique *neg_clique_ptr; 

    //flag to see if its already printed
    bool printed;

    //flag used to distinguish all the unique pairs
    int pair;

    //ptr to next
    struct negative_node *next_ptr;
    
}Negative_node;



//clique functions
Clique* createClique(jsonFile *file);
void cliqueFree(Clique *c);

//graph structure
typedef struct graph{

    //pointer to the first graph node
    Clique *head;

}Graph;

//graph functions
Graph* createGraph();
void graphInsert(Graph *g,jsonFile *key);
void graphPrint(Graph *g);
void graphFree(Graph *graph);
jsonFile *searchClique(Clique *clique, char *site);
Negative_node*  create_negative_node(Clique * neg_clique);
int insert_neg_clique(Clique * neg_clique,Clique* this_clique);
void updateCliquePrinted(Clique *clique, char *key, int value);
void freeNegativeNodeList(Negative_node *head);
void free_negNode_list(Negative_node* ptr);

#endif
