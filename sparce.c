#include "sparce.h";
#include <stdio.h>
#include <stdlib.h>

sparceNode *createSparceNode(int i, double v){

    sparceNode *node = NULL;

    node = malloc(sizeof(sparceNode));
    node->index = i;
    node->value = v;
    node->next = NULL;
    return node;
}

sparceMatrix *createSparceMatrix(){

    sparceMatrix *matrix = NULL;

    matrix = malloc(sizeof(sparceMatrix));
    matrix->head = NULL;

    return matrix;
}

void insertMatrixNode(sparceMatrix *matrix, int i, double v){

    //create node
    sparceNode *node = NULL;

    createSparceMatrix(node);
    //add it to head
    node->next = matrix->head;
    matrix->head = node;
}

void createMiniFile(sparceMatrix **miniFile, double **file, int size){

    //allocate the array
    miniFile = malloc(size * sizeof(sparceMatrix*))

}