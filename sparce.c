#include "sparce.h"
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

    node = createSparceNode(i, v);
    //add it to head
    node->next = matrix->head;
    matrix->head = node;
}

sparceMatrix **createMiniFile(sparceMatrix **miniFile, double **file, int size, int lineSize){

    double *array;

    //allocate the array
    miniFile = malloc(size * sizeof(sparceMatrix*));

    //for each array
    for(int i = 0; i < size; i++)
    {
        //create a sparce matrix
        miniFile[i] = createSparceMatrix();
        array = file[i];
        //for each line
        for(int j = 0; j < lineSize; j++)
        {
            //if the element is not zero
            if(array[j] != 0.0){
                // add it to the sparce matrix
                insertMatrixNode(miniFile[i], j, array[j]);
            }
        }
        //free unsused structs
        free(file[i]);
        
    }

    free(file);
    
    // return the file
    return miniFile;
}

sparceMatrix * create_sparce_matrix(double* array, int lineSize)
{
    
    // for(int i = 0; i < lineSize; i++){

    //     printf("%f\n", array[i]);

    // }

    //allocate the array
    sparceMatrix* miniFile = createSparceMatrix();
    //for each line
    for(int j = 0; j < lineSize; j++){
        //if the element is not zero
        if(array[j] != 0.0){
            // add it to the sparce matrix
            insertMatrixNode(miniFile, j, array[j]);
        }
    }

    // sparceNode *cur = NULL;
    // cur = miniFile->head;
    // while(cur != NULL){
    //     printf("%d      %f\n", cur->index, cur->value);
    //     cur = cur->next;
    // }
    // return the file
    return miniFile;
}


double sparce_search(sparceMatrix* matrix,int index)
{
    sparceNode* cur = matrix->head;
    while(cur!= NULL)
    {
        if(cur->index == index)
        {
            return cur->value;
        }

        if(index > cur->index)
        {
            return 0.0;
        }
        cur = cur->next;
    }
    return 0.0;
}

void deleteSparceNode(sparceNode *node){

    free(node);

}

void deleteSparceMatrix(sparceMatrix *matrix){

    sparceNode *node = matrix->head;
    sparceNode *cur = matrix->head;

    while(cur != NULL){

        node = cur;
        cur = cur->next;
        deleteSparceNode(node);
    }
    
    free(matrix);
}