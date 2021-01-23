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

typedef struct sparceMatrix{

    //pointer to the first node
    
    sparceNode *head;

}sparceMatrix;


typedef struct SparceListNode{

    sparceMatrix *matrix;
    int result;
    struct SparceListNode *next;
}SparceListNode;


typedef struct SparceList{
    
    int counter;

    SparceListNode *head;
    
}SparceList;


sparceNode *createSparceNode(int i, double v);
sparceMatrix *createSparceMatrix();
sparceMatrix **createMiniFile(sparceMatrix **miniFile, double **file, int size, int lineSize);
double sparce_search(sparceMatrix*,int );
sparceMatrix * create_sparce_matrix(double* array, int lineSize);
void deleteSparceMatrix(sparceMatrix *matrix);
SparceListNode *createSparceListNode(sparceMatrix *matrix,int result);
SparceList *createSparceList();
void InsertSparceListNode(SparceList *list,sparceMatrix *matrix,int result);
#endif