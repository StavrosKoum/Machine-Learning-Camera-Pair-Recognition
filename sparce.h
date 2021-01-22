#ifndef SPARCE
#define SPARCE

//sparce node
typedef struct sparceNode{

    //index
    int index;
    //value
    double value;
    //pointer to next
    struct sparceNode *next;

}sparceNode;

typedef struct spraceMatrix{

    //pointer to the first node
    sparceNode *head;

}sparceMatrix;

sparceNode *createSparceNode(int i, double v);
sparceMatrix *createSparceMatrix();
sparceMatrix **createMiniFile(sparceMatrix **miniFile, double **file, int size, int lineSize);

#endif